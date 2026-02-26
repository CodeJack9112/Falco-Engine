#include "stdafx.h"
#include "Vehicle.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>
#include "Mathf.h"

std::string Vehicle::COMPONENT_TYPE = "Vehicle";

Vehicle::Vehicle(SceneNode* parent) : Component(parent, GetEngine->GetMonoRuntime()->vehicle_class)
{
	GetEngine->GetPhysicsManager()->AddVehicle(this);
}

Vehicle::~Vehicle()
{
	GetEngine->GetPhysicsManager()->DeleteVehicle(this);

	if (m_vehicleRayCaster != nullptr)
		GetEngine->GetPhysicsManager()->GetWorld()->removeAction(m_vehicle);
		//delete m_vehicleRayCaster;

	if (m_vehicle != nullptr)
		delete m_vehicle;
}

void Vehicle::AutoConfigureAnchors(int wheelIndex)
{
	WheelInfo & inf = wheels.at(wheelIndex);

	SceneNode* connectedNode = GetEngine->GetSceneManager()->getSceneNodeFast(inf.m_connectedObjectName);
	if (connectedNode != nullptr)
	{
		//Vector3 a1 = GetParentSceneNode()->_getDerivedScale() * (GetParentSceneNode()->_getDerivedOrientation() * anchor);
		inf.m_connectionPoint = Mathf::inverseTransformPoint(GetParentSceneNode(), connectedNode->_getDerivedPosition()/* + a1*/);
	}
}

btVector3 bulletVec3(Vector3 vec3)
{
	return btVector3(vec3.x, vec3.y, vec3.z);
}

Vector3 vec3Bullet(btVector3 vec3)
{
	return Vector3(vec3.getX(), vec3.getY(), vec3.getZ());
}

Quaternion quatBullet(btQuaternion quat)
{
	return Quaternion(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
}

btQuaternion bulletQuat(Quaternion quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

void Vehicle::SceneLoaded()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		RigidBody* body = (RigidBody*)GetParentSceneNode()->GetComponent(RigidBody::COMPONENT_TYPE);

		if (body != nullptr)
		{
			btRigidBody* body1 = body->GetNativeBody();

			if (body1 != nullptr)
			{
				body1->setActivationState(DISABLE_DEACTIVATION);

				m_vehicleRayCaster = new btDefaultVehicleRaycaster(GetEngine->GetPhysicsManager()->GetWorld());
				m_vehicle = new btRaycastVehicle(m_tuning, body1, m_vehicleRayCaster);

				//choose coordinate system
				m_vehicle->setCoordinateSystem(vehicleAxis.x, vehicleAxis.y, vehicleAxis.z);

				float scale = GetParentSceneNode()->_getDerivedScale().y;
				Quaternion rotation = GetParentSceneNode()->_getDerivedOrientation();

				for (auto it = wheels.begin(); it != wheels.end(); ++it)
				{
					WheelInfo & wheelInfo = *it;
					
					Vector3 offset = vec3Bullet(body1->getCenterOfMassTransform().getOrigin()) - GetParentSceneNode()->_getDerivedPosition();

					btWheelInfo& wheel = m_vehicle->addWheel(bulletVec3(wheelInfo.m_connectionPoint * GetParentSceneNode()->_getDerivedScale() - offset),
						bulletVec3(rotation.Inverse() * wheelInfo.m_direction),
						bulletVec3(rotation.Inverse() * wheelInfo.m_axle),
						wheelInfo.m_suspensionRestLength,
						wheelInfo.m_radius * scale,
						m_tuning,
						wheelInfo.m_isFrontWheel);

					wheel.m_suspensionStiffness = wheelInfo.m_suspensionStiffness;
					wheel.m_wheelsDampingRelaxation = wheelInfo.m_suspensionDamping;
					wheel.m_wheelsDampingCompression = wheelInfo.m_suspensionCompression;
					wheel.m_frictionSlip = wheelInfo.m_friction;
					wheel.m_rollInfluence = wheelInfo.m_rollInfluence;
					//wheel.m_maxSuspensionTravelCm = 20.0f;

					if (!wheelInfo.m_connectedObjectName.empty())
						wheelInfo.m_connectedObjectRef = GetEngine->GetSceneManager()->getSceneNodeFast(wheelInfo.m_connectedObjectName);
				}
				
				GetEngine->GetPhysicsManager()->GetWorld()->addAction(m_vehicle);
			}
		}
	}
}

void Vehicle::Update()
{
	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
	{
		m_vehicle->updateWheelTransform(i, true);
		
		btWheelInfo& winf = m_vehicle->getWheelInfo(i);
		btTransform trans = winf.m_worldTransform;
		
		Vector3 position = vec3Bullet(trans.getOrigin());
		Quaternion rotation = quatBullet(trans.getRotation());

		btVector3 right = -winf.m_raycastInfo.m_wheelAxleWS;
		btVector3 up = -winf.m_raycastInfo.m_wheelDirectionWS;
		btVector3 fwd = up.cross(right);
		fwd = fwd.normalize();

		btScalar steering = winf.m_steering;

		btQuaternion steeringOrn(up, steering);
		btMatrix3x3 steeringMat(steeringOrn);

		btQuaternion rotatingOrn(right, invertForward ? -winf.m_rotation : winf.m_rotation);
		btMatrix3x3 rotatingMat(rotatingOrn);

		btMatrix3x3 basis2;
		basis2[0][0] = right[0];
		basis2[1][0] = right[1];
		basis2[2][0] = right[2];

		basis2[0][1] = up[0];
		basis2[1][1] = up[1];
		basis2[2][1] = up[2];

		basis2[0][2] = -fwd[0];
		basis2[1][2] = -fwd[1];
		basis2[2][2] = -fwd[2];

		btTransform m_worldTransform;
		m_worldTransform.setBasis(steeringMat * rotatingMat * basis2);

		rotation = quatBullet(m_worldTransform.getRotation());

		WheelInfo & inf = wheels.at(i);
		if (inf.m_connectedObjectRef != nullptr)
		{
			inf.m_connectedObjectRef->_setDerivedPosition(position);
			inf.m_connectedObjectRef->_setDerivedOrientation(rotation);
		}
	}
}

void Vehicle::StateChanged(bool active)
{
	RigidBody* body = (RigidBody*)GetParentSceneNode()->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		if (body->IsInitialized())
		{
			if (active)
			{
				SceneLoaded();
				GetEngine->GetPhysicsManager()->AddVehicle(this);
			}
		}
	}

	if (!active)
	{
		if (m_vehicle != nullptr)
			GetEngine->GetPhysicsManager()->GetWorld()->removeAction(m_vehicle);

		GetEngine->GetPhysicsManager()->DeleteVehicle(this);
	}
}

void Vehicle::NodeStateChanged(bool active)
{
	StateChanged(active);
}