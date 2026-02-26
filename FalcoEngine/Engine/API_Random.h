#pragma once

#include "API.h"

class API_Random
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Random::Range", (void*)range);
	}

private:
	//Get position
	static float range(float _min, float _max);
};