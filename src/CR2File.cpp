#include "CR2FileImpl.h"
#include "Constants.h"
#include "CR2IDFFrame.h"
#include "CR2ImageData.h"
#include <stdexcept>

void CR2FileExtractPreviewImage(CR2File* file, const std::string& outputPath)
{
	if (file->ImageData->Frames.empty()){
		throw std::runtime_error(CR2ERR_FILE_EXTRACT_IDF0_IDF0_NOT_PRESENT);
	}
	const CR2IDFFrame* IDF0 = file->ImageData->Frames.at(0);
	std::ifstream* fStream = file->InStream;
	
	char* jpegBuffer = new char[IDF0->StripByteCounts];
	const std::streampos curPos = fStream->tellg();
	fStream->seekg(IDF0->StripOffsets[0]);
	fStream->read(jpegBuffer, IDF0->StripByteCounts);
	std::ofstream jpegPreview(outputPath, std::ios::out | std::ios::binary);
	jpegPreview.write(jpegBuffer, IDF0->StripByteCounts);
	jpegPreview.close();
}