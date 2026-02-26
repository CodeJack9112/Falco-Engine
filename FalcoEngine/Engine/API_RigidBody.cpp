#include "stdafx.h"
#include "API_RigidBody.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>

void API_RigidBody::getPosition(MonoObject * this_ptr, API::Vector3 * out_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		Vector3 p = body->GetPosition();

		out_pos->x = p.x;
		out_pos->y = p.y;
		out_pos->z = p.z;
	}
}

void API_RigidBody::setPosition(MonoObject * this_ptr, API::Vector3 * ref_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->SetPosition(Vector3(ref_pos->x, ref_pos->y, ref_pos->z));
	}
}

void API_RigidBody::getRotation(MonoObject * this_ptr, API::Quaternion * out_rot)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		Quaternion q = body->GetRotation();

		out_rot->x = q.x;
		out_rot->y = q.y;
		out_rot->z = q.z;
		out_rot->w = q.w;
	}
}

void API_RigidBody::setRotation(MonoObject * this_ptr, API::Quaternion * ref_rot)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->SetRotation(Quaternion(ref_rot->w, ref_rot->x, ref_rot->y, ref_rot->z));
	}
}

void API_RigidBody::getLinearVelocity(MonoObject * this_ptr, API::Vector3 * out_vel)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		Vector3 v = body->GetLinearVelocity();

		out_vel->x = v.x;
		out_vel->y = v.y;
		out_vel->z = v.z;
	}
}

void API_RigidBody::setLinearVelocity(MonoObject * this_ptr, API::Vector3 * ref_vel)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->SetLinearVelocity(Vector3(ref_vel->x, ref_vel->y, ref_vel->z));
	}
}

void API_RigidBody::getAngularVelocity(MonoObject * this_ptr, API::Vector3 * out_vel)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		Vector3 v = body->GetAngularVelocity();

		out_vel->x = v.x;
		out_vel->y = v.y;
		out_vel->z = v.z;
	}
}

void API_RigidBody::setAngularVelocity(MonoObject * this_ptr, API::Vector3 * ref_vel)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->SetAngularVelocity(Vector3(ref_vel->x, ref_vel->y, ref_vel->z));
	}
}

void API_RigidBody::addForce(MonoObject * this_ptr, API::Vector3 * ref_force, API::Vector3 * ref_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->AddForce(Vector3(ref_force->x, ref_force->y, ref_force->z), Vector3(ref_pos->x, ref_pos->y, ref_pos->z));
	}
}

void API_RigidBody::addTorque(MonoObject * this_ptr, API::Vector3 * ref_torque)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->AddTorque(Vector3(ref_torque->x, ref_torque->y, ref_torque->z));
	}
}

bool API_RigidBody::getIsKinematic(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		return body->GetIsKinematic();
	}

	return false;
}

void API_RigidBody::setIsKinematic(MonoObject * this_ptr, bool value)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->SetIsKinematic(value);
	}
}

float API_RigidBody::getMass(MonoObject* this_ptr)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		return body->GetMass();
	}

	return 0.0f;
}

void API_RigidBody::setMass(MonoObject* this_ptr, float value)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody* body = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&body));

	if (body != nullptr)
	{
		body->SetMass(value);
	}
}
