#include "stdafx.h"
#include "UICanvasEditor2.h"

#include <OgreSceneNode.h>
#include "../Engine/UICanvas.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "PropVector2.h"
#include "PropFloat.h"
#include "PropString.h"
#include "PropComboBox.h"

#include <OgreTextureManager.h>
#include <OgreTexture.h>

UICanvasEditor2::UICanvasEditor2()
{
	setEditorName("UICanvasEditor");
}

UICanvasEditor2::~UICanvasEditor2()
{
}

void UICanvasEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	auto sceneNodes = getSceneNodes();

	SceneNode* sceneNode = sceneNodes[0];
	UICanvas* uiCanvas = (UICanvas*)sceneNode->getAttachedObject(0);

	Property* group = new Property(this, "Canvas");
	group->icon = TextureManager::getSingleton().load("Icons/Hierarchy/canvas.png", "Editor");

	PropComboBox* scaleMode = new PropComboBox(this, "Scale mode", { "Scale with screen size", "Adjust with screen size" });
	scaleMode->setCurrentItem(uiCanvas->GetScaleModeString());
	scaleMode->setOnChangeCallback([=](Property* prop, string val) { onChangeScaleMode(prop, val); });

	PropVector2* szGroup = new PropVector2(this, "Reference screen size", Vector2(uiCanvas->GetReferenceScreenWidth(), uiCanvas->GetReferenceScreenHeight()));
	szGroup->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeScreenSize(prop, val); });

	group->addChild(scaleMode);
	group->addChild(szGroup);

	addProperty(group);
}

void UICanvasEditor2::update()
{
	__super::update();
}

void UICanvasEditor2::onChangeScaleMode(Property* prop, string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UICanvas* uiCanvas = (UICanvas*)(*it)->getAttachedObject(0);
		uiCanvas->SetScaleMode(val);
	}
}

void UICanvasEditor2::onChangeScreenSize(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UICanvas* uiCanvas = (UICanvas*)(*it)->getAttachedObject(0);
		uiCanvas->SetReferenceScreenSize(val.x, val.y);
	}
}
