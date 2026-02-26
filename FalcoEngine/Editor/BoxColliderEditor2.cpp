#include "stdafx.h"
#include "BoxColliderEditor2.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/Mathf.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropVector3.h"
#include "MainWindow.h"

BoxColliderEditor2::BoxColliderEditor2()
{
}

BoxColliderEditor2::~BoxColliderEditor2()
{
}

void BoxColliderEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(BoxCollider::COMPONENT_TYPE);
	BoxCollider* comp = (BoxCollider*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropVector3* size = new PropVector3(this, "Size", comp->GetBoxSize());
	size->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeSize(prop, val); });

	PropVector3* offsetPosition = new PropVector3(this, "Offset Position", comp->GetOffset());
	offsetPosition->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeOffsetPosition(prop, val); });

	Vector3 eulerAngles = Mathf::toEuler(comp->GetRotation());
	eulerAngles = Vector3(Radian(eulerAngles.x).valueDegrees(), Radian(eulerAngles.y).valueDegrees(), Radian(eulerAngles.z).valueDegrees());

	PropVector3* offsetRotation = new PropVector3(this, "Offset Rotation", eulerAngles);
	offsetRotation->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeOffsetRotation(prop, val); });

	PropBool* isTrigger = new PropBool(this, "Is Trigger", comp->GetIsTrigger());
	isTrigger->setOnChangeCallback([=](Property* prop, bool val) { onChangeIsTrigger(prop, val); });

	addProperty(enabled);
	addProperty(isTrigger);
	addProperty(size);
	addProperty(offsetPosition);
	addProperty(offsetRotation);
}

void BoxColliderEditor2::onChangeEnabled(Property* prop, bool val)
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

void BoxColliderEditor2::onChangeSize(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		BoxCollider* comp = (BoxCollider*)component;
		comp->SetBoxSize(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void BoxColliderEditor2::onChangeOffsetPosition(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		BoxCollider* comp = (BoxCollider*)component;
		comp->SetOffset(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void BoxColliderEditor2::onChangeOffsetRotation(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		BoxCollider* comp = (BoxCollider*)component;
		Quaternion rotation = Mathf::toQuaternion(val);
		comp->SetRotation(rotation);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void BoxColliderEditor2::onChangeIsTrigger(Property* prop, bool val)
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
