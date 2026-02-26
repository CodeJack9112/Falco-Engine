#include "stdafx.h"
#include "RigidbodyEditor2.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/RigidBody.h"

#include "PropVector2.h"
#include "PropVector3.h"
#include "PropVector4.h"
#include "PropString.h"
#include "PropBool.h"
#include "PropBool3.h"
#include "PropInt.h"
#include "PropFloat.h"

RigidbodyEditor2::RigidbodyEditor2()
{
}

RigidbodyEditor2::~RigidbodyEditor2()
{
}

void RigidbodyEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(RigidBody::COMPONENT_TYPE);
	RigidBody* body = (RigidBody*)component;

	PropBool* enabled = new PropBool(this, "Enabled", body->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });
	
	PropBool* isStatic = new PropBool(this, "Static", body->GetIsStatic());
	isStatic->setOnChangeCallback([=](Property* prop, bool val) { onChangeStatic(prop, val); });

	PropBool* kinematic = new PropBool(this, "Kinematic", body->GetIsKinematic());
	kinematic->setOnChangeCallback([=](Property* prop, bool val) { onChangeKinematic(prop, val); });

	PropBool* useOwnGravity = new PropBool(this, "Use own gravity", body->GetUseOwnGravity());
	useOwnGravity->setOnChangeCallback([=](Property* prop, bool val) { onChangeUseOwnGravity(prop, val); });

	PropVector3* gravity = new PropVector3(this, "Gravity", body->GetGravity());
	gravity->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeGravity(prop, val); });

	PropFloat* friction = new PropFloat(this, "Friction", body->GetFriction());
	friction->setOnChangeCallback([=](Property* prop, float val) { onChangeFriction(prop, val); });

	PropFloat* bounciness = new PropFloat(this, "Bounciness", body->GetBounciness());
	bounciness->setOnChangeCallback([=](Property* prop, float val) { onChangeBounciness(prop, val); });

	PropFloat* linearDamping = new PropFloat(this, "Linear damping", body->GetLinearDamping());
	linearDamping->setOnChangeCallback([=](Property* prop, float val) { onChangeLinearDamping(prop, val); });

	PropFloat* angularDamping = new PropFloat(this, "Angular damping", body->GetAngularDamping());
	angularDamping->setOnChangeCallback([=](Property* prop, float val) { onChangeAngularDamping(prop, val); });

	PropFloat* mass = new PropFloat(this, "Mass", body->GetMass());
	mass->setOnChangeCallback([=](Property* prop, float val) { onChangeMass(prop, val); });

	bool freezePos[3], freezeRot[3];
	freezePos[0] = body->GetFreezePositionX();
	freezePos[1] = body->GetFreezePositionY();
	freezePos[2] = body->GetFreezePositionZ();

	freezeRot[0] = body->GetFreezeRotationX();
	freezeRot[1] = body->GetFreezeRotationY();
	freezeRot[2] = body->GetFreezeRotationZ();

	PropBool3* freezePosition = new PropBool3(this, "Freeze Position", "X", "Y", "Z", freezePos);
	PropBool3* freezeRotation = new PropBool3(this, "Freeze Rotation", "X", "Y", "Z", freezeRot);

	freezePosition->setOnChangeCallback([=](Property* prop, bool val[3]) { onChangeFreezePosition(prop, val); });
	freezeRotation->setOnChangeCallback([=](Property* prop, bool val[3]) { onChangeFreezeRotation(prop, val); });

	addProperty(enabled);
	addProperty(isStatic);
	addProperty(kinematic);
	addProperty(useOwnGravity);
	addProperty(gravity);
	addProperty(friction);
	addProperty(bounciness);
	addProperty(linearDamping);
	addProperty(angularDamping);
	addProperty(mass);
	addProperty(freezePosition);
	addProperty(freezeRotation);
}

void RigidbodyEditor2::onChangeEnabled(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		component->SetEnabled(val);
	}
}

void RigidbodyEditor2::onChangeMass(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetMass(val);
	}
}

void RigidbodyEditor2::onChangeFreezePosition(Property* prop, bool val[3])
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetFreezePositionX(val[0]);
		body->SetFreezePositionY(val[1]);
		body->SetFreezePositionZ(val[2]);
	}
}

void RigidbodyEditor2::onChangeFreezeRotation(Property* prop, bool val[3])
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetFreezeRotationX(val[0]);
		body->SetFreezeRotationY(val[1]);
		body->SetFreezeRotationZ(val[2]);
	}
}

void RigidbodyEditor2::onChangeKinematic(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetIsKinematic(val);
	}
}

void RigidbodyEditor2::onChangeStatic(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetIsStatic(val);
	}
}

void RigidbodyEditor2::onChangeUseOwnGravity(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetUseOwnGravity(val);
	}
}

void RigidbodyEditor2::onChangeGravity(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetGravity(val);
	}
}

void RigidbodyEditor2::onChangeFriction(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetFriction(val);
	}
}

void RigidbodyEditor2::onChangeBounciness(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetBounciness(val);
	}
}

void RigidbodyEditor2::onChangeLinearDamping(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetLinearDamping(val);
	}
}

void RigidbodyEditor2::onChangeAngularDamping(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		RigidBody* body = (RigidBody*)component;
		body->SetAngularDamping(val);
	}
}
