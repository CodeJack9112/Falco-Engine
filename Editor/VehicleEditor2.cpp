#include "stdafx.h"
#include "VehicleEditor2.h"

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/Vehicle.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropVector3.h"
#include "PropVector2.h"
#include "PropGameObject.h"
#include "PropString.h"
#include "PropButton.h"
#include "PropComboBox.h"

#include "MainWindow.h"

VehicleEditor2::VehicleEditor2()
{
}

VehicleEditor2::~VehicleEditor2()
{
}

void VehicleEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(Vehicle::COMPONENT_TYPE);
	Vehicle* comp = (Vehicle*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropBool* invertForward = new PropBool(this, "Invert forward", comp->getInvertForward());
	invertForward->setOnChangeCallback([=](Property* prop, bool val) { onChangeInvertForward(prop, val); });

	PropComboBox* axis = new PropComboBox(this, "Axis", { "XYZ", "XZY", "ZXY", "ZYX", "YXZ", "YZX" });
	
	if (comp->getAxis() == Vector3(0, 1, 2)) axis->setCurrentItem(0);
	if (comp->getAxis() == Vector3(0, 2, 1)) axis->setCurrentItem(1);
	if (comp->getAxis() == Vector3(2, 0, 1)) axis->setCurrentItem(2);
	if (comp->getAxis() == Vector3(2, 1, 0)) axis->setCurrentItem(3);
	if (comp->getAxis() == Vector3(1, 0, 2)) axis->setCurrentItem(4);
	if (comp->getAxis() == Vector3(1, 2, 0)) axis->setCurrentItem(5);

	axis->setOnChangeCallback([=](Property* prop, string val) { onChangeAxis(prop, val); });

	Property* wheelsGroup = new Property(this, "Wheels");
	wheelsGroup->setHasButton(true);
	wheelsGroup->setButtonText("+");
	wheelsGroup->setOnButtonClickCallback([=](TreeNode* prop) { onClickAddWheel(prop); });

	std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();

	for (auto it = wheels.begin(); it != wheels.end(); ++it)
	{
		Vehicle::WheelInfo inf = *it;

		SceneNode* connectedObj = nullptr;
		if (!inf.m_connectedObjectName.empty())
		{
			if (GetEngine->GetSceneManager()->hasSceneNode(inf.m_connectedObjectName))
			{
				connectedObj = GetEngine->GetSceneManager()->getSceneNode(inf.m_connectedObjectName);
			}
		}

		int index = std::distance(wheels.begin(), it);

		Property* wheel = new Property(this, "Wheel " + to_string(index + 1));
		wheel->setUserData(static_cast<void*>(new int(index)));
		wheel->setPopupMenu({ "Remove" }, [=](TreeNode* node, int val) { onClickWheelPopup(node, val); });

		PropGameObject* connectedObject = new PropGameObject(this, "Connected object", connectedObj);
		connectedObject->setSupportedFormats({ "::SceneNode" });
		connectedObject->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropConnectedBody(prop, from); });

		PropBool* isFrontWheel = new PropBool(this, "Is Front Wheel", inf.m_isFrontWheel);
		isFrontWheel->setOnChangeCallback([=](Property* prop, bool val) { onChangeIsFrontWheel(prop, val); });

		PropVector3* connectionPoint = new PropVector3(this, "Connection Point", inf.m_connectionPoint);
		connectionPoint->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeConnectionPoint(prop, val); });

		PropButton* autoConfigure = new PropButton(this, "", "Auto");
		autoConfigure->setOnClickCallback([=](Property* prop) { onClickAutoConfigure(prop); });

		PropFloat* radius = new PropFloat(this, "Radius", inf.m_radius);
		radius->setOnChangeCallback([=](Property* prop, float val) { onChangeRadius(prop, val); });

		PropFloat* width = new PropFloat(this, "Width", inf.m_width);
		width->setOnChangeCallback([=](Property* prop, float val) { onChangeWidth(prop, val); });

		PropFloat* suspensionStiffness = new PropFloat(this, "Suspension Stiffness", inf.m_suspensionStiffness);
		suspensionStiffness->setOnChangeCallback([=](Property* prop, float val) { onChangeSuspensionStiffness(prop, val); });

		PropFloat* suspensionDamping = new PropFloat(this, "Suspension Damping", inf.m_suspensionDamping);
		suspensionDamping->setOnChangeCallback([=](Property* prop, float val) { onChangeSuspensionDamping(prop, val); });

		PropFloat* suspensionCompression = new PropFloat(this, "Suspension Compression", inf.m_suspensionCompression);
		suspensionCompression->setOnChangeCallback([=](Property* prop, float val) { onChangeSuspensionCompression(prop, val); });

		PropFloat* suspensionRestLength = new PropFloat(this, "Suspension Rest Length", inf.m_suspensionRestLength);
		suspensionRestLength->setOnChangeCallback([=](Property* prop, float val) { onChangeSuspensionRestLength(prop, val); });

		PropFloat* friction = new PropFloat(this, "Friction", inf.m_friction);
		friction->setOnChangeCallback([=](Property* prop, float val) { onChangeFriction(prop, val); });

		PropFloat* rollInfluence = new PropFloat(this, "Roll Influence", inf.m_rollInfluence);
		rollInfluence->setOnChangeCallback([=](Property* prop, float val) { onChangeRollInfluence(prop, val); });

		PropVector3* direction = new PropVector3(this, "Direction", inf.m_direction);
		direction->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeDirection(prop, val); });

		PropVector3* axle = new PropVector3(this, "Axle", inf.m_axle);
		axle->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeAxle(prop, val); });

		wheel->addChild(connectedObject);
		wheel->addChild(isFrontWheel);
		wheel->addChild(connectionPoint);
		wheel->addChild(autoConfigure);
		wheel->addChild(radius);
		wheel->addChild(width);
		wheel->addChild(suspensionStiffness);
		wheel->addChild(suspensionDamping);
		wheel->addChild(suspensionCompression);
		wheel->addChild(suspensionRestLength);
		wheel->addChild(friction);
		wheel->addChild(rollInfluence);
		wheel->addChild(direction);
		wheel->addChild(axle);

		wheelsGroup->addChild(wheel);
	}

	addProperty(enabled);
	addProperty(invertForward);
	addProperty(axis);
	addProperty(wheelsGroup);
}

