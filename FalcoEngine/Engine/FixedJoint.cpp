#include "stdafx.h"
#include "FixedJoint.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>
#include "Mathf.h"

std::string FixedJoint::COMPONENT_TYPE = "FixedJoint";

FixedJoint::FixedJoint(SceneNode* parent) : Component(parent)
{
}

FixedJoint::~FixedJoint()
{
	if (fixedConstraint != nullptr)
	{
		GetEngine->GetPhysicsManager()->GetWorld()->removeConstraint(fixedConstraint);
		//delete hingeConstraint;
	}
}

void FixedJoint::AutoConfigureAnchors()
{
	SceneNode* connectedNode = GetEngine->GetSceneManager()->getSceneNodeFast(connectedObjectName);
	if (connectedNode != nullptr)
	{
		Vector3 a1 = GetParentSceneNode()->_getDerivedScale() * (GetParentSceneNode()->_getDerivedOrientation() * anchor);
		connectedAnchor = Mathf::inverseTransformPoint(connectedNode, GetParentSceneNode()->_getDerivedPosition() + a1);
	}
}

void FixedJoint::SceneLoaded()
{
	if (!GetEnabled() || !GetParentSceneNode()->getVisible())
		return;

	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		RigidBody* body = (RigidBody*)GetParentSceneNode()->GetComponent(RigidBody::COMPONENT_TYPE);

		if (body != nullptr)
		{
			SceneNode* connectedNode = GetEngine->GetSceneManager()->getSceneNodeFast(connectedObjectName);
			if (connectedNode != nullptr)
			{
				RigidBody* connectedBody = (RigidBody*)connectedNode->GetComponent(RigidBody::COMPONENT_TYPE);

				if (connectedBody != nullptr)
				{
					btRigidBody* body1 = body->GetNativeBody();
					btRigidBody* body2 = connectedBody->GetNativeBody();

					if (body1 != nullptr && body2 != nullptr)
					{
						btTransform localA, localB;

						localA.setIdentity();
						localB.setIdentity();

						Quaternion r1 = GetParentSceneNode()->_getDerivedOrientation();
						Quaternion r2 = connectedNode->_getDerivedOrientation();

						Vector3 scl1 = GetParentSceneNode()->_getDerivedScale();
						Vector3 scl2 = connectedBody->GetParentSceneNode()->_getDerivedScale();

						Vector3 rAnchor = anchor * scl1;
						Vector3 rConnectedAnchor = connectedAnchor * scl2;

						localA.setOrigin(btVector3(rAnchor.x, rAnchor.y, rAnchor.z));
						localB.setOrigin(btVector3(rConnectedAnchor.x, rConnectedAnchor.y, rConnectedAnchor.z));

						localA.getBasis().setRotation(btQuaternion(r1.x, r1.y, r1.z, r1.w));
						localB.getBasis().setRotation(btQuaternion(r2.x, r2.y, r2.z, r2.w));

						localA = body->GetCenterOfMass().inverse() * localA;
						localB = connectedBody->GetCenterOfMass().inverse() * localB;

						if (fixedConstraint != nullptr)
							GetEngine->GetPhysicsManager()->GetWorld()->removeConstraint(fixedConstraint);

						fixedConstraint = new btFixedConstraint(*body1, *body2, localA, localB);

						GetEngine->GetPhysicsManager()->GetWorld()->addConstraint(fixedConstraint, !linkedBodiesCollision);
					}
				}
			}
		}
	}
}

void FixedJoint::StateChanged(bool active)
{
	RigidBody* body = (RigidBody*)GetParentSceneNode()->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		SceneNode* connectedNode = GetEngine->GetSceneManager()->getSceneNodeFast(connectedObjectName);
		if (connectedNode != nullptr)
		{
			RigidBody* connectedBody = (RigidBody*)connectedNode->GetComponent(RigidBody::COMPONENT_TYPE);

			if (connectedBody != nullptr)
			{
				if (body->IsInitialized() && connectedBody->IsInitialized())
				{
					if (active)
					{
						SceneLoaded();
					}
				}
			}
		}
	}

	if (!active)
	{
		if (fixedConstraint != nullptr)
			GetEngine->GetPhysicsManager()->GetWorld()->removeConstraint(fixedConstraint);
	}
}

void FixedJoint::NodeStateChanged(bool active)
{
	StateChanged(active);
}

void FixedJoint::SetConnectedObjectName(std::string name)
{
	connectedObjectName = name;
}