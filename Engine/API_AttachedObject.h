#pragma once

#include "API.h"

class API_AttachedObject
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.AttachedObject::get_gameObject", (void*)getGameObject);
	}

private:
	//Constructor
	static MonoObject* getGameObject(MonoObject* this_ptr);
};