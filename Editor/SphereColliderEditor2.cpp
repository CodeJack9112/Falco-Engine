#include "stdafx.h"
#include "SphereColliderEditor2.h"

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/SphereCollider.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropVector3.h"
#include "MainWindow.h"

SphereColliderEditor2::SphereColliderEditor2()
{
}

SphereColliderEditor2::~SphereColliderEditor2()
{
}

void SphereColliderEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(SphereCollider::COMPONENT_TYPE);
	SphereCollider* comp = (SphereCollider*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropFloat* radius = new PropFloat(this, "Radius", comp->GetRadius());
	radius->setOnChangeCallback([=](Property* prop, float val) { onChangeRadius(prop, val); });

	PropVector3* offsetPosition = new PropVector3(this, "Offset Position", comp->GetOffset());
	offsetPosition->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeOffsetPosition(prop, val); });

	PropBool* isTrigger = new PropBool(this, "Is Trigger", comp->GetIsTrigger());
	isTrigger->setOnChangeCallback([=](Property* prop, bool val) { onChangeIsTrigger(prop, val); });

	addProperty(enabled);
	addProperty(isTrigger);
	addProperty(radius);
	addProperty(offsetPosition);
}

void SphereColliderEditor2::onChangeEnabled(Property* prop, bool val)
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

void SphereColliderEditor2::onChangeRadius(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		SphereCollider* comp = (SphereCollider*)component;
		comp->SetRadius(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void SphereColliderEditor2::onChangeOffsetPosition(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		SphereCollider* comp = (SphereCollider*)component;
		comp->SetOffset(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void SphereColliderEditor2::onChangeIsTrigger(Property* prop, bool val)
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