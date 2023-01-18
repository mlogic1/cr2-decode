#include "CR2FileImpl.h"
#include "Constants.h"
#include "CR2IDFFrame.h"
#include "CR2ImageData.h"
#include "huffman/huffman.h"
#include <stdexcept>
#include <stdlib.h>
#include <iostream>

// https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/

static uint16_t SwapBytesUint16(uint16_t input)
{
#ifdef _WIN32
	return _byteswap_ushort(input);
#else
	return __builtin_bswap16(input);
#endif
}

void CR2FileExtractPreviewImage(CR2File* file, const std::string& outputPath)
{
	if (file->ImageData->Frames.empty()){
		throw std::runtime_error(CR2ERR_FILE_EXTRACT_IDF0_IDF0_NOT_PRESENT);
	}
	const CR2IDFFrame* IDF0 = file->ImageData->Frames.at(0);
	std::ifstream* fStream = file->InStream;

	char* jpegBuffer = new char[IDF0->StripByteCounts];
	const std::streampos curPos = fStream->tellg();
	fStream->seekg(IDF0->StripOffsets[0]);
	fStream->read(jpegBuffer, IDF0->StripByteCounts);
	std::ofstream jpegPreview(outputPath, std::ios::out | std::ios::binary);
	jpegPreview.write(jpegBuffer, IDF0->StripByteCounts);
	jpegPreview.close();
	fStream->seekg(curPos);
}

void CR2FileExtractLosslessJpeg(CR2File* file, const char* outputPath)
{
	std::ifstream* fStream = file->InStream;
	const std::streampos curPos = fStream->tellg();

	const CR2IDFFrame* frame = file->ImageData->Frames.at(3);
	std::cout <<"num frames: " <<file->ImageData->Frames.size()<<std::endl;

	fStream->seekg(frame->StripOffsets[0]);
	uint16_t soi = 0;
	fStream->read((char*)&soi, 2);

	soi = SwapBytesUint16(soi);

	if (soi != CR2_IMAGE3_SOI)
		throw std::runtime_error(CR2ERR_IMAGE3_SOI_VALIDATE);

	std::cout <<"soi: " << soi <<std::endl;

	uint16_t dhtMarker = 0;
	fStream->read((char*)&dhtMarker, 2);
	dhtMarker = SwapBytesUint16(dhtMarker);

	if (dhtMarker != CR2_IMAGE3_DHT_MARKER)
		throw std::runtime_error(CR2ERR_IMAGE3_DHT_MARKER_VALIDATE);

	std::cout << "dht marker: " << dhtMarker << std::endl;

	uint16_t Lh = 0; // segment length
	fStream->read((char*)&Lh, 2);
	Lh = SwapBytesUint16(Lh);
	std::cout << "Segment Length(Lh): " << Lh << std::endl;

	char table = 0;	// table
	fStream->read(&table, 1);

	// static constexpr const char TC_HIGH = 0b11110000; // table class mask
	static constexpr const char TC_LOW = 0b00001111; // table destination identifier

	char Tc = (table >> 4);
	char Th = ((table >> 4) & TC_LOW);
	std::cout << "Table class: " << (int)Tc << std::endl;
	std::cout << "Table destination: " << (int)Th << std::endl;

	// WIP - testing -----------
	char huffCodes[16];
	
	int freq[16];
	fStream->read(huffCodes, 16);

	// continue here	
	
	HuffmanCodes(huffCodes, freq, 16);
	// --------------------------

	/*
	std::cout << "Begin huff codes " << std::endl;
	for (int i = 0; i < 16; ++i)
		std::cout << (int)freq[i] << std::endl;
	std::cout << "End huff codes " << std::endl;*/


	fStream->seekg(curPos);
}
