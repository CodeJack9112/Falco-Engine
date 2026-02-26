#pragma once

#include "OgrePrerequisites.h"
#include <string>
#include <vector>

using namespace std;

struct TSection
{
public:
	string key;
	string value;
};

struct TTextData
{
public:
	string resourceType;
	string resourceName;

	std::vector<TSection> sections;
	TSection GetByKey(string key);
};

class _OgreExport TextParser
{
private:
	enum CurrentPos { CP_RESOURCE_TYPE, CP_RESOURCE_NAME, CP_SECTIONS, CP_SECTION_KEY, CP_SECTION_VALUE, CP_END };
	CurrentPos currentPos = CurrentPos::CP_RESOURCE_TYPE;

public:
	TextParser();
	~TextParser();

	TTextData ParseText(string textData);
};