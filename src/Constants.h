#ifndef CONSTANTS_H
#define CONSTANTS_H

// Header related constants
#define CR2_HEADER_BYTE_ORDERING_LITTLE_ENDIAN	0x4949
#define CR2_HEADER_BYTE_ORDERING_BIG_ENDIAN		0x4D4D
#define CR2_HEADER_TIFF_OFFSET					0x0010
#define CR2_HEADER_TIFF_MAGIC_WORD				0x002A
#define CR2_HEADER_MAGIC_WORD					0x4352

// IFD constants
#define CR2_IFD_TAG_IMAGE_WIDTH					0x100
#define CR2_IFD_TAG_IMAGE_HEIGHT				0x101
#define CR2_IFD_TAG_BITS_PER_SAMPLE				0x102
#define CR2_IFD_TAG_COMPRESSION					0x103
#define CR2_IFD_TAG_MAKE						0x10F
#define CR2_IFD_TAG_MODEL						0x110
#define CR2_IFD_TAG_STRIP_OFFSET				0x111
#define CR2_IFD_TAG_ORIENTATION					0x112
#define CR2_IFD_TAG_STRIP_BYTE_COUNTS			0x117
#define CR2_IFD_TAG_X_RESOLUTION				0x11A
#define CR2_IFD_TAG_Y_RESOLUTION				0x11B
#define CR2_IFD_TAG_RESOLUTION_UNIT				0x128
#define CR2_IFD_TAG_DATE_TIME					0x132
#define CR2_IFD_TAG_EXIF						0x8769
#define CR2_IFD_TAG_GPS_DATA					0x8825
#define CR2_IFD_TAG_THUMB_OFFSET				0x0201
#define CR2_IFD_TAG_THUMB_LENGTH				0x0202
#define CR2_IFD_TAG_PHOTOMETRIC_INTERPRETATION	0x0106
#define CR2_IFD_TAG_SAMPLES_PER_PIXEL			0x0115
#define CR2_IFD_TAG_ROWS_PER_STRIP				0x0116
#define CR2_IFD_TAG_PLANAR_CONFIGURATION		0x011C
#define CR2_IFD_TAG_RAW_IMAGE_SEGMENTATION		0xC640

// EXIF constants
#define CR2_EXIF_TAG_EXPOSURE_TIME				0x829A
#define CR2_EXIF_TAG_f_NUMBER					0x829D
#define CR2_EXIF_TAG_MAKERNOTE_OFFSET			0x927C


// Makernote constants
#define CR2_MAKERNOTE_TAG_CAMERA_SETTINGS		0x0001
#define CR2_MAKERNOTE_TAG_FOCAL_LENGTH			0x0002
#define CR2_MAKERNOTE_TAG_IMAGE_TYPE			0x0006
#define CR2_MAKERNOTE_TAG_FIRMWARE_VERSION		0x0007
#define CR2_MAKERNOTE_TAG_SERIAL_NUMBER			0x000C
#define CR2_MAKERNOTE_TAG_FILE_LENGTH			0x000E
#define CR2_MAKERNOTE_TAG_MODEL_ID				0x0010
#define CR2_MAKERNOTE_TAG_COLOR_SPACE			0x00B4

// currently ignored
#define CR2_IDF_TAG_ARTIST						0x13B
#define CR2_IDF_TAG_HOST_COMPUTER				0x13C
#define CR2_IDF_TAG_XMP							0x2BC
#define CR2_IDF_TAG_COPYRIGHT					0x8298

// Misc constants
#define CR2_IDF_TAG_DATE_TIME_SIZE 20


/*	Error list */
constexpr const char* CR2ERR_UNIMPLEMENTED							= "Unimplemented function or logic";
constexpr const char* CR2ERR_FILE_NOT_FOUND 						= "File not found";
constexpr const char* CR2ERR_HEADER_FAULTY_BYTE_CODE				= "Unknown byte code. File might not be CR2 format";
constexpr const char* CR2ERR_HEADER_FAULTY_CR2_MAGIC_WORD			= "Unknown CR2 magic word. File might not be CR2 format";
constexpr const char* CR2ERR_IDF_UNKNOWN_TAG_ID						= "Unknown IDF Tag ID";
constexpr const char* CR2ERR_IDF_BPS_ALREADY_ALLOCATED				= "Bits per sample is already allocated. This should not occur more than once per frame";
constexpr const char* CR2ERR_IDF_STRIP_OFFSETS_ALREADY_ALLOCATED	= "Strip offsets already allocated. This should not oocur more than once per frame";
constexpr const char* CR2ERR_IDF_EXIF_OFFSET_MISSING				= "EXIF offset tag not found in IDF frame";
constexpr const char* CR2ERR_FILE_EXTRACT_IDF0_IDF0_NOT_PRESENT		= "There are no frames present in this file";


#endif // CONSTANTS_H