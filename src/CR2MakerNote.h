#ifndef CR2_MAKERNOTE_H
#define CR2_MAKERNOTE_H

#include <stdint.h>

struct CR2MakerNote
{
	uint16_t* CameraSettings = nullptr;
	uint16_t CameraSettingsLen = 0;

	uint16_t* FocalLength = nullptr;
	uint16_t FocalLengthLen = 0;

	std::string ImageType = "";

	~CR2MakerNote()
	{
		if (CameraSettings)
			delete[] CameraSettings;
			
		if (FocalLength)
			delete[] FocalLength;
	}
};

#endif // CR2_MAKERNOTE_H