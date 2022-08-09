#ifndef CR2_DECODER_H
#define CR2_DECODER_H

#include "CR2File.h"

CR2File* C2DLoad(const char* path);

void C2DFree(CR2File* file);

#endif // CR2_DECODER_H
