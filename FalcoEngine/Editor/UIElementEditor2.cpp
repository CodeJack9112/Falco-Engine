#include "stdafx.h"
#include "UIElementEditor2.h"

#include <OgreSceneNode.h>
#include "../Engine/UIElement.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "PropVector2.h"
#include "PropColorPicker.h"
#include "PropString.h"
#include "PropComboBox.h"

UIElementEditor2::UIElementEditor2()
{
	setEditorName("UIElementEditor");
}

UIElementEditor2::~UIElementEditor2()
{
}

void UIElementEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Property* group = new Property(this, "UI Element");

	auto sceneNodes = getSceneNodes();

	SceneNode* sceneNode = sceneNodes[0];
	UIElement* uiElement = (UIElement*)sceneNode->getAttachedObject(0);

	PropColorPicker* color = new PropColorPicker(this, "Color tint", uiElement->getColor());
	color->setOnChangeCallback([=](Property * prop, ColourValue val) { onChangeColor(prop, val); });

	PropComboBox* canvasAlignment = new PropComboBox(this, "Canvas alignment", { "TopLeft", "MiddleLeft", "BottomLeft", "TopRight", "MiddleRight", "BottomRight", "TopMiddle", "BottomMiddle", "Center" });
	canvasAlignment->setCurrentItem(uiElement->GetCanvasAlignmentString());
	canvasAlignment->setOnChangeCallback([=](Property* prop, string val) { onChangeAlignment(prop, val); });

	PropVector2* size = new PropVector2(this, "Size", uiElement->GetSize());
	size->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeSize(prop, val); });

	PropVector2* anchor = new PropVector2(this, "Anchor", uiElement->GetAnchor());
	anchor->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeAnchor(prop, val); });

	group->addChild(color);
	group->addChild(canvasAlignment);
	group->addChild(size);
	group->addChild(anchor);

	addProperty(group);
}

void UIElementEditor2::update()
{
	__super::update();
}

void UIElementEditor2::onChangeColor(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		uiElement->setColor(val);
	}
}

void UIElementEditor2::onChangeAlignment(Property* prop, string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		uiElement->SetCanvasAlignment(val);
	}
}

void UIElementEditor2::onChangeSize(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		uiElement->SetSize(val);
	}
}

void UIElementEditor2::onChangeAnchor(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		uiElement->SetAnchor(val);
	}
}
