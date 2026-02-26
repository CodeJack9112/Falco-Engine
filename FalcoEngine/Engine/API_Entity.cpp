#include "API_Entity.h"
#include "Engine.h"
#include <OgreEntity.h>
#include <OgreSubEntity.h>

int API_Entity::getSubEntitiesCount(MonoObject* this_ptr)
{
	Entity* ent;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&ent));

	return ent->getNumSubEntities();
}

MonoObject* API_Entity::getSubEntity(MonoObject* this_ptr, int index)
{
	Entity* ent;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->attached_object_ptr_field, reinterpret_cast<void*>(&ent));

	MonoObject * sub = nullptr;

	if (index < ent->getNumSubEntities())
		sub = ent->getSubEntity(index)->objectMono;

	return sub;
}
