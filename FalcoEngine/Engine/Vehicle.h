#pragma once

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "PhysicsManager.h"
#include <string>

using namespace std;

class Vehicle : public Component
{
public:
	struct WheelInfo
	{
	public:
		std::string m_connectedObjectName = "[None]";
		float m_radius = 0.5f;
		float m_width = 0.4f;
		float m_suspensionStiffness = 20.0f;
		float m_suspensionDamping = 2.3f;
		float m_suspensionCompression = 4.4f;
		float m_suspensionRestLength = 0.6f;
		float m_friction = 1000.0f;
		float m_rollInfluence = 0.1f;
		Vector3 m_direction = Vector3(0, -1, 0);
		Vector3 m_axle = Vector3(-1, 0, 0);
		Vector3 m_connectionPoint = Vector3(0, 0, 0);
		bool m_isFrontWheel = false;

		SceneNode* m_connectedObjectRef = nullptr;
	};

private:
	const int maxProxies = 32766;
	const int maxOverlap = 65535;

	btRaycastVehicle::btVehicleTuning m_tuning;
	btVehicleRaycaster* m_vehicleRayCaster = nullptr;
	btRaycastVehicle* m_vehicle = nullptr;

	Vector3 vehicleAxis = Vector3(0, 1, 2);
	bool invertForward = false;

	std::vector<Vehicle::WheelInfo> wheels;

public:
	Vehicle(SceneNode* parent);
	virtual ~Vehicle();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	void AutoConfigureAnchors(int wheelIndex);
	std::vector<WheelInfo>& getWheels() { return wheels; }

	void Update();
	btRaycastVehicle* getVehicle() { return m_vehicle; }

	Vector3 getAxis() { return vehicleAxis; }
	void setAxis(Vector3 value) { vehicleAxis = value; }

	bool getInvertForward() { return invertForward; }
	void setInvertForward(bool value) { invertForward = value; }

	virtual void SceneLoaded();
	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);
};

