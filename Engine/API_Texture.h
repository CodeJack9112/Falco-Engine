#pragma once

#include "API.h"

class API_Texture
{
public:
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Texture2D::INTERNAL_loadFromFile", (void*)loadFromFile);
		mono_add_internal_call("FalcoEngine.Texture2D::get_width", (void*)getWidth);
		mono_add_internal_call("FalcoEngine.Texture2D::get_height", (void*)getHeight);
	}

private:
	static MonoObject* loadFromFile(MonoObject * path);
	static int getWidth(MonoObject * this_ptr);
	static int getHeight(MonoObject * this_ptr);
};

