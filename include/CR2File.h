#ifndef CR2_FILE_H
#define CR2_FILE_H

#include <string>

typedef struct CR2File CR2File;

void CR2FileExtractPreviewImage(CR2File* file, const std::string& outputPath);
void CR2FileExtractLosslessJpeg(CR2File* file, const char* outputPath);

#endif // CR2_FILE_H
