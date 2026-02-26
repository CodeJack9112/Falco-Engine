#include "API_Debug.h"
#include "stdafx.h"
#include <iostream>
#include "Engine.h"
#include "StringConverter.h"

void API_Debug::log(MonoString* stringVal)
{
	const char * _str = (const char*)mono_string_to_utf8((MonoString*)stringVal);
	if (_str != nullptr)
	{
		std::string str = CP_SYS(std::string(_str));

		std::cout << str << std::endl;
	}
}
