#include "stdafx.h"
#include "NavMeshAgentEditor2.h"

#include <OgreSceneNode.h>

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/NavMeshAgent.h"

#include "PropBool.h"
#include "PropFloat.h"

NavMeshAgentEditor2::NavMeshAgentEditor2()
{
}

NavMeshAgentEditor2::~NavMeshAgentEditor2()
{
}

void NavMeshAgentEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(NavMeshAgent::COMPONENT_TYPE);
	NavMeshAgent* comp = (NavMeshAgent*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropFloat* radius = new PropFloat(this, "Radius", comp->GetRadius());
	radius->setOnChangeCallback([=](Property* prop, float val) { onChangeRadius(prop, val); });

	PropFloat* height = new PropFloat(this, "Height", comp->GetHeight());
	height->setOnChangeCallback([=](Property* prop, float val) { onChangeHeight(prop, val); });

	PropFloat* speed = new PropFloat(this, "Speed", comp->GetSpeed());
	speed->setOnChangeCallback([=](Property* prop, float val) { onChangeSpeed(prop, val); });

	PropFloat* rotationSpeed = new PropFloat(this, "Rotation speed", comp->GetRotationSpeed());
	rotationSpeed->setOnChangeCallback([=](Property* prop, float val) { onChangeRotationSpeed(prop, val); });

	PropFloat* acceleration = new PropFloat(this, "Acceleration", comp->GetAcceleration());
	acceleration->setOnChangeCallback([=](Property* prop, float val) { onChangeAcceleration(prop, val); });

	addProperty(enabled);
	addProperty(radius);
	addProperty(height);
	addProperty(speed);
	addProperty(rotationSpeed);
	addProperty(acceleration);
}

void NavMeshAgentEditor2::onChangeEnabled(Property* prop, bool val)
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

void NavMeshAgentEditor2::onChangeRadius(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshAgent* comp = (NavMeshAgent*)component;
		comp->SetRadius(val);
	}
}

void NavMeshAgentEditor2::onChangeHeight(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshAgent* comp = (NavMeshAgent*)component;
		comp->SetHeight(val);
	}
}

void NavMeshAgentEditor2::onChangeSpeed(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshAgent* comp = (NavMeshAgent*)component;
		comp->SetSpeed(val);
	}
}

void NavMeshAgentEditor2::onChangeAcceleration(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshAgent* comp = (NavMeshAgent*)component;
		comp->SetAcceleration(val);
	}
}

void NavMeshAgentEditor2::onChangeRotationSpeed(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		NavMeshAgent* comp = (NavMeshAgent*)component;
		comp->SetRotationSpeed(val);
	}
}