void VehicleEditor2::onChangeEnabled(Property* prop, bool val)
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

void VehicleEditor2::onChangeAxis(Property* prop, string val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Vehicle* component = (Vehicle*)(*it)->components.at(index);

		//"XYZ", "XZY", "ZXY", "ZYX", "YXZ", "YZX"

		if (val == "XYZ") component->setAxis(Vector3(0, 1, 2));
		if (val == "XZY") component->setAxis(Vector3(0, 2, 1));
		if (val == "ZXY") component->setAxis(Vector3(2, 0, 1));
		if (val == "ZYX") component->setAxis(Vector3(2, 1, 0));
		if (val == "YXZ") component->setAxis(Vector3(1, 0, 2));
		if (val == "YZX") component->setAxis(Vector3(1, 2, 0));
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeInvertForward(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Vehicle* component = (Vehicle*)(*it)->components.at(index);

		component->setInvertForward(val);
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onDropConnectedBody(TreeNode* prop, TreeNode* from)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(from->name);
	((PropGameObject*)prop)->setValue(node);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_connectedObjectName = from->name;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onClickAutoConfigure(Property* prop)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;
		comp->AutoConfigureAnchors(wheelIndex);
	}

	updateEditor();

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeRadius(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;
		
		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_radius = value;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeWidth(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_width = value;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeSuspensionStiffness(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_suspensionStiffness = value;
	}
}

void VehicleEditor2::onChangeSuspensionDamping(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_suspensionDamping = value;
	}
}

void VehicleEditor2::onChangeSuspensionCompression(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_suspensionCompression = value;
	}
}

void VehicleEditor2::onChangeSuspensionRestLength(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_suspensionRestLength = value;
	}
}

void VehicleEditor2::onChangeFriction(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_friction = value;
	}
}

void VehicleEditor2::onChangeRollInfluence(TreeNode* prop, float value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_rollInfluence = value;
	}
}

void VehicleEditor2::onChangeDirection(TreeNode* prop, Vector3 value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_direction = value;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeAxle(TreeNode* prop, Vector3 value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_axle = value;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeConnectionPoint(TreeNode* prop, Vector3 value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_connectionPoint = value;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onChangeIsFrontWheel(TreeNode* prop, bool value)
{
	void* data = prop->parent->parent->parent->getUserData();
	int index = *static_cast<int*>(data);

	data = prop->parent->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		Vehicle::WheelInfo& inf = wheels.at(wheelIndex);

		inf.m_isFrontWheel = value;
	}

	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onClickAddWheel(TreeNode* prop)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		Vehicle* comp = (Vehicle*)component;

		std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
		wheels.push_back(Vehicle::WheelInfo());
	}

	updateEditor();
	MainWindow::gizmo2->showWireframe(true);
}

void VehicleEditor2::onClickWheelPopup(TreeNode* prop, int index)
{
	void* data = prop->parent->parent->getUserData();
	int _index = *static_cast<int*>(data);

	data = prop->getUserData();
	int wheelIndex = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	if (index == 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			Component* component = (*it)->components.at(_index);
			Vehicle* comp = (Vehicle*)component;

			std::vector<Vehicle::WheelInfo> & wheels = comp->getWheels();
			wheels.erase(wheels.begin() + wheelIndex);
		}
	}

	updateEditor();
	MainWindow::gizmo2->showWireframe(true);
}
