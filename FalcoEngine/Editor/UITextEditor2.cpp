#include "stdafx.h"
#include "UITextEditor2.h"

#include <OgreSceneNode.h>
#include "../Engine/UIText.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "PropString.h"
#include "PropFloat.h"
#include "PropComboBox.h"

UITextEditor2::UITextEditor2()
{
	setEditorName("UITextEditor");
}

UITextEditor2::~UITextEditor2()
{
}

void UITextEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Property* group = new Property(this, "UI Text");
	group->icon = TextureManager::getSingleton().load("Icons/Hierarchy/text.png", "Editor");

	auto sceneNodes = getSceneNodes();

	SceneNode* sceneNode = sceneNodes[0];
	UIText* uiText = (UIText*)sceneNode->getAttachedObject(0);

	PropString * font = new PropString(this, "Font", uiText->getFontName());
	font->setSupportedFormats({ "ttf", "otf" });
	font->setReadonly(true);
	font->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropFont(prop, from); });

	PropFloat * size = new PropFloat(this, "Text size", uiText->getSize());
	size->setOnChangeCallback([=](Property* prop, float val) { onChangeTextSize(prop, val); });

	PropString * text = new PropString(this, "Text", uiText->getText());
	text->setMultiline(true);
	text->setOnChangeCallback([=](Property* prop, string val) { onChangeText(prop, val); });

	std::vector<std::string> valign = { "Top", "Middle", "Bottom" };
	std::vector<std::string> halign = { "Left", "Center", "Right" };

	PropComboBox* hAlign = new PropComboBox(this, "Horizontal alignment", halign);
	PropComboBox* vAlign = new PropComboBox(this, "Vertical alignment", valign);

	hAlign->setCurrentItem(halign[uiText->getHorizontalAlignment()]);
	vAlign->setCurrentItem(valign[uiText->getVerticalAlignment()]);

	hAlign->setOnChangeCallback([=](Property* prop, string val) { onChangeHAlign(prop, val); });
	vAlign->setOnChangeCallback([=](Property* prop, string val) { onChangeVAlign(prop, val); });

	group->addChild(text);
	group->addChild(size);
	group->addChild(font);
	group->addChild(hAlign);
	group->addChild(vAlign);

	addProperty(group);
}

void UITextEditor2::update()
{
	__super::update();
}

void UITextEditor2::onDropFont(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIText* uiElement = (UIText*)(*it)->getAttachedObject(0);
		uiElement->setFontName(fullPath);
	}
}

void UITextEditor2::onChangeHAlign(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIText* uiElement = (UIText*)(*it)->getAttachedObject(0);

		if (val == "Left") uiElement->setHorizontalAlignment(UIText::HorizontalAlignment::H_LEFT);
		if (val == "Center") uiElement->setHorizontalAlignment(UIText::HorizontalAlignment::H_CENTER);
		if (val == "Right") uiElement->setHorizontalAlignment(UIText::HorizontalAlignment::H_RIGHT);
	}
}

void UITextEditor2::onChangeVAlign(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIText* uiElement = (UIText*)(*it)->getAttachedObject(0);

		if (val == "Top") uiElement->setVerticalAlignment(UIText::VerticalAlignment::V_TOP);
		if (val == "Middle") uiElement->setVerticalAlignment(UIText::VerticalAlignment::V_MIDDLE);
		if (val == "Bottom") uiElement->setVerticalAlignment(UIText::VerticalAlignment::V_BOTTOM);
	}
}

void UITextEditor2::onChangeTextSize(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIText* uiElement = (UIText*)(*it)->getAttachedObject(0);
		uiElement->setFontSize(val);
	}
}

void UITextEditor2::onChangeText(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIText* uiElement = (UIText*)(*it)->getAttachedObject(0);
		uiElement->setText(val);
	}
}
