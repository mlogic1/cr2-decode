#include "CR2ImageData.h"
#include "CR2IDFEntry.h"
#include "CR2IDFFrame.h"
#include "Constants.h"
#include <CR2Decoder.h>
#include <CR2File.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdexcept>

static void _CR2TestIDF0(CR2IDFFrame* frame, CR2IDFEntry* idfEntry, std::ifstream& inStream)
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
			inStream.read((char*)frame->StripOffsets, frame->StripOffsetsLen * sizeof(uint32_t));		// TODO: Unsure if this is correct - needs checking
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

		case CR2_IDF_TAG_ARTIST:
		case CR2_IDF_TAG_HOST_COMPUTER:
		case CR2_IDF_TAG_XMP:
		case CR2_IDF_TAG_COPYRIGHT:
		break;

		case CR2_IDF_TAG_EXIF:
		{
			int breakpoint = 1;
			break;
		}
			
		default:
			const std::string error = std::string(CR2ERR_IDF_UNKNOWN_TAG_ID) + ": " + std::to_string(idfEntry->TagID);
			throw std::runtime_error(error);
			break;
	}
}

/* private API */
static void _C2DDecodeHeader(CR2Header* header, std::ifstream& fStream)
{
	// size_t HDDR_SIZE = sizeof(CR2Header);
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

static void _C2DDecodeIDF(CR2IDFEntry* entry, std::ifstream& fStream)
{
	if (sizeof(CR2IDFEntry) == 12)
	{
		fStream.read( (char*)entry, 12);
	}
	else
	{
		// manually read the 12 bytes in case there was memory padding added, this is just a fallback in case the struct is not exacly the size of idf
		throw std::runtime_error(CR2ERR_UNIMPLEMENTED);
	}
}

CR2File* C2DLoad(const char* path)
{
	std::ifstream inStream(path, std::ios::in | std::ios::binary);
	if (inStream.fail()){
		throw std::runtime_error(CR2ERR_FILE_NOT_FOUND);
	}

	CR2ImageData* imData = new CR2ImageData();			// TODO: this is not cleaned up
	_C2DDecodeHeader(&imData->Header, inStream);

	uint16_t numIFDEntries;
	inStream.read((char*)&numIFDEntries, 2);
	// read the rest of the content

	printf("Found %d IDF Entries\n", numIFDEntries);
	CR2IDFFrame idfFrame;
	CR2IDFEntry* idfEntries = new CR2IDFEntry[numIFDEntries];
	for (int i = 0; i < numIFDEntries; ++i)
	{
		printf("Parsing IDF%d\n", i);
		
		_C2DDecodeIDF(idfEntries + i, inStream);
		_CR2TestIDF0(&idfFrame, idfEntries + i, inStream);	
	}
	
	imData->Frames.emplace_back(idfFrame);
	delete[] idfEntries;

	// TODO: read next IFD offset and parse other IFD's
	CR2File* f = new CR2File();
	return f;
}

void C2DFree(CR2File* file)
{
	delete file;
	file = nullptr;
}
