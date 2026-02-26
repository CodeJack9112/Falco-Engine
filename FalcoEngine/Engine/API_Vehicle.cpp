#include "API_Vehicle.h"
#include "stdafx.h"
#include "RigidBody.h"
#include "Vehicle.h"
#include <OgreSceneNode.h>

int API_Vehicle::getNumWheels(MonoObject* this_ptr, int wheel)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	Vehicle* vehicle = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&vehicle));

	if (vehicle != nullptr)
	{
		return vehicle->getWheels().size();
	}
}

float API_Vehicle::getSteering(MonoObject* this_ptr, int wheel)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	Vehicle* vehicle = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&vehicle));

	if (vehicle != nullptr)
	{
		if (wheel < vehicle->getWheels().size())
			return Math::fRad2Deg * vehicle->getVehicle()->getSteeringValue(wheel);
		else
			return 0;
	}
}

void API_Vehicle::setSteering(MonoObject* this_ptr, float angle, int wheel)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	Vehicle* vehicle = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&vehicle));

	if (vehicle != nullptr)
	{
		if (wheel < vehicle->getWheels().size())
			vehicle->getVehicle()->setSteeringValue(Math::fDeg2Rad * angle, wheel);
	}
}

void API_Vehicle::setBreak(MonoObject* this_ptr, float value, int wheel)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	Vehicle* vehicle = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&vehicle));

	if (vehicle != nullptr)
	{
		if (wheel < vehicle->getWheels().size())
			vehicle->getVehicle()->setBrake(value, wheel);
	}
}

void API_Vehicle::applyEngineForce(MonoObject* this_ptr, float value, int wheel)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	Vehicle* vehicle = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&vehicle));

	if (vehicle != nullptr)
	{
		if (wheel < vehicle->getWheels().size())
			vehicle->getVehicle()->applyEngineForce(value, wheel);
	}
}