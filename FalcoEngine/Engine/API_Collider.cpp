#include "API_Collider.h"
#include "Collider.h"

void API_Collider::getOffset(MonoObject* this_ptr, API::Vector3* out_val)
{
	Collider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	Vector3 offset = component->GetOffset();

	out_val->x = offset.x;
	out_val->y = offset.y;
	out_val->z = offset.z;
}

void API_Collider::setOffset(MonoObject* this_ptr, API::Vector3* ref_val)
{
	Collider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetOffset(Vector3(ref_val->x, ref_val->y, ref_val->z));
}

void API_Collider::getRotation(MonoObject* this_ptr, API::Quaternion* out_val)
{
	Collider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	Quaternion rotation = component->GetRotation();

	out_val->x = rotation.x;
	out_val->y = rotation.y;
	out_val->z = rotation.z;
	out_val->w = rotation.w;
}

void API_Collider::setRotation(MonoObject* this_ptr, API::Quaternion* ref_val)
{
	Collider* component;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&component));

	component->SetRotation(Quaternion(ref_val->w, ref_val->x, ref_val->y, ref_val->z));
}
