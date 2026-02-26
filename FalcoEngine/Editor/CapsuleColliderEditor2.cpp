#include "stdafx.h"
#include "CapsuleColliderEditor2.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/Mathf.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropVector3.h"
#include "MainWindow.h"

CapsuleColliderEditor2::CapsuleColliderEditor2()
{
}

CapsuleColliderEditor2::~CapsuleColliderEditor2()
{
}

void CapsuleColliderEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(CapsuleCollider::COMPONENT_TYPE);
	CapsuleCollider* comp = (CapsuleCollider*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropFloat* radius = new PropFloat(this, "Radius", comp->GetRadius());
	radius->setOnChangeCallback([=](Property* prop, float val) { onChangeRadius(prop, val); });

	PropFloat* height = new PropFloat(this, "Height", comp->GetHeight());
	height->setOnChangeCallback([=](Property* prop, float val) { onChangeHeight(prop, val); });

	PropVector3* offsetPosition = new PropVector3(this, "Offset Position", comp->GetOffset());
	offsetPosition->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeOffsetPosition(prop, val); });

	Vector3 eulerAngles = Mathf::toEuler(comp->GetRotation());

	PropVector3* offsetRotation = new PropVector3(this, "Offset Rotation", eulerAngles);
	offsetRotation->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeOffsetRotation(prop, val); });

	PropBool* isTrigger = new PropBool(this, "Is Trigger", comp->GetIsTrigger());
	isTrigger->setOnChangeCallback([=](Property* prop, bool val) { onChangeIsTrigger(prop, val); });

	addProperty(enabled);
	addProperty(isTrigger);
	addProperty(radius);
	addProperty(height);
	addProperty(offsetPosition);
	addProperty(offsetRotation);
}

void CapsuleColliderEditor2::onChangeEnabled(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		component->SetEnabled(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CapsuleColliderEditor2::onChangeRadius(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CapsuleCollider* comp = (CapsuleCollider*)component;
		comp->SetRadius(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CapsuleColliderEditor2::onChangeHeight(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CapsuleCollider* comp = (CapsuleCollider*)component;
		comp->SetHeight(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CapsuleColliderEditor2::onChangeOffsetPosition(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CapsuleCollider* comp = (CapsuleCollider*)component;
		comp->SetOffset(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CapsuleColliderEditor2::onChangeOffsetRotation(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CapsuleCollider* comp = (CapsuleCollider*)component;
		Quaternion rotation = Mathf::toQuaternion(val);
		comp->SetRotation(rotation);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CapsuleColliderEditor2::onChangeIsTrigger(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Collider* comp = (Collider*)component;
		comp->SetIsTrigger(val);
	}
}