#include "CR2ImageData.h"
#include "CR2IDFEntry.h"
#include "CR2IDFFrame.h"
#include "CR2MakerNote.h"
#include "Constants.h"
#include "CR2FileImpl.h"
#include <CR2Decoder.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdexcept>

static void _C2DReadIDFEntry(CR2IDFEntry* entry, std::ifstream& fStream);

static void _C2DDecodeIDFEntry(CR2IDFFrame* frame, CR2IDFEntry* idfEntry, std::ifstream& inStream)
{
	switch (idfEntry->TagID)
	{
		case CR2_IFD_TAG_IMAGE_WIDTH:
			frame->ImageWidth = idfEntry->Value;
			break;

		case CR2_IFD_TAG_IMAGE_HEIGHT:
			frame->ImageHeight = idfEntry->Value;
			break;

		case CR2_IFD_TAG_BITS_PER_SAMPLE:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			
			if (frame->BitsPerSample){
				throw std::runtime_error(CR2ERR_IDF_BPS_ALREADY_ALLOCATED);
			}
			frame->BitsPerSampleLen = idfEntry->Values;
			frame->BitsPerSample = new uint16_t[frame->BitsPerSampleLen];
			inStream.read((char*)frame->BitsPerSample, frame->BitsPerSampleLen * sizeof(uint16_t));
			
			inStream.seekg(curPos);
			break;
		}
		case CR2_IFD_TAG_MAKE:
		{
			const std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			char* buff = new char[idfEntry->Values];
			inStream.read(buff, idfEntry->Values);
			frame->Make = buff;
			delete[] buff;
			inStream.seekg(curPos);
			break;
		}

		case CR2_IFD_TAG_MODEL:
		{
			const std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			char* buff = new char[idfEntry->Values];
			inStream.read(buff, idfEntry->Values);
			frame->Model = buff;
			delete[] buff;
			inStream.seekg(curPos);
			break;
		}

		case CR2_IFD_TAG_STRIP_OFFSET:
		{
			const std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);

			if (frame->StripOffsets){
				throw std::runtime_error(CR2ERR_IDF_STRIP_OFFSETS_ALREADY_ALLOCATED);
			}

			frame->StripOffsetsLen = idfEntry->Values;
			frame->StripOffsets = new uint32_t[frame->StripOffsetsLen];
			// inStream.read((char*)frame->StripOffsets, frame->StripOffsetsLen * sizeof(uint32_t));		// TODO: Unsure if this is correct - needs checking
			frame->StripOffsets[0] = idfEntry->Value;
			inStream.seekg(curPos);
			break;
		}

		case CR2_IFD_TAG_STRIP_BYTE_COUNTS:
		{
			frame->StripByteCounts = idfEntry->Value;
			break;
		}

		case CR2_IFD_TAG_X_RESOLUTION:
		{
			const std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			inStream.read((char*)&frame->XResolution, sizeof(unsigned long));
			inStream.seekg(curPos);
			break;
		}
			
		case CR2_IFD_TAG_Y_RESOLUTION:
		{
			const std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			inStream.read((char*)&frame->YResolution, sizeof(unsigned long));
			inStream.seekg(curPos);
			break;
		}

		case CR2_IFD_TAG_RESOLUTION_UNIT:
			frame->ResolutionUnit = idfEntry->Value;
			break;

		case CR2_IFD_TAG_ORIENTATION:
			frame->Orientation = idfEntry->Values;
			break;

		case CR2_IFD_TAG_COMPRESSION:
		{
			frame->Compression = idfEntry->Value;
			break;
		}

		case CR2_IFD_TAG_DATE_TIME:
		{
			const std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			char buff[CR2_IDF_TAG_DATE_TIME_SIZE];
			inStream.read(buff, CR2_IDF_TAG_DATE_TIME_SIZE);
			frame->dateTime = std::string(buff);
			inStream.seekg(curPos);	
			break;
		}

		case CR2_IFD_TAG_THUMB_OFFSET:
			frame->ThumbnailOffset = idfEntry->Value;
			break;

		case CR2_IFD_TAG_THUMB_LENGTH:
			frame->ThumbnailLength = idfEntry->Value;
			break;

		case CR2_IDF_TAG_ARTIST:
		case CR2_IDF_TAG_HOST_COMPUTER:
		case CR2_IDF_TAG_XMP:
		case CR2_IDF_TAG_COPYRIGHT:
		break;

		case CR2_IFD_TAG_EXIF:
		{
			frame->ExifSubDirOffset = idfEntry->Value;
			break;
		}

		case CR2_IFD_TAG_PHOTOMETRIC_INTERPRETATION:
			frame->PhotoMetricInterpretation = idfEntry->Value;
			break;

		case CR2_IFD_TAG_SAMPLES_PER_PIXEL:
			frame->SamplesPerPixel = idfEntry->Value;
			break;

		case CR2_IFD_TAG_ROWS_PER_STRIP:
			frame->RowsPerStrip = idfEntry->Value;
			break;

		case CR2_IFD_TAG_PLANAR_CONFIGURATION:
			frame->PlanarConfiguration = idfEntry->Value;
			break;
			
		case CR2_IFD_TAG_RAW_IMAGE_SEGMENTATION:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			inStream.read((char*)frame->RawImageSegmentation, sizeof(frame->RawImageSegmentation));
			int sizereminder = *(frame->RawImageSegmentation + 1);
			inStream.seekg(curPos);
			break;
		}

		default:
			const std::string error = std::string(CR2ERR_IDF_UNKNOWN_TAG_ID) + ": " + std::to_string(idfEntry->TagID);
			// throw std::runtime_error(error);	// TODO: this needs to be handled better. Either add all possible tags, or write warnings instead of throwing errors
			break;
	}
}

