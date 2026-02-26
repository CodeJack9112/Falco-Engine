#include "stdafx.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>

std::string BoxCollider::COMPONENT_TYPE = "BoxCollider";

BoxCollider::BoxCollider(SceneNode * parent) : Collider(parent, GetEngine->GetMonoRuntime()->boxcollider_class)
{
	Entity * obj = (Entity*)parent->getAttachedObject(0);

	collisionShape = new btBoxShape(btVector3(1, 1, 1));
	SetCollisionShape(collisionShape);
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::SetBoxSize(Vector3 value)
{
	boxSize = value;

	delete collisionShape;
	collisionShape = new btBoxShape(btVector3(value.x, value.y, value.z));
	SetCollisionShape(collisionShape);
}
