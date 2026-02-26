#include "stdafx.h"
#include "UIButtonEditor2.h"

#include <OgreSceneNode.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "MainWindow.h"

#include "PropBool.h"
#include "PropFloat.h"
#include "PropString.h"
#include "PropComboBox.h"
#include "PropColorPicker.h"

#include "../Engine/Engine.h"

UIButtonEditor2::UIButtonEditor2()
{
	setEditorName("UIButtonEditor");
}

UIButtonEditor2::~UIButtonEditor2()
{
}

void UIButtonEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Property* group = new Property(this, "UI Button");
	group->icon = TextureManager::getSingleton().load("Icons/Hierarchy/button.png", "Editor");

	Property* states = new Property(this, "Button states");

	auto sceneNodes = getSceneNodes();
	SceneNode* sceneNode = sceneNodes[0];
	UIElement* uiElement = (UIElement*)sceneNode->getAttachedObject(0);

	string imageName = ((UIButton*)uiElement)->GetImageNormalName();
	PropString * normalImage = new PropString(this, "Normal image",imageName);
	normalImage->setReadonly(true);
	normalImage->setSupportedFormats(MainWindow::GetImagesFileFormats());
	normalImage->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropNormalImage(prop, from); });

	PropColorPicker* colorNormal = new PropColorPicker(this, "Normal color", ((UIButton*)uiElement)->GetColorNormal());
	colorNormal->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeColorNormal(prop, val); });

	imageName = ((UIButton*)uiElement)->GetImageHoverName();
	PropString* hoverImage = new PropString(this, "Hover image", imageName);
	hoverImage->setReadonly(true);
	hoverImage->setSupportedFormats(MainWindow::GetImagesFileFormats());
	hoverImage->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropHoverImage(prop, from); });

	PropColorPicker* colorHover = new PropColorPicker(this, "Hover color", ((UIButton*)uiElement)->GetColorHover());
	colorHover->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeColorHover(prop, val); });

	imageName = ((UIButton*)uiElement)->GetImagePressedName();
	PropString * pressedImage = new PropString(this, "Pressed image", imageName);
	pressedImage->setReadonly(true);
	pressedImage->setSupportedFormats(MainWindow::GetImagesFileFormats());
	pressedImage->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropPressedImage(prop, from); });

	PropColorPicker* colorPressed = new PropColorPicker(this, "Pressed color", ((UIButton*)uiElement)->GetColorPressed());
	colorPressed->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeColorPressed(prop, val); });

	imageName = ((UIButton*)uiElement)->GetImageDisabledName();
	PropString * disabledImage = new PropString(this, "Disabled image", imageName);
	disabledImage->setReadonly(true);
	disabledImage->setSupportedFormats(MainWindow::GetImagesFileFormats());
	disabledImage->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropDisabledImage(prop, from); });

	PropColorPicker* colorDisabled = new PropColorPicker(this, "Disabled color", ((UIButton*)uiElement)->GetColorDisabled());
	colorDisabled->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeColorDisabled(prop, val); });

	PropBool* interactable = new PropBool(this, "Interactable", ((UIButton*)uiElement)->GetInteractable());
	interactable->setOnChangeCallback([=](Property* prop, bool val) { onChangeInteractable(prop, val); });

	states->addChild(normalImage);
	states->addChild(colorNormal);
	states->addChild(hoverImage);
	states->addChild(colorHover);
	states->addChild(pressedImage);
	states->addChild(colorPressed);
	states->addChild(disabledImage);
	states->addChild(colorDisabled);
	states->addChild(interactable);

	SceneNode* objNodeDown = nullptr;
	if (GetEngine->GetSceneManager()->hasSceneNode(((UIButton*)uiElement)->GetPointerDownEvent().sceneNode))
		objNodeDown = GetEngine->GetSceneManager()->getSceneNode(((UIButton*)uiElement)->GetPointerDownEvent().sceneNode);

	SceneNode* objNodeUp = nullptr;
	if (GetEngine->GetSceneManager()->hasSceneNode(((UIButton*)uiElement)->GetPointerUpEvent().sceneNode))
		objNodeUp = GetEngine->GetSceneManager()->getSceneNode(((UIButton*)uiElement)->GetPointerUpEvent().sceneNode);

	MonoRuntime::FunctionList funcsDown = GetEngine->GetMonoRuntime()->GetFunctions(objNodeDown);
	MonoRuntime::FunctionList funcsUp = GetEngine->GetMonoRuntime()->GetFunctions(objNodeUp);

	std::vector<string> downEvents = { "None" };
	std::vector<string> upEvents = { "None" };

	for (MonoRuntime::FunctionList::iterator it = funcsDown.begin(); it != funcsDown.end(); ++it)
	{
		downEvents.push_back(it->first + "." + it->second);
	}

	for (MonoRuntime::FunctionList::iterator it = funcsUp.begin(); it != funcsUp.end(); ++it)
	{
		upEvents.push_back(it->first + "." + it->second);
	}

	Property* events = new Property(this, "Events");
	Property* pointerDownEventGroup = new Property(this, "Pointer Down");
	PropString * pointerDownEventNode = new PropString(this, "Object", ((UIButton*)uiElement)->GetPointerDownEvent().sceneNode);
	PropComboBox* pointerDownEventFunc = new PropComboBox(this, "Function", downEvents);
	pointerDownEventFunc->setCurrentItem(((UIButton*)uiElement)->GetPointerDownEvent().functionName);
	pointerDownEventFunc->setOnChangeCallback([=](Property* prop, string val) { onChangePointerDownEventFunc(prop, val); });

	pointerDownEventNode->setPopupMenu({ "Clear" }, [=](TreeNode* node, int val) { onEventObjectDownPopup(node, val); });

	Property* pointerUpEventGroup = new Property(this, "Pointer Up");
	PropString* pointerUpEventNode = new PropString(this, "Object", ((UIButton*)uiElement)->GetPointerUpEvent().sceneNode);
	PropComboBox* pointerUpEventFunc = new PropComboBox(this, "Function", upEvents);
	pointerUpEventFunc->setCurrentItem(((UIButton*)uiElement)->GetPointerUpEvent().functionName);
	pointerUpEventFunc->setOnChangeCallback([=](Property* prop, string val) { onChangePointerUpEventFunc(prop, val); });

	pointerUpEventNode->setPopupMenu({ "Clear" }, [=](TreeNode* node, int val) { onEventObjectUpPopup(node, val); });

	pointerDownEventNode->setReadonly(true);
	pointerUpEventNode->setReadonly(true);

	std::vector<std::string> formats;
	formats.push_back("::SceneNode");
	pointerDownEventNode->setSupportedFormats(formats);
	pointerUpEventNode->setSupportedFormats(formats);

	pointerDownEventNode->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropObjectDownEvent(prop, from); });
	pointerUpEventNode->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropObjectUpEvent(prop, from); });

	pointerDownEventGroup->addChild(pointerDownEventNode);
	pointerDownEventGroup->addChild(pointerDownEventFunc);

	pointerUpEventGroup->addChild(pointerUpEventNode);
	pointerUpEventGroup->addChild(pointerUpEventFunc);

	events->addChild(pointerDownEventGroup);
	events->addChild(pointerUpEventGroup);

	group->addChild(states);
	group->addChild(events);

	addProperty(group);
}

