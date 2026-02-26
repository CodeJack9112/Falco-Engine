#pragma once

#include "API.h"

class API_Entity
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Entity::get_subEntitiesCount", (void*)getSubEntitiesCount);
		mono_add_internal_call("FalcoEngine.Entity::INTERNAL_getSubEntity", (void*)getSubEntity);
	}

private:
	//getSubEntitiesCount
	static int getSubEntitiesCount(MonoObject* this_ptr);

	//getSubEntity
	static MonoObject * getSubEntity(MonoObject* this_ptr, int index);
};