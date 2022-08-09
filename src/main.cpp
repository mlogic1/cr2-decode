#include <iostream>
#include <CR2Decoder.h>

constexpr const char* SAMPLE_IMAGE_FILE = "../sample/canon-sample.CR2";

int main(int argc, char* argv[])
{
	std::cout << "hello main" << std::endl;
	CR2File* f = C2DLoad(SAMPLE_IMAGE_FILE);

	if (!f){
		std::cout << "Unable to decode file" << std::endl;
		return -1;
	}

	C2DFree(f);
	return 0;
}
