#include "Collider.h"
#include "RigidBody.h"

Collider::~Collider()
{
	if (collisionShape != nullptr) delete collisionShape;

	if (!globalDelete)
		reloadBody();
}

void Collider::reloadBody()
{
	RigidBody* body = (RigidBody*)GetParentSceneNode()->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		if (body->IsInitialized())
		{
			if (body->GetEnabled())
			{
				body->SetupPhysics(false);
			}
		}
	}
}

void Collider::SetCollisionShape(btCollisionShape* shape)
{
	collisionShape = shape;

	reloadBody();
}

void Collider::SetOffset(Vector3 value)
{
	offset = value;

	reloadBody();
}

void Collider::SetRotation(Quaternion value)
{
	rotation = value;

	reloadBody();
}

void Collider::SetIsTrigger(bool value)
{
	isTrigger = value;

	reloadBody();
}

void Collider::StateChanged(bool active)
{
	RigidBody* body = (RigidBody*)GetParentSceneNode()->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		if (body->IsInitialized())
		{
			if (body->GetEnabled())
			{
				body->StateChanged(false);
				body->StateChanged(true);
			}
		}
	}
}

void Collider::NodeStateChanged(bool active)
{
	//StateChanged(active);
}
