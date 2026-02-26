#pragma once

#include "API.h"

class API_Shader
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Shader::.ctor", (void*)ctor);
		mono_add_internal_call("FalcoEngine.Shader::get_name", (void*)getName);
		mono_add_internal_call("FalcoEngine.Shader::INTERNAL_find", (void*)find);
		mono_add_internal_call("FalcoEngine.Shader::INTERNAL_getType", (void*)getType);
	}

private:
	//Constructor
	static void ctor(MonoObject* this_ptr);

	//Get name
	static MonoString* getName(MonoObject* this_ptr);

	//Find
	static MonoObject* find(MonoString* name);

	//Find
	static int getType(MonoObject* this_ptr);
};

