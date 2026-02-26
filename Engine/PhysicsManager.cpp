#include "stdafx.h"
#include "PhysicsManager.h"
#include "RigidBody.h"
#include "Vehicle.h"
#include <iostream>

PhysicsManager::PhysicsManager()
{
}

PhysicsManager::~PhysicsManager()
{
	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;
}

void PhysicsManager::Init()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;

	//collisionConfiguration->setConvexConvexMultipointIterations(7, 7);

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

void PhysicsManager::Setup()
{
	for (std::vector<RigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
	{
		(*it)->SetupPhysics(true);
		//(*it)->ProcessColliders();
	}
}

void PhysicsManager::Setup(SceneNode * root)
{
	RigidBody * body = (RigidBody*)root->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		body->SetupPhysics(true);
		//body->ProcessColliders();
	}

	VectorIterator it = root->getChildIterator();
	while (it.hasMoreElements())
	{
		SceneNode * node = (SceneNode*)it.getNext();

		Setup(node);
	}
}

void PhysicsManager::Update()
{
	//Update all rigidbodies
	for (std::vector<RigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
	{
		(*it)->Update();
	}

	//Update all vehicles
	for (std::vector<Vehicle*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
	{
		(*it)->Update();
	}

	//Physics step
	double now = clock.getTimeSeconds();
	dynamicsWorld->stepSimulation(((now - lastTime) * 2.0) * GetEngine->getTimeScale(), 10, 1.0 / 120.0);
	lastTime = now;

	checkCollisions();
}

void PhysicsManager::checkCollisions()
{
	std::map<btCollisionObject*, CollisionInfo> newContacts;

	/* Browse all collision pairs */
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
		btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

		/* Check all contacts points */
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;

				if (newContacts.find(obB) == newContacts.end())
				{
					newContacts[obB] = CollisionInfo(obA, ptA, ptB, normalOnB);
				}
			}
		}
	}

	/* Check for added contacts ... */
	if (!newContacts.empty())
	{
		for (auto it = newContacts.begin(); it != newContacts.end(); it++)
		{
			if (m_contacts.find((*it).first) == m_contacts.end())
			{
				//std::cout << "Collision detected" << std::endl;
				
				//Signal
				for each (RigidBody * body in bodies)
				{
					if (body->GetNativeBody() == it->second.obA)
					{
						SceneNode* thisNode = body->GetParentSceneNode();
						RigidBody* otherBody = nullptr;

						for each (RigidBody * body2 in bodies)
						{
							if (body2->GetNativeBody() == it->first)
							{
								otherBody = body2;
								break;
							}
						}

						if (otherBody != nullptr)
						{
							btVector3 ptx = it->second.ptB;
							btVector3 nmx = it->second.normalOnB;

							float px = ptx.x();
							float py = ptx.y();
							float pz = ptx.z();

							float nx = nmx.x();
							float ny = nmx.y();
							float nz = nmx.z();

							//Contact point
							MonoObject* pt = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector3_class);
							mono_field_set_value(pt, GetEngine->GetMonoRuntime()->vector3_x, &(float)px);
							mono_field_set_value(pt, GetEngine->GetMonoRuntime()->vector3_y, &(float)py);
							mono_field_set_value(pt, GetEngine->GetMonoRuntime()->vector3_z, &(float)pz);

							void* _point = mono_object_unbox(pt);

							//Contact normal
							MonoObject* nm = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector3_class);
							mono_field_set_value(nm, GetEngine->GetMonoRuntime()->vector3_x, &(float)nx);
							mono_field_set_value(nm, GetEngine->GetMonoRuntime()->vector3_y, &(float)ny);
							mono_field_set_value(nm, GetEngine->GetMonoRuntime()->vector3_z, &(float)nz);

							void* _normal = mono_object_unbox(nm);

							//A
							MonoObject* collisionStructA = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->collision_class);
							mono_field_set_value(collisionStructA, GetEngine->GetMonoRuntime()->collision_other, otherBody->GetMonoObject());
							mono_field_set_value(collisionStructA, GetEngine->GetMonoRuntime()->collision_point, _point);
							mono_field_set_value(collisionStructA, GetEngine->GetMonoRuntime()->collision_normal, _normal);

							//B
							MonoObject* collisionStructB = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->collision_class);
							mono_field_set_value(collisionStructB, GetEngine->GetMonoRuntime()->collision_other, body->GetMonoObject());
							mono_field_set_value(collisionStructB, GetEngine->GetMonoRuntime()->collision_point, _point);
							mono_field_set_value(collisionStructB, GetEngine->GetMonoRuntime()->collision_normal, _normal);

							void* _colA = mono_object_unbox(collisionStructA);
							void* _colB = mono_object_unbox(collisionStructB);

							void* argsA[1] = { _colA };
							void* argsB[1] = { _colB };

							SceneNode* otherNode = otherBody->GetParentSceneNode();

							GetEngine->GetMonoRuntime()->ExecuteForNode(thisNode, "OnCollisionEnter", "", argsA, "Collision");
							GetEngine->GetMonoRuntime()->ExecuteForNode(otherNode, "OnCollisionEnter", "", argsB, "Collision");

							if (body->IsTrigger())
							{
								void* argsC[1] = { otherBody->GetMonoObject() };
								GetEngine->GetMonoRuntime()->ExecuteForNode(thisNode, "OnTriggerEnter", "", argsC, "Rigidbody");
							}

							if (otherBody->IsTrigger())
							{
								void* argsC[1] = { body->GetMonoObject() };
								GetEngine->GetMonoRuntime()->ExecuteForNode(otherNode, "OnTriggerEnter", "", argsC, "Rigidbody");
							}

							break;
						}
					}
				}
			}
			else
			{
				// Remove to filter no more active contacts
				m_contacts.erase((*it).first);
			}
		}
	}

	/* ... and removed contacts */
	if (!m_contacts.empty())
	{
		for (auto it = m_contacts.begin(); it != m_contacts.end(); it++)
		{
			//std::cout << "End of collision detected" << std::endl;
			
			//Signal
			for each (RigidBody * body in bodies)
			{
				if (body->GetNativeBody() == it->second.obA)
				{
					SceneNode* thisNode = body->GetParentSceneNode();
					RigidBody* otherBody = nullptr;

					for each (RigidBody * body2 in bodies)
					{
						if (body2->GetNativeBody() == it->first)
						{
							otherBody = body2;
							break;
						}
					}

					if (otherBody != nullptr)
					{
						btVector3 ptx = it->second.ptB;
						btVector3 nmx = it->second.normalOnB;

						float px = ptx.x();
						float py = ptx.y();
						float pz = ptx.z();

						float nx = nmx.x();
						float ny = nmx.y();
						float nz = nmx.z();

						//Contact point
						MonoObject* pt = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector3_class);
						mono_field_set_value(pt, GetEngine->GetMonoRuntime()->vector3_x, &(float)px);
						mono_field_set_value(pt, GetEngine->GetMonoRuntime()->vector3_y, &(float)py);
						mono_field_set_value(pt, GetEngine->GetMonoRuntime()->vector3_z, &(float)pz);

						void* _point = mono_object_unbox(pt);

						//Contact normal
						MonoObject* nm = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->vector3_class);
						mono_field_set_value(nm, GetEngine->GetMonoRuntime()->vector3_x, &(float)nx);
						mono_field_set_value(nm, GetEngine->GetMonoRuntime()->vector3_y, &(float)ny);
						mono_field_set_value(nm, GetEngine->GetMonoRuntime()->vector3_z, &(float)nz);

						void* _normal = mono_object_unbox(nm);

						//A
						MonoObject* collisionStructA = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->collision_class);
						mono_field_set_value(collisionStructA, GetEngine->GetMonoRuntime()->collision_other, otherBody->GetMonoObject());
						mono_field_set_value(collisionStructA, GetEngine->GetMonoRuntime()->collision_point, _point);
						mono_field_set_value(collisionStructA, GetEngine->GetMonoRuntime()->collision_normal, _normal);

						//B
						MonoObject* collisionStructB = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), GetEngine->GetMonoRuntime()->collision_class);
						mono_field_set_value(collisionStructB, GetEngine->GetMonoRuntime()->collision_other, body->GetMonoObject());
						mono_field_set_value(collisionStructB, GetEngine->GetMonoRuntime()->collision_point, _point);
						mono_field_set_value(collisionStructB, GetEngine->GetMonoRuntime()->collision_normal, _normal);

						void* _colA = mono_object_unbox(collisionStructA);
						void* _colB = mono_object_unbox(collisionStructB);

						void* argsA[1] = { _colA };
						void* argsB[1] = { _colB };

						SceneNode* otherNode = otherBody->GetParentSceneNode();

						GetEngine->GetMonoRuntime()->ExecuteForNode(thisNode, "OnCollisionExit", "", argsA, "Collision");
						GetEngine->GetMonoRuntime()->ExecuteForNode(otherNode, "OnCollisionExit", "", argsB, "Collision");

						if (body->IsTrigger())
						{
							void* argsC[1] = { otherBody->GetMonoObject() };
							GetEngine->GetMonoRuntime()->ExecuteForNode(thisNode, "OnTriggerExit", "", argsC, "Rigidbody");
						}

						if (otherBody->IsTrigger())
						{
							void* argsC[1] = { body->GetMonoObject() };
							GetEngine->GetMonoRuntime()->ExecuteForNode(otherNode, "OnTriggerExit", "", argsC, "Rigidbody");
						}

						break;
					}
				}
			}
		}

		m_contacts.clear();
	}

	m_contacts = newContacts;
}

void PhysicsManager::Reset()
{
	for (std::vector<RigidBody*>::iterator it = bodies.begin(); it != bodies.end(); ++it)
	{
		if ((*it)->GetNativeBody() != nullptr)
			dynamicsWorld->removeRigidBody((*it)->GetNativeBody());
	}

	bodies.clear();

	//delete dynamics world
	delete dynamicsWorld;
	//delete solver
	delete solver;
	//delete broadphase
	delete overlappingPairCache;
	//delete dispatcher
	delete dispatcher;
	delete collisionConfiguration;

	Init();
}

void PhysicsManager::AddBody(RigidBody * body)
{
	bodies.push_back(body);
}

void PhysicsManager::DeleteBody(RigidBody * body)
{
	auto it = std::find(bodies.begin(), bodies.end(), body);
	if (it != bodies.end())
		bodies.erase(it);
}

void PhysicsManager::AddVehicle(Vehicle* vehicle)
{
	vehicles.push_back(vehicle);
}

void PhysicsManager::DeleteVehicle(Vehicle* vehicle)
{
	auto it = std::find(vehicles.begin(), vehicles.end(), vehicle);
	if (it != vehicles.end())
		vehicles.erase(it);
}
