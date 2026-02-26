#include "stdafx.h"
#include "RigidBody.h"
#include "Collider.h"
#include "SkeletonBone.h"
#include <iostream>

#include "Procedural/ProceduralStableHeaders.h"
#include "Procedural/Procedural.h"
#include "CapsuleCollider.h"
#include "GUIDGenerator.h"

std::string RigidBody::COMPONENT_TYPE = "Rigidbody";

RigidBody::RigidBody(SceneNode * parent) : Component(parent, GetEngine->GetMonoRuntime()->rigidbody_class)
{
	GetEngine->GetPhysicsManager()->AddBody(this);
	SceneNode * node = parent;
}

RigidBody::~RigidBody()
{
	GetEngine->GetPhysicsManager()->DeleteBody(this);
	physicsActive = false;

	if (body != nullptr)
	{
		if (body->getCollisionShape() != nullptr)
			delete body->getCollisionShape();

		GetEngine->GetPhysicsManager()->GetWorld()->removeRigidBody(body);
	}

	//delete motionState;
	delete body;

	if (motionState != nullptr)
		delete motionState;
}

void RigidBody::SetupPhysics(bool initialize)
{
	if (initialize)
	{
		initialized = true;
	}

	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	ProcessColliders();

	collisionFlags = body->getCollisionFlags();
	SetIsKinematic(GetIsKinematic());

	physicsActive = true;

	if (useOwnGravity)
		body->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void RigidBody::ProcessColliders()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	btCompoundShape * mainShape = new btCompoundShape();
	
	if (body != nullptr)
	{
		if (body->getCollisionShape() != nullptr)
			delete body->getCollisionShape();

		GetEngine->GetPhysicsManager()->GetWorld()->removeRigidBody(body);
		delete body;
	}

	if (motionState != nullptr)
		delete motionState;

	isTrigger = false;

	for (std::vector<Component*>::iterator it = GetParentSceneNode()->components.begin(); it != GetParentSceneNode()->components.end(); ++it)
	{
		if (dynamic_cast<Collider*>(*it))
		{
			Collider * col = (Collider*)(*it);
			if (!col->GetEnabled())
				continue;

			if (col->GetIsTrigger())
				isTrigger = true;

			Vector3 scl = GetParentSceneNode()->_getDerivedScale();
			btCollisionShape * shape = col->GetCollisionShape();

			if (shape != nullptr)
			{
				shape->setLocalScaling(btVector3(scl.x, scl.y, scl.z));

				btTransform shapeTransform;
				shapeTransform.setIdentity();
				shapeTransform.setOrigin(btVector3(col->GetOffset().x * scl.x, col->GetOffset().y * scl.y, col->GetOffset().z * scl.z));
				shapeTransform.setRotation(btQuaternion(col->GetRotation().x, col->GetRotation().y, col->GetRotation().z, col->GetRotation().w));
				
				mainShape->addChildShape(shapeTransform, shape);
			}
		}
	}
	
	centerOfMass.setIdentity();
	btVector3 localInertia(0, 0, 0);

	if (!isStatic && mass > 0 && mainShape->getNumChildShapes() > 0)
	{
		btVector3 principalInertia;
		btScalar* masses = new btScalar[mainShape->getNumChildShapes()];
		for (int j = 0; j < mainShape->getNumChildShapes(); j++)
		{
			masses[j] = mass / (float)mainShape->getNumChildShapes();
		}

		mainShape->calculatePrincipalAxisTransform(masses, centerOfMass, principalInertia);

		for (int i = 0; i < mainShape->getNumChildShapes(); i++)
		{
			btTransform newChildTransform = centerOfMass.inverse() * mainShape->getChildTransform(i);
			mainShape->updateChildTransform(i, newChildTransform);
		}

		mainShape->calculateLocalInertia(mass, localInertia);
	}

	SceneNode* node = GetParentSceneNode();

	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(node->_getDerivedPosition().x, node->_getDerivedPosition().y, node->_getDerivedPosition().z));
	startTransform.setRotation(btQuaternion(node->_getDerivedOrientation().x, node->_getDerivedOrientation().y, node->_getDerivedOrientation().z, node->_getDerivedOrientation().w));

	motionState = new btDefaultMotionState(startTransform * centerOfMass);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, mainShape, localInertia);
	body = new btRigidBody(rbInfo);

	if (isTrigger)
		body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	
	body->setActivationState(DISABLE_DEACTIVATION);

	if (!isStatic)
		body->setMassProps(mass, localInertia);
	else
		body->setMassProps(0, localInertia);

	body->setLinearFactor(btVector3(!freezePositionX, !freezePositionY, !freezePositionZ));
	body->setAngularFactor(btVector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
	body->setFriction(friction);
	body->setRestitution(bounciness);
	body->setDamping(linearDamping, angularDamping);

	body->updateInertiaTensor();
	body->clearForces();

	GetEngine->GetPhysicsManager()->GetWorld()->addRigidBody(body);
}

