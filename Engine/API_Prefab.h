#pragma once

#include "API.h"

class API_Prefab
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Prefab::INTERNAL_instantiate", (void*)instantiate);
	}

	//Add force
	static MonoObject * instantiate(MonoObject * prefabFileName, API::Vector3 * position, API::Quaternion * rotation);
};