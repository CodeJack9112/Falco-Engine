#pragma once

#include <LibZip\zip.h>
#include "../Engine/StringConverter.h"
#include <string>
#include <vector>

using namespace std;

class ZipHelper
{
public:
	static bool isFileInZip(zip_t* zip, std::string path);

	static char* readFileFromZip(zip_t* zip, string path, int& outBufSize);

	static std::vector<string> getAllFilesNamesInZip(zip_t* zip);
};