void UIButtonEditor2::update()
{
	__super::update();
}

void UIButtonEditor2::onDropNormalImage(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetImageNormal(fullPath);
		((UIButton*)uiElement)->SetImage(fullPath);
	}
}

void UIButtonEditor2::onDropHoverImage(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetImageHover(fullPath);
	}
}

void UIButtonEditor2::onDropPressedImage(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetImagePressed(fullPath);
	}
}

void UIButtonEditor2::onDropDisabledImage(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetImageDisabled(fullPath);
	}
}

void UIButtonEditor2::onDropObjectDownEvent(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->alias;
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		UIEvent evt = ((UIButton*)uiElement)->GetPointerDownEvent();
		evt.sceneNode = from->name;

		((UIButton*)uiElement)->SetPointerDownEvent(evt);
	}

	updateEditor();
}

void UIButtonEditor2::onDropObjectUpEvent(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->alias;
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		UIEvent evt = ((UIButton*)uiElement)->GetPointerUpEvent();
		evt.sceneNode = from->name;

		((UIButton*)uiElement)->SetPointerUpEvent(evt);
	}

	updateEditor();
}

void UIButtonEditor2::onChangePointerDownEventFunc(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);

		UIEvent evt = ((UIButton*)uiElement)->GetPointerDownEvent();
		evt.functionName = val;

		((UIButton*)uiElement)->SetPointerDownEvent(evt);
	}
}

void UIButtonEditor2::onChangePointerUpEventFunc(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);

		UIEvent evt = ((UIButton*)uiElement)->GetPointerUpEvent();
		evt.functionName = val;

		((UIButton*)uiElement)->SetPointerUpEvent(evt);
	}
}

void UIButtonEditor2::onEventObjectDownPopup(TreeNode* node, int val)
{
	((PropString*)node)->setValue("None");

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		UIEvent evt = ((UIButton*)uiElement)->GetPointerDownEvent();
		evt.sceneNode = "None";
		evt.functionName = "None";

		((UIButton*)uiElement)->SetPointerDownEvent(evt);
	}

	updateEditor();
}

void UIButtonEditor2::onEventObjectUpPopup(TreeNode* node, int val)
{
	((PropString*)node)->setValue("None");

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		UIEvent evt = ((UIButton*)uiElement)->GetPointerUpEvent();
		evt.sceneNode = "None";
		evt.functionName = "None";

		((UIButton*)uiElement)->SetPointerUpEvent(evt);
	}

	updateEditor();
}

void UIButtonEditor2::onChangeColorNormal(Property* prop, ColourValue value)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetColorNormal(value);
		((UIButton*)uiElement)->SetColor(value);
	}
}

void UIButtonEditor2::onChangeColorHover(Property* prop, ColourValue value)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetColorHover(value);
	}
}

void UIButtonEditor2::onChangeColorPressed(Property* prop, ColourValue value)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetColorPressed(value);
	}
}

void UIButtonEditor2::onChangeColorDisabled(Property* prop, ColourValue value)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetColorDisabled(value);
	}
}

void UIButtonEditor2::onChangeInteractable(Property* prop, bool value)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIButton*)uiElement)->SetInteractable(value);
	}
}