void RigidBody::Update()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	if (!physicsActive)
		return;

	if (body == nullptr)
		return;

	SceneNode* parent = GetParentSceneNode();

	if (parent != nullptr)
	{
		if (!isKinematic)
		{
			btTransform trans;

			if (body->getMotionState() != nullptr)
			{
				body->getMotionState()->getWorldTransform(trans);
				trans = trans * centerOfMass.inverse();
			}

			parent->_setDerivedPosition(Vector3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
			parent->_setDerivedOrientation(Quaternion(float(trans.getRotation().getW()), float(trans.getRotation().getX()), float(trans.getRotation().getY()), float(trans.getRotation().getZ())));
		}
		else
		{
			SetPosition(parent->_getDerivedPosition());
			SetRotation(parent->_getDerivedOrientation());
		}
	}
}

float RigidBody::GetMass()
{
	return mass;
}

void RigidBody::SetMass(float mass)
{
	this->mass = mass;

	if (body != nullptr)
	{
		btVector3 localInertia(0, 0, 0);
		body->setMassProps(mass, localInertia);
		body->updateInertiaTensor();
	}
}

void RigidBody::SetGravity(Vector3 value)
{
	gravity = value;
	if (body != nullptr)
		body->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void RigidBody::SetFriction(float value)
{
	friction = value;
	if (body != nullptr)
	{
		body->setFriction(friction);
		body->updateInertiaTensor();
	}
}

void RigidBody::SetLinearDamping(float value)
{
	linearDamping = value;
	if (body != nullptr)
	{
		body->setDamping(linearDamping, angularDamping);
		body->updateInertiaTensor();
	}
}

void RigidBody::SetAngularDamping(float value)
{
	angularDamping = value;
	if (body != nullptr)
	{
		body->setDamping(linearDamping, angularDamping);
		body->updateInertiaTensor();
	}
}

void RigidBody::SetBounciness(float value)
{
	bounciness = value;
	if (body != nullptr)
	{
		body->setRestitution(bounciness);
		body->updateInertiaTensor();
	}
}

Vector3 RigidBody::GetPosition()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return GetParentSceneNode()->_getDerivedPosition();

	btTransform transform;
	if (!isKinematic)
		transform = body->getCenterOfMassTransform() * centerOfMass.inverse();
	else
	{
		btDefaultMotionState* mState = (btDefaultMotionState*)body->getMotionState();
		mState->getWorldTransform(transform);
		transform = transform * centerOfMass.inverse();
	}

	btVector3 p = transform.getOrigin();

	return Vector3(p.getX(), p.getY(), p.getZ());
}

