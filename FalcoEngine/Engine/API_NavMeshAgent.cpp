#include "stdafx.h"
#include "API_NavMeshAgent.h"
#include "NavigationManager.h"
#include "NavMeshAgent.h"
#include <OgreSceneNode.h>

#include "Engine.h"

void API_NavMeshAgent::getTargetPosition(MonoObject * this_ptr, API::Vector3 * out_position)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{	
		out_position->x = agent->GetTargetPosition().x;
		out_position->y = agent->GetTargetPosition().y;
		out_position->z = agent->GetTargetPosition().z;
	}
}

void API_NavMeshAgent::setTargetPosition(MonoObject * this_ptr, API::Vector3 * ref_position)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		agent->SetTargetPosition(Vector3(ref_position->x, ref_position->y, ref_position->z));
	}
}

float API_NavMeshAgent::getRadius(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		return agent->GetRadius();
	}

	return 0;
}

void API_NavMeshAgent::setRadius(MonoObject * this_ptr, float radius)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		agent->SetRadius(radius);
	}
}

float API_NavMeshAgent::getHeight(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		return agent->GetHeight();
	}

	return 0;
}

void API_NavMeshAgent::setHeight(MonoObject * this_ptr, float height)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		agent->SetHeight(height);
	}
}

float API_NavMeshAgent::getSpeed(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		return agent->GetSpeed();
	}

	return 0;
}

void API_NavMeshAgent::setSpeed(MonoObject * this_ptr, float speed)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		agent->SetSpeed(speed);
	}
}

float API_NavMeshAgent::getAcceleration(MonoObject * this_ptr)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		return agent->GetAcceleration();
	}

	return 0;
}

void API_NavMeshAgent::setAcceleration(MonoObject * this_ptr, float acceleration)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	NavMeshAgent* agent = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&agent));

	if (agent != nullptr)
	{
		agent->SetAcceleration(acceleration);
	}
}
