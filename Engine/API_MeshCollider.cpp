#include "API_MeshCollider.h"
#include "Engine.h"
#include "MeshCollider.h"

bool API_MeshCollider::getConvex(MonoObject* this_ptr)
{
	MeshCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	return component->GetConvex();
}

void API_MeshCollider::setConvex(MonoObject* this_ptr, bool value)
{
	MeshCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetConvex(value);
}
