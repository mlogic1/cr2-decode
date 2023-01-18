#include <iostream>
#include <CR2Decoder.h>
#include <CR2File.h>
#include <map>
#include <string>
#include <vector>
#include <set>

constexpr const char* SAMPLE_IMAGE_FILE = "../sample/canon-sample.CR2";
// constexpr const char* SAMPLE_IMAGE_FILE = "../../sample/IMG_2026.CR2";

// List of possible options
enum class ArgumentOption
{
	ShowHelp = 0,
	ExtractJpeg,
	DisplayInfo
};

const std::map<std::string, ArgumentOption> StrArgToOption =
{
	{ "-h"		, ArgumentOption::ShowHelp 	},
	{ "-H"		, ArgumentOption::ShowHelp 	},
	{ "--help"	, ArgumentOption::ShowHelp 	},
	{ "-j"		, ArgumentOption::ExtractJpeg 	},
	{ "--jpeg"	, ArgumentOption::ExtractJpeg 	},
	{ "-J"		, ArgumentOption::ExtractJpeg 	},
	{ "-i"		, ArgumentOption::DisplayInfo 	},
	{ "-I"		, ArgumentOption::DisplayInfo 	},
	{ "--info"	, ArgumentOption::DisplayInfo 	}
};

struct Arguments
{
	std::set<ArgumentOption> Options;
	std::string InputImagePath;
};

Arguments ParseArguments(int argc, char* argv[])
{
	Arguments result;

	if (argc < 2){
		result.Options.insert(ArgumentOption::ShowHelp);
		return result;
	}

	for (int i = 1; i < argc; ++i)
	{
		std::string strOption(argv[i]);

		if (StrArgToOption.find(strOption) != StrArgToOption.end())
		{
			ArgumentOption argOption = StrArgToOption.at(strOption);		// argument is an option
			result.Options.insert(argOption);
		}else{
			result.InputImagePath = strOption;					// argument is a file name
		}
	}

	return result;
}

void PrintHelp()
{
	std::cout << "cr2-decode - RAW file decoder" << std::endl << "\t     ver. " << C2DVersion() << std::endl;
	std::cout << "Usage: cr2-decode [options] input-image.cr2\n	\
	\
	OPTIONS \n\
	\t	-h / --help\t|\tShow this help				\n \
	\t	-j / --jpeg\t|\tExtract full size jpg from raw image	\n \
	\t	-i / --info\t|\tPrint image information			\n";
}

int main(int argc, char* argv[])
{
	Arguments arguments = ParseArguments(argc, argv);

	if (arguments.Options.find(ArgumentOption::ShowHelp) != arguments.Options.end())
	{
		PrintHelp();
		return 0;
	}

	CR2File* f = nullptr;
	try
	{
		f = C2DLoad(SAMPLE_IMAGE_FILE);
	}catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}

	if (!f){
		std::cout << "Unable to decode file" << std::endl;
		return -1;
	}

	CR2FileExtractLosslessJpeg(f, "/home/filip/Desktop/test/loss.less");

	if (arguments.Options.find(ArgumentOption::ExtractJpeg) != arguments.Options.end())
	{
		CR2FileExtractPreviewImage(f, arguments.InputImagePath + std::string(".jpg"));
	}


	C2DFree(f);
	return 0;
}
