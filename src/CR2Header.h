#ifndef CR2_HEADER_H
#define CR2_HEADER_H

struct CR2Header
{
	unsigned char ByteOrdering[2];
	short TiffMagicWord;
	long TiffOffset;
	short CR2MagicWord;
	char CR2VerMajor;
	char CR2VerMinor;
	long RawIFDOffset;
};

#endif // CR2_HEADER_H
