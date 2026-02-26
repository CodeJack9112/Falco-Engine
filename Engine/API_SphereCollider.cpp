#include "API_SphereCollider.h"
#include "SphereCollider.h"

float API_SphereCollider::getRadius(MonoObject* this_ptr)
{
	SphereCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	return component->GetRadius();
}

void API_SphereCollider::setRadius(MonoObject* this_ptr, float val)
{
	SphereCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetRadius(val);
}
