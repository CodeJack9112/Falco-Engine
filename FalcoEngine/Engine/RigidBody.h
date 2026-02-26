#pragma once

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "PhysicsManager.h"
#include <string>

using namespace std;

class RigidBody : public Component
{
private:
	btDefaultMotionState * motionState = nullptr;
	btRigidBody * body = nullptr;
	btTransform startTransform;
	btTransform centerOfMass;
	Quaternion initialRotation = Quaternion::IDENTITY;
	int collisionFlags = 0;

	btScalar mass = 1.0f;
	Vector3 gravity = Vector3(0, -9.81f, 0);
	float friction = 1.0f;
	float bounciness = 0.0f;
	float linearDamping = 0.0f;
	float angularDamping = 0.0f;

	bool freezePositionX = false;
	bool freezePositionY = false;
	bool freezePositionZ = false;
	bool freezeRotationX = false;
	bool freezeRotationY = false;
	bool freezeRotationZ = false;
	bool isKinematic = false;

	bool physicsActive = false;
	bool initialized = false;
	bool isStatic = false;
	bool useOwnGravity = false;

	bool isTrigger = false;

public:
	RigidBody(SceneNode * parent);
	virtual ~RigidBody();

	void Update();
	void SetupPhysics(bool initialize);
	void ProcessColliders();

	float GetMass();
	void SetMass(float mass);

	bool GetIsStatic() { return isStatic; }
	void SetIsStatic(bool stat) { isStatic = stat; }

	bool GetUseOwnGravity() { return useOwnGravity; }
	void SetUseOwnGravity(bool val) { useOwnGravity = val; }

	Vector3 GetGravity() { return gravity; }
	void SetGravity(Vector3 value);

	float GetFriction() { return friction; }
	void SetFriction(float value);

	float GetLinearDamping() { return linearDamping; }
	void SetLinearDamping(float value);

	float GetAngularDamping() { return angularDamping; }
	void SetAngularDamping(float value);

	float GetBounciness() { return bounciness; }
	void SetBounciness(float value);

	bool IsTrigger() { return isTrigger; }

	btTransform GetCenterOfMass() { return centerOfMass; }

	btRigidBody * GetNativeBody() { return body; }

	Vector3 GetPosition();
	Quaternion GetRotation();
	void SetPosition(Vector3 position);
	void SetRotation(Quaternion rotation);
	void AddForce(Vector3 force, Vector3 pos);
	void AddTorque(Vector3 torque);
	Vector3 GetLinearVelocity();
	void SetLinearVelocity(Vector3 velocity);
	Vector3 GetAngularVelocity();
	void SetAngularVelocity(Vector3 velocity);

	void SetFreezePositionX(bool value);
	void SetFreezePositionY(bool value);
	void SetFreezePositionZ(bool value);
	void SetFreezeRotationX(bool value);
	void SetFreezeRotationY(bool value);
	void SetFreezeRotationZ(bool value);
	void SetIsKinematic(bool value);

	bool GetFreezePositionX() { return freezePositionX; }
	bool GetFreezePositionY() { return freezePositionY; }
	bool GetFreezePositionZ() { return freezePositionZ; }
	bool GetFreezeRotationX() { return freezeRotationX; }
	bool GetFreezeRotationY() { return freezeRotationY; }
	bool GetFreezeRotationZ() { return freezeRotationZ; }
	bool GetIsKinematic() { return isKinematic; }
	bool IsInitialized() { return initialized; }

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	virtual void SceneLoaded();
	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);
};

