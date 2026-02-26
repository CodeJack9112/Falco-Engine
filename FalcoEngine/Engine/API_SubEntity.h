#pragma once

#include "API.h"

class API_SubEntity
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.SubEntity::get_material", (void*)getMaterial);
		mono_add_internal_call("FalcoEngine.SubEntity::set_material", (void*)setMaterial);
		mono_add_internal_call("FalcoEngine.SubEntity::get_sharedMaterial", (void*)getSharedMaterial);
		mono_add_internal_call("FalcoEngine.SubEntity::set_sharedMaterial", (void*)setSharedMaterial);
	}

private:
	//getMaterial
	static MonoObject * getMaterial(MonoObject* this_ptr);

	//setMaterial
	static void setMaterial(MonoObject* this_ptr, MonoObject * material);

	//getSharedMaterial
	static MonoObject* getSharedMaterial(MonoObject* this_ptr);

	//setSharedMaterial
	static void setSharedMaterial(MonoObject* this_ptr, MonoObject* material);
};

