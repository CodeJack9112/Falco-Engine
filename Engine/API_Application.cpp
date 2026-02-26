#include "API_Application.h"
#include "Engine.h"
#include "StringConverter.h"

void API_Application::quit()
{
	GetEngine->shouldExit = true;
}

MonoString* API_Application::getAssetsPath()
{
	std::string _str = CP_UNI(GetEngine->GetAssetsPath());
	MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), _str.c_str());

	return str;
}
