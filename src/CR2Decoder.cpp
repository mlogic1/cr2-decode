#include "CR2ImageData.h"
#include "Constants.h"
#include <CR2Decoder.h>
#include <CR2File.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <stdio.h>

/* private API */
static void _C2DDecodeHeader(CR2Header* header, std::ifstream& fStream)
{
	size_t HDDR_SIZE = sizeof(CR2Header);
	fStream.read((char*)&header->ByteOrdering, 2);
	fStream.read((char*)&header->TiffMagicWord, 2);
	fStream.read((char*)&header->TiffOffset, 4);
	fStream.read((char*)&header->CR2MagicWord, 2);	// this one is copied in the reverse order, its 0x5243 - check is it the same on windows and fix
	fStream.read((char*)&header->CR2VerMajor, 1);
	fStream.read((char*)&header->CR2VerMinor, 1);
	fStream.read((char*)&header->RawIFDOffset, 4);
}

CR2File* C2DLoad(const char* path)
{
	std::ifstream inStream(path, std::ios::in | std::ios::binary);
	if (inStream.fail()){
		return nullptr;
	}

	CR2ImageData* imData = new CR2ImageData();
	_C2DDecodeHeader(&imData->Header, inStream);
	// read the rest of the content

	CR2File* f = new CR2File();
	return f;
}

void C2DFree(CR2File* file)
{
	delete file;
	file = nullptr;
}
