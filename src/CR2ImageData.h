#ifndef CR2_IMAGE_DATA_H
#define CR2_IMAGE_DATA_H

#include "CR2Header.h"
#include "CR2IDFFrame.h"
#include <vector>

struct CR2ImageData
{
	CR2Header Header;
	std::vector<CR2IDFFrame*> Frames;
};

#endif // CR2_IMAGE_DATA_H