static void _C2DDecodeExifEntry(CR2ExifData* exifData, CR2IDFEntry* idfEntry, std::ifstream& inStream)
{
	switch (idfEntry->TagID)
	{
			case CR2_EXIF_TAG_EXPOSURE_TIME:
			{
				std::streampos curPos = inStream.tellg();
				inStream.seekg(idfEntry->Value);
				
				inStream.read((char*)&exifData->ExposureTime, sizeof(exifData->ExposureTime));
				inStream.seekg(curPos);
				break;
			}

			case CR2_EXIF_TAG_f_NUMBER:
			{
				std::streampos curPos = inStream.tellg();
				inStream.seekg(idfEntry->Value);
				inStream.read((char*)&exifData->fNumber, sizeof(exifData->fNumber));
				inStream.seekg(curPos);
				break;
			}

			case CR2_EXIF_TAG_MAKERNOTE_OFFSET:
			{
				exifData->MakerNoteoffset = idfEntry->Value;
				break;
			}
		default:
			const std::string error = std::string(CR2ERR_IDF_UNKNOWN_TAG_ID) + ": " + std::to_string(idfEntry->TagID);
			// throw std::runtime_error(error);	// TODO: this needs to be handled better. Either add all possible tags, or write warnings instead of throwing errors
			break;
	}
}


static void _C2DDecodeMakerNoteEntry(CR2MakerNote* makerNote, CR2IDFEntry* idfEntry, std::ifstream& inStream)
{
	switch (idfEntry->TagID)
	{
		case CR2_MAKERNOTE_TAG_CAMERA_SETTINGS:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			makerNote->CameraSettings = new uint16_t[idfEntry->Values];
			inStream.read((char*)makerNote->CameraSettings, idfEntry->Values * sizeof(uint16_t));
			inStream.seekg(curPos);
			break;
		}

		case CR2_MAKERNOTE_TAG_FOCAL_LENGTH:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			makerNote->FocalLength = new uint16_t[idfEntry->Values];
			makerNote->FocalLengthLen = idfEntry->Values;
			inStream.read((char*)makerNote->FocalLength, idfEntry->Values * sizeof(uint16_t));
			inStream.seekg(curPos);
			break;
		}

		case CR2_MAKERNOTE_TAG_IMAGE_TYPE:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			
			char* buffer = new char[idfEntry->Values];
			inStream.read(buffer, idfEntry->Values);
			makerNote->ImageType = buffer;
			delete[] buffer;
			inStream.seekg(curPos);
			break;
		}

		default:
			const std::string error = std::string(CR2ERR_IDF_UNKNOWN_TAG_ID) + ": " + std::to_string(idfEntry->TagID);
			// TODO: add this to the logging system
			break;
	}	
}

