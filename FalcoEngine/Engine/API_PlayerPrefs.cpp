#include "API_PlayerPrefs.h"
#include "StringConverter.h"

#include "Engine.h"
#include "PlayerPrefs.h"

#include <string>

using namespace std;

bool API_PlayerPrefs::trySetInt(MonoString* key, int value)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);
	
	PlayerPrefs::Singleton.setInt(keyStr, value);

	return true;
}

bool API_PlayerPrefs::trySetFloat(MonoString* key, float value)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	PlayerPrefs::Singleton.setFloat(keyStr, value);

	return true;
}

bool API_PlayerPrefs::trySetString(MonoString* key, MonoString* value)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	string valStr = (const char*)mono_string_to_utf8(value);
	valStr = CP_SYS(valStr);

	PlayerPrefs::Singleton.setString(keyStr, valStr);

	return true;
}

int API_PlayerPrefs::getInt(MonoString* key, int defaultValue)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	return PlayerPrefs::Singleton.getInt(keyStr, defaultValue);
}

float API_PlayerPrefs::getFloat(MonoString* key, float defaultValue)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	return PlayerPrefs::Singleton.getFloat(keyStr, defaultValue);
}

MonoString* API_PlayerPrefs::getString(MonoString* key, MonoString* defaultValue)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	string valStr = (const char*)mono_string_to_utf8(defaultValue);
	valStr = CP_SYS(valStr);

	string str = PlayerPrefs::Singleton.getString(keyStr, valStr);
	return mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), str.c_str());
}

bool API_PlayerPrefs::hasKey(MonoString* key)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	return PlayerPrefs::Singleton.hasKey(keyStr);
}

void API_PlayerPrefs::deleteKey(MonoString* key)
{
	string keyStr = (const char*)mono_string_to_utf8(key);
	keyStr = CP_SYS(keyStr);

	PlayerPrefs::Singleton.deleteKey(keyStr);
}

void API_PlayerPrefs::deleteAll()
{
	PlayerPrefs::Singleton.deleteAll();
}

void API_PlayerPrefs::save()
{
	PlayerPrefs::Singleton.save(GetEngine->GetLibraryPath() + "PlayerPrefs.bin");
}
