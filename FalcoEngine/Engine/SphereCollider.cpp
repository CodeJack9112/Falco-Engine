#include "stdafx.h"
#include "SphereCollider.h"
#include "RigidBody.h"

std::string SphereCollider::COMPONENT_TYPE = "SphereCollider";

SphereCollider::SphereCollider(SceneNode * parent) : Collider(parent, GetEngine->GetMonoRuntime()->spherecollider_class)
{
	Entity * obj = (Entity*)parent->getAttachedObject(0);

	collisionShape = new btSphereShape(radius);
	SetCollisionShape(collisionShape);
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::SetRadius(float value)
{
	radius = value;

	delete collisionShape;
	collisionShape = new btSphereShape(radius);
	SetCollisionShape(collisionShape);
}
