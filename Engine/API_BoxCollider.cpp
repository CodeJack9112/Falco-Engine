#include "API_BoxCollider.h"
#include "BoxCollider.h"

void API_BoxCollider::getSize(MonoObject* this_ptr, API::Vector3* out_val)
{
	BoxCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	Vector3 size = component->GetBoxSize();

	out_val->x = size.x;
	out_val->y = size.y;
	out_val->z = size.z;
}

void API_BoxCollider::setSize(MonoObject* this_ptr, API::Vector3* ref_val)
{
	BoxCollider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetBoxSize(Vector3(ref_val->x, ref_val->y, ref_val->z));
}
