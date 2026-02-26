#include "stdafx.h"
#include "CharacterJointEditor2.h"

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/CharacterJoint.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropVector3.h"
#include "PropGameObject.h"
#include "PropString.h"
#include "PropButton.h"

#include "MainWindow.h"

CharacterJointEditor2::CharacterJointEditor2()
{
}

CharacterJointEditor2::~CharacterJointEditor2()
{
}

void CharacterJointEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(CharacterJoint::COMPONENT_TYPE);
	CharacterJoint* comp = (CharacterJoint*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	SceneNode* connectedObj = nullptr;
	if (!comp->GetConnectedObjectName().empty())
	{
		if (GetEngine->GetSceneManager()->hasSceneNode(comp->GetConnectedObjectName()))
		{
			connectedObj = GetEngine->GetSceneManager()->getSceneNode(comp->GetConnectedObjectName());
		}
	}

	PropGameObject* connectedObject = new PropGameObject(this, "Connected object", connectedObj);
	connectedObject->setOnChangeCallback([=](Property* prop, SceneNode* val) { onChangeConnectedObject(prop, val); });
	connectedObject->setSupportedFormats({ "::SceneNode" });
	connectedObject->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropConnectedBody(prop, from); });

	PropBool* linkedBodiesCollision = new PropBool(this, "Linked bodies collision", comp->GetLinkedBodiesCollision());
	linkedBodiesCollision->setOnChangeCallback([=](Property* prop, bool val) { onChangeLinkedBodiesCollision(prop, val); });

	PropVector3* anchor = new PropVector3(this, "Anchor", comp->GetAnchor());
	anchor->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeAnchor(prop, val); });

	PropVector3* connectedAnchor = new PropVector3(this, "Connected anchor", comp->GetConnectedAnchor());
	connectedAnchor->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeConnectedAnchor(prop, val); });

	PropButton* autoConfigure = new PropButton(this, "", "Auto");
	autoConfigure->setOnClickCallback([=](Property* prop) { onClickAutoConfigure(prop); });

	PropVector3* limitMin = new PropVector3(this, "Limit min", Vector3(Radian(comp->GetLimitMin().x).valueDegrees(), Radian(comp->GetLimitMin().y).valueDegrees(), Radian(comp->GetLimitMin().z).valueDegrees()));
	limitMin->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeLimitMin(prop, val); });

	PropVector3* limitMax = new PropVector3(this, "Limit max", Vector3(Radian(comp->GetLimitMax().x).valueDegrees(), Radian(comp->GetLimitMax().y).valueDegrees(), Radian(comp->GetLimitMax().z).valueDegrees()));
	limitMax->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeLimitMax(prop, val); });

	addProperty(enabled);
	addProperty(connectedObject);
	addProperty(linkedBodiesCollision);
	addProperty(anchor);
	addProperty(connectedAnchor);
	addProperty(autoConfigure);
	addProperty(limitMin);
	addProperty(limitMax);
}

void CharacterJointEditor2::onChangeEnabled(Property* prop, bool val)
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

void CharacterJointEditor2::onChangeConnectedObject(Property* prop, SceneNode* val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		if (val != nullptr)
			comp->SetConnectedObjectName(val->getName());
		else
			comp->SetConnectedObjectName("");
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onChangeLinkedBodiesCollision(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		comp->SetLinkedBodiesCollision(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onChangeAnchor(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		comp->SetAnchor(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onChangeConnectedAnchor(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		comp->SetConnectedAnchor(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onChangeLimitMin(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		comp->SetLimitMin(Vector3(Degree(val.x).valueRadians(), Degree(val.y).valueRadians(), Degree(val.z).valueRadians()));
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onChangeLimitMax(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		comp->SetLimitMax(Vector3(Degree(val.x).valueRadians(), Degree(val.y).valueRadians(), Degree(val.z).valueRadians()));
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onDropConnectedBody(TreeNode* prop, TreeNode* from)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(from->name);
	((PropGameObject*)prop)->setValue(node);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;

		comp->SetConnectedObjectName(from->name);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void CharacterJointEditor2::onClickAutoConfigure(Property* prop)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		CharacterJoint* comp = (CharacterJoint*)component;
		comp->AutoConfigureAnchors();
	}

	updateEditor();

	MainWindow::gizmo2->showWireframe(true);
}
