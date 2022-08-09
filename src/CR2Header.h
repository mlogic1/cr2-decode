#ifndef CR2_HEADER_H
#define CR2_HEADER_H

#define HEADER_SIZE 0x13

struct CanonHeader
{
	unsigned char ByteOrdering[2];
	short TiffMagicWord;
	long TiffOffset;
	char CR2MagicWord;
	char CR2VerMajor;
	char CR2VerMinor;
	long RawIFDOffset;
};

#endif // CR2_HEADER_H