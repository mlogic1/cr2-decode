#ifndef CR2_IDF_FRAME_H
#define CR2_IDF_FRAME_H

#include "CR2IDFEntry.h"
#include <string>

struct CR2IDFFrame
{
	uint16_t ImageWidth = 0;
	uint16_t ImageHeight = 0;
	uint16_t* BitsPerSample = nullptr;
	uint16_t BitsPerSampleLen = 0;
	uint16_t Compression = 0;
	uint32_t* StripOffsets = nullptr;
	uint16_t StripOffsetsLen = 0;
	std::string Make;
	std::string Model;
	uint16_t Orientation = 0;
	uint32_t StripByteCounts = 0;
	unsigned long XResolution = 0;
	unsigned long YResolution = 0;
	uint16_t ResolutionUnit = 0;
	std::string dateTime;

	~CR2IDFFrame()
	{
		delete[] BitsPerSample;
		delete[] StripOffsets;
	}
};

#endif // CR2_IDF_FRAME_H