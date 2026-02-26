#include "stdafx.h"
#include "CapsuleCollider.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>

std::string CapsuleCollider::COMPONENT_TYPE = "CapsuleCollider";

CapsuleCollider::CapsuleCollider(SceneNode * parent) : Collider(parent, GetEngine->GetMonoRuntime()->capsulecollider_class)
{
	Entity * obj = (Entity*)parent->getAttachedObject(0);

	collisionShape = new btCapsuleShape(radius, height);
	SetCollisionShape(collisionShape);
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::SetRadius(float r)
{
	radius = r;

	delete collisionShape;
	collisionShape = new btCapsuleShape(radius, height);
	SetCollisionShape(collisionShape);
}

void CapsuleCollider::SetHeight(float h)
{
	height = h;

	delete collisionShape;
	collisionShape = new btCapsuleShape(radius, height);
	SetCollisionShape(collisionShape);
}
