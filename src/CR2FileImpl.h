#ifndef CR2_FILE_IMPL_H
#define CR2_FILE_IMPL_H

#include <CR2File.h>
#include "CR2ImageData.h"
#include <fstream>

struct CR2File
{
	CR2ImageData* ImageData = nullptr;
	std::ifstream* InStream = nullptr;
};


#endif // CR2_FILE_IMPL_H
