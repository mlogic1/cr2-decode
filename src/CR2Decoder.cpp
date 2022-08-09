#include "CR2Header.h"
#include "Constants.h"
#include <CR2Decoder.h>
#include <CR2File.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <stdio.h>

/* private API */
static CR2Header _C2DDecodeHeader(char* headerBin)
{
	CR2Header header;
	memcpy(header.ByteOrdering, headerBin, 2);
	header.TiffMagicWord = (short)headerBin[2];	// this probably needs memcpy
	header.TiffOffset = (long)headerBin[4];		// this probably needs memcpy
	memcpy(&header.CR2MagicWord, headerBin + 8, 2);	// this one is copied in the reverse order, its 0x5243 - check is it the same on windows and fix
	header.CR2VerMajor = headerBin[10];
	header.CR2VerMinor = headerBin[11];
	memcpy(&header.RawIFDOffset, headerBin + 12, sizeof(long));

	printf("0x%04x\n", header.CR2MagicWord);

	std::cout << "byte 8 = " << headerBin[8] << std::endl;
	std::cout << "byte 9 = " << headerBin[9] << std::endl;
	printf("%d\n", header.RawIFDOffset);

	return header;
}

CR2File* C2DLoad(const char* path)
{
	std::ifstream inStream(path, std::ios::in | std::ios::binary);
	if (inStream.fail()){
		return nullptr;
	}

	char* headerBin = new char[HEADER_SIZE];
	inStream.read(headerBin, HEADER_SIZE);
	CR2Header header = _C2DDecodeHeader(headerBin);


	// read the rest of the content
	// inStream.read(headerBin, HEADER_SIZE);

	delete headerBin;

	CR2File* f = new CR2File();


	return f;
}

void C2DFree(CR2File* file)
{
	delete file;
	file = nullptr;
}
