#include "API_Asset.h"
#include "Engine.h"
#include "StringConverter.h"
#include "ZipHelper.h"
#include "IO.h"

bool API_Asset::exists(MonoString* value)
{
	std::string filename = (const char*)mono_string_to_utf8((MonoString*)value);
	filename = CP_SYS(filename);

	if (!GetEngine->GetUseUnpackedResources())
	{
		return ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), filename);
	}
	else
	{
		if (filename.empty())
			return false;

		return IO::FileExists(GetEngine->GetAssetsPath() + filename);
	}
}
