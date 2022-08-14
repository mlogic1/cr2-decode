#include "CR2ImageData.h"
#include "CR2IDFEntry.h"
#include "Constants.h"
#include <CR2Decoder.h>
#include <CR2File.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdexcept>

/*	Error list */
constexpr const char* CR2ERR_UNIMPLEMENTED					= "Unimplemented function or logic";
constexpr const char* CR2ERR_FILE_NOT_FOUND 				= "File not found";
constexpr const char* CR2ERR_HEADER_FAULTY_BYTE_CODE		= "Unknown byte code. File might not be CR2 format";
constexpr const char* CR2ERR_HEADER_FAULTY_CR2_MAGIC_WORD	= "Unknown CR2 magic word. File might not be CR2 format";


static void _CR2TestIDF0(CR2IDFEntry* idfEntry, std::ifstream& inStream)
{
	// temp vars
	static uint16_t imWidth = 0;
	static uint16_t imLen = 0;
		switch (idfEntry->TagID)
		{
		case 256:
			imWidth = idfEntry->Value;
			break;

		case 257:
			imLen = idfEntry->Value;
			break;

		case 258:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			uint16_t bps[3] = { 0, 0, 0 };
			inStream.read((char*)bps, idfEntry->Values * sizeof(short));
			// TODO: do something with bits per sample
			inStream.seekg(curPos);
			break;
		}

		case 271:
		{
			std::streampos curPos = inStream.tellg();
			inStream.seekg(idfEntry->Value);
			char* buff = new char[idfEntry->Values];
			inStream.read(buff, idfEntry->Values);
			// TODO: do something with string
			delete[] buff;
			break;
		}
			
			
		default:
			int x = 1;
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

	if (header->CR2MagicWord != CR2_MAGIC_WORD)
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

	for (int i = 0; i < numIFDEntries; ++i)
	{
		printf("Parsing IDF%d\n", i);
		CR2IDFEntry* idfEntry = new CR2IDFEntry();		// TODO: this is not cleaned up
		_C2DDecodeIDF(idfEntry, inStream);
		_CR2TestIDF0(idfEntry, inStream);
		int x = 1;
	}

	CR2File* f = new CR2File();
	return f;
}

void C2DFree(CR2File* file)
{
	delete file;
	file = nullptr;
}
