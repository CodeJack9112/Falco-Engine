#include "stdafx.h"
#include "API_SceneManager.h"
#include "Engine.h"
#include "SceneSerializer.h"
#include "IO.h"
#include "PhysicsManager.h"
#include "StringConverter.h"

MonoString * API_SceneManager::getLoadedScene()
{
	std::string loadedScene = IO::RemovePart(GetEngine->loadedScene, GetEngine->GetAssetsPath());

	MonoString * path = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(loadedScene).c_str());

	return path;
}

void API_SceneManager::loadScene(MonoObject * path)
{
	std::string _path = (const char*)mono_string_to_utf8((MonoString*)path);
	_path = CP_SYS(_path);

	GetEngine->GetMonoRuntime()->LoadScene(GetEngine->GetAssetsPath() + _path);
}