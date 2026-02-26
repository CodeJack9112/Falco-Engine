#include "stdafx.h"
#include "NavMeshObstacleEditor2.h"

#include <OgreSceneNode.h>

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/NavMeshObstacle.h"
#include "../Engine/Engine.h"
#include "../Engine/NavigationManager.h"

#include "PropBool.h"
#include "PropVector3.h"

#include "MainWindow.h"

NavMeshObstacleEditor2::NavMeshObstacleEditor2()
{
}

NavMeshObstacleEditor2::~NavMeshObstacleEditor2()
{
}

void NavMeshObstacleEditor2::init(std::vector<Ogre::SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(NavMeshObstacle::COMPONENT_TYPE);
	NavMeshObstacle* comp = (NavMeshObstacle*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropVector3* size = new PropVector3(this, "Size", comp->getSize());
	size->setOnChangeCallback([=](Property* prop, Ogre::Vector3 val) { onChangeSize(prop, val); });

	PropVector3* offset = new PropVector3(this, "Offset", comp->getOffset());
	offset->setOnChangeCallback([=](Property* prop, Ogre::Vector3 val) { onChangeOffset(prop, val); });

	addProperty(enabled);
	addProperty(size);
	addProperty(offset);
}

void NavMeshObstacleEditor2::onChangeEnabled(Property* prop, bool val)
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

	GetEngine->GetNavigationManager()->setNavMeshIsDirty();
}

void NavMeshObstacleEditor2::onChangeSize(Property* prop, Ogre::Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshObstacle* comp = (NavMeshObstacle*)component;
		comp->setSize(val);
	}

	MainWindow::gizmo2->showWireframe(true);

	GetEngine->GetNavigationManager()->setNavMeshIsDirty();
}

void NavMeshObstacleEditor2::onChangeOffset(Property* prop, Ogre::Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshObstacle* comp = (NavMeshObstacle*)component;
		comp->setOffset(val);
	}

	MainWindow::gizmo2->showWireframe(true);

	GetEngine->GetNavigationManager()->setNavMeshIsDirty();
}
