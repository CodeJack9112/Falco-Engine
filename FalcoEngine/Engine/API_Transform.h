#pragma once

#include "API.h"

class API_Transform
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_get_position", (void*)getPosition);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_set_position", (void*)setPosition);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_get_scale", (void*)getScale);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_set_scale", (void*)setScale);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_get_rotation", (void*)getRotation);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_set_rotation", (void*)setRotation);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_get_local_position", (void*)getLocalPosition);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_set_local_position", (void*)setLocalPosition);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_get_local_rotation", (void*)getLocalRotation);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_set_local_rotation", (void*)setLocalRotation);
		mono_add_internal_call("FalcoEngine.Transform::get_gameObject", (void*)getGameObject);
		mono_add_internal_call("FalcoEngine.Transform::get_childCount", (void*)getChildCount);
		mono_add_internal_call("FalcoEngine.Transform::get_parent", (void*)getParent);
		mono_add_internal_call("FalcoEngine.Transform::set_parent", (void*)setParent);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_get_child", (void*)getChild);
		mono_add_internal_call("FalcoEngine.Transform::INTERNAL_find_child", (void*)findChild);
	}

private:
	//Get position
	static void getPosition(MonoObject * this_ptr, API::Vector3 * out_pos);

	//Set position
	static void setPosition(MonoObject * this_ptr, API::Vector3 * ref_pos);

	//Get scale
	static void getScale(MonoObject * this_ptr, API::Vector3 * out_scale);

	//Set scale
	static void setScale(MonoObject * this_ptr, API::Vector3 * ref_scale);

	//Get rotation
	static void getRotation(MonoObject * this_ptr, API::Quaternion * out_rot);

	//Set rotation
	static void setRotation(MonoObject * this_ptr, API::Quaternion * ref_rot);

	//Get local position
	static void getLocalPosition(MonoObject * this_ptr, API::Vector3 * out_pos);

	//Set local position
	static void setLocalPosition(MonoObject * this_ptr, API::Vector3 * ref_pos);

	//Get rotation
	static void getLocalRotation(MonoObject * this_ptr, API::Quaternion * out_rot);

	//Set rotation
	static void setLocalRotation(MonoObject * this_ptr, API::Quaternion * ref_rot);

	//Get GameObject
	static MonoObject * getGameObject(MonoObject * this_ptr);

	//Get child count
	static int getChildCount(MonoObject * this_ptr);

	//Get parent
	static MonoObject * getParent(MonoObject * this_ptr);

	//Set parent
	static void setParent(MonoObject * this_ptr, MonoObject * parent);

	//Get child by index
	static MonoObject* getChild(MonoObject* this_ptr, int index);

	//Find child by name
	static MonoObject* findChild(MonoObject* this_ptr, MonoString * name);
};