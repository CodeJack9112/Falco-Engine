#include "stdafx.h"
#include "MeshColliderEditor2.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/MeshCollider.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "MainWindow.h"

MeshColliderEditor2::MeshColliderEditor2()
{
}

MeshColliderEditor2::~MeshColliderEditor2()
{
}

void MeshColliderEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(MeshCollider::COMPONENT_TYPE);
	MeshCollider* comp = (MeshCollider*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropBool* convex = new PropBool(this, "Convex", comp->GetConvex());
	convex->setOnChangeCallback([=](Property* prop, bool val) { onChangeConvex(prop, val); });

	PropBool* isTrigger = new PropBool(this, "Is Trigger", comp->GetIsTrigger());
	isTrigger->setOnChangeCallback([=](Property* prop, bool val) { onChangeIsTrigger(prop, val); });

	addProperty(enabled);
	addProperty(isTrigger);
	addProperty(convex);
}

void MeshColliderEditor2::onChangeEnabled(Property* prop, bool val)
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

void MeshColliderEditor2::onChangeConvex(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		MeshCollider* comp = (MeshCollider*)component;
		comp->SetConvex(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void MeshColliderEditor2::onChangeIsTrigger(Property* prop, bool val)
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