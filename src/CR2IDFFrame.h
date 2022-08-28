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

	uint32_t ExifSubDirOffset = 0;

	uint32_t ThumbnailOffset = 0;
	uint32_t ThumbnailLength = 0;

	uint16_t PhotoMetricInterpretation = 0;
	uint16_t SamplesPerPixel = 0;
	uint16_t RowsPerStrip = 0;
	uint16_t PlanarConfiguration = 0;

	~CR2IDFFrame()
	{
		delete[] BitsPerSample;
		delete[] StripOffsets;
	}
};

#endif // CR2_IDF_FRAME_H