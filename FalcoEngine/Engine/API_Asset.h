#pragma once

#include "API.h"

class API_Asset
{
public:
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Asset::INTERNAL_exists", (void*)exists);
	}

	static bool exists(MonoString* value);
};