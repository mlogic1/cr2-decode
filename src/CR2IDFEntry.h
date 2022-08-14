#ifndef CR2_IDF_ENTRY_H
#define CR2_IDF_ENTRY_H

#include <stdint.h>

struct CR2IDFEntry
{
	short TagID;
    short TagType;
    uint32_t Values;
    uint32_t Value;
};

#endif // CR2_IDF_ENTRY_H