/* private API */
static void _C2DDecodeHeader(CR2Header* header, std::ifstream& fStream)
{
	fStream.read((char*)&header->ByteOrdering, 2);
	fStream.read((char*)&header->TiffMagicWord, 2);
	fStream.read((char*)&header->TiffOffset, 4);

	fStream.read((char*)&header->CR2MagicWord + 1, 1);				// this works but it's probably wrong, probably has to do with byteordering
	fStream.read((char*)&header->CR2MagicWord, 1);					//  needs to be tested with MM byte ordering

	fStream.read((char*)&header->CR2VerMajor, 1);
	fStream.read((char*)&header->CR2VerMinor, 1);
	fStream.read((char*)&header->RawIFDOffset, 4);

	const short byteOrdering = header->ByteOrdering[0] << 8 | header->ByteOrdering[1];

	if (byteOrdering != CR2_HEADER_BYTE_ORDERING_LITTLE_ENDIAN && byteOrdering != CR2_HEADER_BYTE_ORDERING_BIG_ENDIAN)
	{
		throw std::runtime_error(CR2ERR_HEADER_FAULTY_BYTE_CODE);
	}

	if (header->CR2MagicWord != CR2_HEADER_MAGIC_WORD)
	{
		throw std::runtime_error(CR2ERR_HEADER_FAULTY_CR2_MAGIC_WORD);
	}
}

static void _C2DReadIDFEntry(CR2IDFEntry* entry, std::ifstream& fStream)
{
	if (sizeof(CR2IDFEntry) == 12)
	{
		fStream.read( (char*)entry, 12);
	}
	else
	{
		throw std::runtime_error(CR2ERR_UNIMPLEMENTED);	// manually read the 12 bytes in case there was memory padding added, this is just a fallback in case the struct is not exacly the size of idf
	}
}