Quaternion RigidBody::GetRotation()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return GetParentSceneNode()->_getDerivedOrientation();

	btTransform transform;
	if (!isKinematic)
		transform = body->getCenterOfMassTransform() * centerOfMass.inverse();
	else
	{
		btDefaultMotionState* mState = (btDefaultMotionState*)body->getMotionState();
		mState->getWorldTransform(transform);
		transform = transform * centerOfMass.inverse();
	}

	btQuaternion q = transform.getRotation();

	return Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}

void RigidBody::SetPosition(Vector3 position)
{
	if (body != nullptr)
	{
		if (!isKinematic)
		{
			btTransform transform = body->getCenterOfMassTransform() * centerOfMass.inverse();
			transform.setOrigin(btVector3(position.x, position.y, position.z));
			body->setCenterOfMassTransform(transform * centerOfMass);
		}
		else
		{
			btDefaultMotionState* mState = (btDefaultMotionState*)body->getMotionState();

			btTransform transform;
			mState->getWorldTransform(transform);
			transform = transform * centerOfMass.inverse();

			transform.setOrigin(btVector3(position.x, position.y, position.z));
			mState->setWorldTransform(transform * centerOfMass);
		}
	}
}

void RigidBody::SetRotation(Quaternion rotation)
{
	if (body != nullptr)
	{
		if (!isKinematic)
		{
			btTransform transform;
			transform = body->getCenterOfMassTransform() * centerOfMass.inverse();
			btQuaternion r;
			r.setValue(rotation.x, rotation.y, rotation.z, rotation.w);
			transform.setRotation(r);
			body->setCenterOfMassTransform(transform * centerOfMass);
		}
		else
		{
			btDefaultMotionState* mState = (btDefaultMotionState*)body->getMotionState();

			btTransform transform;
			mState->getWorldTransform(transform);
			transform = transform * centerOfMass.inverse();

			btQuaternion r;
			r.setValue(rotation.x, rotation.y, rotation.z, rotation.w);
			transform.setRotation(r);

			mState->setWorldTransform(transform * centerOfMass);
		}
	}
}

void RigidBody::AddForce(Vector3 force, Vector3 pos)
{
	if (body != nullptr)
	{
		body->applyForce(btVector3(force.x, force.y, force.z), btVector3(pos.x, pos.y, pos.z));
	}
}

void RigidBody::AddTorque(Vector3 torque)
{
	if (body != nullptr)
	{
		body->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}
}

Vector3 RigidBody::GetLinearVelocity()
{
	btVector3 vel = body->getLinearVelocity();

	return Vector3(vel.getX(), vel.getY(), vel.getZ());
}

