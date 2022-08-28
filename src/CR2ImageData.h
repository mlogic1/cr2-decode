#ifndef CR2_IMAGE_DATA_H
#define CR2_IMAGE_DATA_H

#include "CR2Header.h"
#include "CR2IDFFrame.h"
#include "CR2ExifData.h"
#include "CR2MakerNote.h"
#include <vector>

struct CR2ImageData
{
	CR2Header Header;
	std::vector<CR2IDFFrame*> Frames;

	CR2ExifData ExifData;
	CR2MakerNote MakerNotes;

	~CR2ImageData()
	{
		for (CR2IDFFrame* frame : Frames)
			delete frame;
		Frames.clear();
	}
};

#endif // CR2_IMAGE_DATA_H