CR2File* C2DLoad(const char* path)
{
	std::ifstream* inStream = new std::ifstream(path, std::ios::in | std::ios::binary);
	if (inStream->fail()){
		delete inStream;
		throw std::runtime_error(CR2ERR_FILE_NOT_FOUND);
	}
	
	CR2File* f = new CR2File();
	f->InStream = inStream;
	f->ImageData = new CR2ImageData();
	
	_C2DDecodeHeader(&f->ImageData->Header, *f->InStream);

	//=============================================================
	//							IDF #0							 //
	//=============================================================
	uint16_t numIFDEntries = 0;
	f->InStream->read((char*)&numIFDEntries, sizeof(numIFDEntries));
	
	printf("IDF #0. Found %d IDF Entries\n", numIFDEntries);
	CR2IDFFrame* idfFrame = new CR2IDFFrame();
	CR2IDFEntry* idfEntries = new CR2IDFEntry[numIFDEntries];
	for (int i = 0; i < numIFDEntries; ++i)
	{
		printf("Parsing IDF entry%d\n", i);
		
		_C2DReadIDFEntry(idfEntries + i, *f->InStream);
		_C2DDecodeIDFEntry(idfFrame, idfEntries + i, *f->InStream);
	}
	
	f->ImageData->Frames.emplace_back(idfFrame);
	delete[] idfEntries;

	uint32_t nextOffset = 0;																		// next IDF offset, keep parsing while it's different than 0
	f->InStream->read((char*)&nextOffset, sizeof(uint32_t));

	if (idfFrame->ExifSubDirOffset != 0)															// check if exif is present, if so, parse exif data
	{
		f->InStream->seekg(idfFrame->ExifSubDirOffset);
		uint16_t numExifEntries = 0;
		f->InStream->read((char*)&numExifEntries, sizeof(numExifEntries));
		printf("Exif data detected. Found %d exif IDF entries.\n", numExifEntries);

		idfEntries = new CR2IDFEntry[numExifEntries];
		for(int i = 0; i < numExifEntries; ++i)
		{
			_C2DReadIDFEntry(idfEntries + i, *f->InStream);
			_C2DDecodeExifEntry(&f->ImageData->ExifData, idfEntries + i, *f->InStream);
		}
		delete[] idfEntries;

		if (f->ImageData->ExifData.MakerNoteoffset != 0)											// check if makernote is present, if so, parse makernote data
		{
			f->InStream->seekg(f->ImageData->ExifData.MakerNoteoffset);
			uint16_t numMakerNoteEntries = 0;
			f->InStream->read((char*)&numMakerNoteEntries, sizeof(numMakerNoteEntries));
			printf("Makernote data detected. Found %d makernote IDF entries.\n", numMakerNoteEntries);
			
			idfEntries = new CR2IDFEntry[numMakerNoteEntries];
			for(int i = 0; i < numMakerNoteEntries; ++i)
			{
				_C2DReadIDFEntry(idfEntries + i, *f->InStream);
				_C2DDecodeMakerNoteEntry(&f->ImageData->MakerNotes, idfEntries + i, *f->InStream);
			}
			delete[] idfEntries;
		}
	}

	//=============================================================
	//							IDF #1							 //
	//=============================================================
	idfFrame = new CR2IDFFrame();
	f->InStream->seekg(nextOffset); 
	f->InStream->read((char*)&numIFDEntries, sizeof(uint16_t));
	idfEntries = new CR2IDFEntry[numIFDEntries];
	printf("IDF #1. Found %d IDF Entries\n", numIFDEntries);
	for (int i = 0; i < numIFDEntries; ++i)
	{
		_C2DReadIDFEntry(idfEntries + i, *f->InStream);
		_C2DDecodeIDFEntry(idfFrame, idfEntries + i, *f->InStream);
	}
	delete[] idfEntries;
	f->ImageData->Frames.emplace_back(idfFrame);
	f->InStream->read((char*)&nextOffset, sizeof(uint32_t));
	

	//=============================================================
	//							IDF #2							 //
	//=============================================================
	idfFrame = new CR2IDFFrame();
	f->InStream->seekg(nextOffset);
	f->InStream->read((char*)&numIFDEntries, sizeof(numIFDEntries));
	idfEntries = new CR2IDFEntry[numIFDEntries];
	printf("IDF #2. Found %d IDF Entries\n", numIFDEntries);
	for (int i = 0; i < numIFDEntries; ++i)
	{
		_C2DReadIDFEntry(idfEntries + i, *f->InStream);
		_C2DDecodeIDFEntry(idfFrame, idfEntries + i, *f->InStream);
	}
	delete[] idfEntries;
	f->ImageData->Frames.emplace_back(idfFrame);
	f->InStream->read((char*)&nextOffset, sizeof(uint32_t));

	//=============================================================
	//							IDF #3							 //
	//=============================================================
	idfFrame = new CR2IDFFrame();
	f->InStream->seekg(nextOffset);
	f->InStream->read((char*)&numIFDEntries, sizeof(numIFDEntries));
	idfEntries = new CR2IDFEntry[numIFDEntries];
	for (int i = 0; i < numIFDEntries; ++i)
	{
		_C2DReadIDFEntry(idfEntries + i, *f->InStream);
		_C2DDecodeIDFEntry(idfFrame, idfEntries + i, *f->InStream);
	}
	delete[] idfEntries;
	f->ImageData->Frames.emplace_back(idfFrame);
	f->InStream->read((char*)&nextOffset, sizeof(nextOffset));
	// TODO: handle nextoffset = 0

	return f;
}

void C2DFree(CR2File* file)
{
	file->InStream->close();
	delete file->InStream;
	delete file->ImageData;
	delete file;
	file = nullptr;
}
