#include "TextParser.h"

TextParser::TextParser()
{
}


TextParser::~TextParser()
{
}

#pragma optimize("", off)
TTextData TextParser::ParseText(string textData)
{
	if (textData.empty())
		return TTextData();

	TTextData data;

	currentPos = CurrentPos::CP_RESOURCE_TYPE;
	bool endParse = false;

	char c = textData[0];
	string word;

	TSection section;

	for (int i = 0; i < textData.size(); ++i)
	{
		c = textData[i];

		if (c != ' ' && c != '\n' && c != '\r\n' && c != '\r' && c != '\t' && c != '\0')
		{
			word += c;
		}
		else
		{
			if (currentPos != CurrentPos::CP_RESOURCE_TYPE && currentPos != CurrentPos::CP_SECTION_KEY)
			{
				if (c == ' ')
				{
					word += c;

					continue;
				}
			}

			switch (currentPos)
			{
			case CurrentPos::CP_RESOURCE_TYPE:
				data.resourceType = word;
				currentPos = CurrentPos::CP_RESOURCE_NAME;
				break;

			case CurrentPos::CP_RESOURCE_NAME:
				data.resourceName = word;
				currentPos = CurrentPos::CP_SECTIONS;
				break;

			case CurrentPos::CP_SECTIONS:
				if (word == "{")
				{
					currentPos = CurrentPos::CP_SECTION_KEY;
				}
				break;

			case CurrentPos::CP_SECTION_KEY:
				if (word == "}")
					currentPos = CurrentPos::CP_END;
				else
				{
					section.key = word;
					if (c == ' ')
						currentPos = CurrentPos::CP_SECTION_VALUE;
				}
				break;

			case CurrentPos::CP_SECTION_VALUE:
				section.value = word;
				data.sections.push_back(section);
				section = TSection();
				if (c == '\n' || c == '\r\n' || c == '\r')
					currentPos = CurrentPos::CP_SECTION_KEY;
				break;

			case CurrentPos::CP_END:
				endParse = true;
				break;
			}

			word = "";
		}

		if (endParse)
			break;
	}

	return data;
}
#pragma optimize("", on)

TSection TTextData::GetByKey(string key)
{
	std::vector<TSection>::iterator it;

	for (it = sections.begin(); it < sections.end(); ++it)
	{
		if (it->key == key)
			return *it;
	}

	return TSection();
}
