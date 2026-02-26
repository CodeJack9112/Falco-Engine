#include "stdafx.h"
#include "CharacterJoint.h"
#include "RigidBody.h"
#include <OgreSceneNode.h>
#include "Mathf.h"

std::string CharacterJoint::COMPONENT_TYPE = "CharacterJoint";

CharacterJoint::CharacterJoint(SceneNode * parent) : Component(parent)
{
}

CharacterJoint::~CharacterJoint()
{
	if (characterConstraint != nullptr)
	{
		GetEngine->GetPhysicsManager()->GetWorld()->removeConstraint(characterConstraint);
		//delete characterConstraint;
	}
}

void CharacterJoint::AutoConfigureAnchors()
{
	SceneNode * connectedNode = GetEngine->GetSceneManager()->getSceneNodeFast(connectedObjectName);
	if (connectedNode != nullptr)
	{
		Vector3 a1 = GetParentSceneNode()->_getDerivedScale() * (GetParentSceneNode()->_getDerivedOrientation() * anchor);
		connectedAnchor = Mathf::inverseTransformPoint(connectedNode, GetParentSceneNode()->_getDerivedPosition() + a1);
	}
}

void CharacterJoint::SceneLoaded()
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

						Quaternion r1 = GetParentSceneNode()->_getDerivedOrientation().Inverse();
						Quaternion r2 = connectedNode->_getDerivedOrientation().Inverse();

						Vector3 scl1 = GetParentSceneNode()->_getDerivedScale();
						Vector3 scl2 = connectedBody->GetParentSceneNode()->_getDerivedScale();

						Vector3 rAnchor = anchor * scl1;
						Vector3 rConnectedAnchor = connectedAnchor * scl2;

						localA.getBasis().setRotation(btQuaternion(r1.x, r1.y, r1.z, r1.w));
						localB.getBasis().setRotation(btQuaternion(r2.x, r2.y, r2.z, r2.w));

						localA.setOrigin(btVector3(rAnchor.x, rAnchor.y, rAnchor.z));
						localB.setOrigin(btVector3(rConnectedAnchor.x, rConnectedAnchor.y, rConnectedAnchor.z));

						localA = body->GetCenterOfMass().inverse() * localA;
						localB = connectedBody->GetCenterOfMass().inverse() * localB;

						if (characterConstraint != nullptr)
							GetEngine->GetPhysicsManager()->GetWorld()->removeConstraint(characterConstraint);

						characterConstraint = new btGeneric6DofConstraint(*body1, *body2, localA, localB, true);

						characterConstraint->setAngularLowerLimit(btVector3(limitMin.x, limitMin.y, limitMin.z));
						characterConstraint->setAngularUpperLimit(btVector3(limitMax.x, limitMax.y, limitMax.z));

						GetEngine->GetPhysicsManager()->GetWorld()->addConstraint(characterConstraint, !linkedBodiesCollision);
					}
				}
			}
		}
	}
}

void CharacterJoint::StateChanged(bool active)
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
		if (characterConstraint != nullptr)
			GetEngine->GetPhysicsManager()->GetWorld()->removeConstraint(characterConstraint);
	}
}

void CharacterJoint::NodeStateChanged(bool active)
{
	StateChanged(active);
}

void CharacterJoint::SetConnectedObjectName(std::string name)
{
	connectedObjectName = name;
}