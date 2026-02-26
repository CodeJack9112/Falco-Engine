#include "API_CapsuleCollider.h"
#include "CapsuleCollider.h"

float API_CapsuleCollider::getRadius(MonoObject* this_ptr)
{
	CapsuleCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	return component->GetRadius();
}

void API_CapsuleCollider::setRadius(MonoObject* this_ptr, float val)
{
	CapsuleCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetRadius(val);
}

float API_CapsuleCollider::getHeight(MonoObject* this_ptr)
{
	CapsuleCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	return component->GetHeight();
}

void API_CapsuleCollider::setHeight(MonoObject* this_ptr, float val)
{
	CapsuleCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetHeight(val);
}
