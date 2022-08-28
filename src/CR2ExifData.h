#ifndef CR2_EXIF_DATA_H
#define CR2_EXIF_DATA_H

#include <stdint.h>

struct CR2ExifData
{
	uint64_t ExposureTime = 0;
	uint64_t fNumber = 0;
	uint32_t MakerNoteoffset = 0;
};

/* 
================================================================================================
Exposuretime is a rational number, consists of 2 32 bit integers, here is how to read them
	uint32_t num, denom;
	num = static_cast<uint32_t>(makerNote->ExposureTime);
	denom = (makerNote->ExposureTime >> 32);

This should result in a value similar to: 1 / 4000
================================================================================================
Same applies to fNumber
================================================================================================
*/

#endif // CR2_EXIF_DATA_H
