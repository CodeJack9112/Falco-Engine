#pragma once

#include "Engine.h"

#include "../Bullet/Include/btBulletCollisionCommon.h"
#include "../Bullet/Include/btBulletDynamicsCommon.h"

class RigidBody;
class Vehicle;
class PhysicsManager
{
private:
	struct CollisionInfo
	{
	public:
		CollisionInfo() {}
		CollisionInfo(btCollisionObject* obA, const btVector3 ptA, const btVector3 ptB, const btVector3 normalOnB)
		{
			this->obA = obA;
			this->ptA = ptA;
			this->ptB = ptB;
			this->normalOnB = normalOnB;
		}

		btCollisionObject* obA = nullptr;
		btVector3 ptA;
		btVector3 ptB;
		btVector3 normalOnB;
	};

	btDiscreteDynamicsWorld * dynamicsWorld;
	btDefaultCollisionConfiguration * collisionConfiguration;
	btCollisionDispatcher * dispatcher;
	btBroadphaseInterface * overlappingPairCache;
	btSequentialImpulseConstraintSolver * solver;

	std::vector<RigidBody*> bodies;
	std::vector<Vehicle*> vehicles;
	btClock clock;
	float lastTime = 0;

	std::map<btCollisionObject*, CollisionInfo> m_contacts;

	void checkCollisions();

public:
	PhysicsManager();
	~PhysicsManager();

	void Init();
	void Setup();
	void Setup(SceneNode * root);
	void Update();
	void Reset();

	btDiscreteDynamicsWorld * GetWorld() { return dynamicsWorld; }

	void AddBody(RigidBody * body);
	void DeleteBody(RigidBody * body);
	std::vector<RigidBody*> & GetBodies() { return bodies; }

	void AddVehicle(Vehicle* vehicle);
	void DeleteVehicle(Vehicle* vehicle);
	std::vector<Vehicle*>& GetVehicles() { return vehicles; }
};

