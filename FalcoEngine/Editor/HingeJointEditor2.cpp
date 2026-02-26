#include "stdafx.h"
#include "HingeJointEditor2.h"

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/HingeJoint.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropVector3.h"
#include "PropVector2.h"
#include "PropGameObject.h"
#include "PropString.h"
#include "PropButton.h"

#include "MainWindow.h"

HingeJointEditor2::HingeJointEditor2()
{
}

HingeJointEditor2::~HingeJointEditor2()
{
}

void HingeJointEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(HingeJoint::COMPONENT_TYPE);
	HingeJoint* comp = (HingeJoint*)component;

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
	connectedObject->setOnChangeCallback([=](Property* prop, SceneNode * val) { onChangeConnectedObject(prop, val); });
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

	PropVector3* axis = new PropVector3(this, "Axis", comp->GetAxis());
	axis->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeAxis(prop, val); });

	PropVector2* limits = new PropVector2(this, "Limits", Vector2(Radian(comp->GetLimitMin()).valueDegrees(), Radian(comp->GetLimitMax()).valueDegrees()));
	limits->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeLimits(prop, val); });

	addProperty(enabled);
	addProperty(connectedObject);
	addProperty(linkedBodiesCollision);
	addProperty(anchor);
	addProperty(connectedAnchor);
	addProperty(autoConfigure);
	addProperty(axis);
	addProperty(limits);
}

void HingeJointEditor2::onChangeEnabled(Property* prop, bool val)
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

void HingeJointEditor2::onChangeConnectedObject(Property* prop, SceneNode* val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		if (val != nullptr)
			comp->SetConnectedObjectName(val->getName());
		else
			comp->SetConnectedObjectName("");
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onChangeLinkedBodiesCollision(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		comp->SetLinkedBodiesCollision(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onChangeAnchor(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		comp->SetAnchor(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onChangeConnectedAnchor(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		comp->SetConnectedAnchor(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onChangeAxis(Property* prop, Vector3 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		comp->SetAxis(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onChangeLimits(Property* prop, Vector2 val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		comp->SetLimitMin(Degree(val.x).valueRadians());
		comp->SetLimitMax(Degree(val.y).valueRadians());
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onDropConnectedBody(TreeNode* prop, TreeNode* from)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(from->name);
	((PropGameObject*)prop)->setValue(node);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;

		comp->SetConnectedObjectName(from->name);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void HingeJointEditor2::onClickAutoConfigure(Property* prop)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		HingeJoint* comp = (HingeJoint*)component;
		comp->AutoConfigureAnchors();
	}

	updateEditor();

	MainWindow::gizmo2->showWireframe(true);
}