void RigidBody::SetLinearVelocity(Vector3 velocity)
{
	body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

Vector3 RigidBody::GetAngularVelocity()
{
	btVector3 vel = body->getAngularVelocity();

	return Vector3(vel.getX(), vel.getY(), vel.getZ());
}

void RigidBody::SetAngularVelocity(Vector3 velocity)
{
	body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

void RigidBody::SetFreezePositionX(bool value)
{
	freezePositionX = value;
	if (body != nullptr)
		body->setLinearFactor(btVector3(!freezePositionX, !freezePositionY, !freezePositionZ));
}

void RigidBody::SetFreezePositionY(bool value)
{
	freezePositionY = value;
	if (body != nullptr)
		body->setLinearFactor(btVector3(!freezePositionX, !freezePositionY, !freezePositionZ));
}

void RigidBody::SetFreezePositionZ(bool value)
{
	freezePositionZ = value;
	if (body != nullptr)
		body->setLinearFactor(btVector3(!freezePositionX, !freezePositionY, !freezePositionZ));
}

void RigidBody::SetFreezeRotationX(bool value)
{
	freezeRotationX = value;
	if (body != nullptr)
		body->setAngularFactor(btVector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
}

void RigidBody::SetFreezeRotationY(bool value)
{
	freezeRotationY = value;
	if (body != nullptr)
		body->setAngularFactor(btVector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
}

void RigidBody::SetFreezeRotationZ(bool value)
{
	freezeRotationZ = value;
	if (body != nullptr)
		body->setAngularFactor(btVector3(!freezeRotationX, !freezeRotationY, !freezeRotationZ));
}

void RigidBody::SetIsKinematic(bool value)
{
	isKinematic = value;

	if (body != nullptr)
	{
		if (isKinematic)
		{
			body->setCollisionFlags(collisionFlags | btCollisionObject::CF_KINEMATIC_OBJECT);
		}
		else
		{
			body->setCollisionFlags(collisionFlags);
		}
	}
}

void RigidBody::SceneLoaded()
{
	//MaterialPtr wireframeMaterial;

	//if (!MaterialManager::getSingleton().resourceExists("WireframeMaterial_111", "Assets"))
	//	wireframeMaterial = MaterialManager::getSingleton().create("WireframeMaterial_111", "Assets");
	//else
	//	wireframeMaterial = MaterialManager::getSingleton().getByName("WireframeMaterial_111", "Assets");

	//wireframeMaterial->setLightingEnabled(false);
	//wireframeMaterial->setReceiveShadows(false);
	//wireframeMaterial->setDepthBias(1, 0);
	//wireframeMaterial->removeAllTechniques();
	//wireframeMaterial->createTechnique();
	//wireframeMaterial->getTechnique(0)->createPass();
	//wireframeMaterial->getTechnique(0)->getPass(0)->setLineWidth(1.5);
	//wireframeMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PolygonMode::PM_WIREFRAME);
	//wireframeMaterial->getTechnique(0)->getPass(0)->setSceneBlendingOperation(Ogre::SceneBlendOperation::SBO_ADD);
	//wireframeMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	//wireframeMaterial->getTechnique(0)->setSchemeName("NoGBuffer");

	//SceneNode * wireframeNode = GetParentSceneNode()->createChildSceneNode("bone" + GetParentSceneNode()->getName() + to_string(rand()));

	//float r = 0.5;
	//float h = 3;

	//CapsuleCollider * capsule = (CapsuleCollider*)GetParentSceneNode()->GetComponent(CapsuleCollider::COMPONENT_TYPE);

	//if (capsule != nullptr)
	//{
	//	r = capsule->GetRadius();
	//	h = capsule->GetHeight();
	//}

	//MeshPtr capsuleMesh = Procedural::CapsuleGenerator().setHeight(h).setRadius(r).realizeMesh("Sphere" + GetParentSceneNode()->getName() + to_string(rand()));
	//Entity * capsuleEntity = GetEngine->GetSceneManager()->createEntity("Sphere_ent" + GetParentSceneNode()->getName() + to_string(rand()), capsuleMesh);
	//capsuleEntity->setMaterial(wireframeMaterial);
	////capsuleEntity->setQueryFlags(1 << 3);
	////capsuleEntity->setRenderQueueGroup(RENDER_QUEUE_9 - 1);

	//wireframeNode->attachObject(capsuleEntity);
	//if (capsule != nullptr)
	//{
	//	wireframeNode->setPosition(capsule->GetOffset());
	//	wireframeNode->_setDerivedOrientation(GetParentSceneNode()->_getDerivedOrientation() * capsule->GetRotation());
	//}
}

void RigidBody::StateChanged(bool active)
{
	//return;
	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (initialized)
		{
			if (active == false)
			{
				if (physicsActive)
				{
					if (body != nullptr)
					{
						GetEngine->GetPhysicsManager()->DeleteBody(this);
						GetEngine->GetPhysicsManager()->GetWorld()->removeRigidBody(body);
						physicsActive = false;
					}
				}
			}
			else
			{
				if (!physicsActive)
				{
					if (GetParentSceneNode()->getVisible())
					{
						SetupPhysics(false);
						GetEngine->GetPhysicsManager()->AddBody(this);
					}
				}
			}
		}
	}
}

void RigidBody::NodeStateChanged(bool active)
{
	StateChanged(active);
}
