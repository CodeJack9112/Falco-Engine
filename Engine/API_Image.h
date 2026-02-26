#pragma once

#include "API.h"

class API_Image
{
public:
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Image::get_image", (void*)getImage);
		mono_add_internal_call("FalcoEngine.Image::set_image", (void*)setImage);
	}

private:
	static MonoObject* getImage(MonoObject* this_ptr);
	static void setImage(MonoObject* this_ptr, MonoObject* texture);
};

