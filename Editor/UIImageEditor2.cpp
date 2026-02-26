#include "stdafx.h"
#include "UIImageEditor2.h"

#include <OgreSceneNode.h>
#include "../Engine/UIImage.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "MainWindow.h"

#include "PropString.h"

UIImageEditor2::UIImageEditor2()
{
	setEditorName("UIImageEditor");
}

UIImageEditor2::~UIImageEditor2()
{
}

void UIImageEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	auto sceneNodes = getSceneNodes();

	SceneNode* sceneNode = sceneNodes[0];
	UIElement* uiElement = (UIElement*)sceneNode->getAttachedObject(0);

	Property * group = new Property(this, "UI Image");
	group->icon = TextureManager::getSingleton().load("Icons/Hierarchy/image.png", "Editor");

	std::string imageName = ((UIImage*)uiElement)->GetImageName();
	PropString * image = new PropString(this, "Image", imageName);
	image->setReadonly(true);
	image->setSupportedFormats(MainWindow::GetImagesFileFormats());
	image->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropImage(prop, from); });

	group->addChild(image);
	addProperty(group);
}

void UIImageEditor2::update()
{
	__super::update();
}

void UIImageEditor2::onDropImage(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		UIElement* uiElement = (UIElement*)(*it)->getAttachedObject(0);
		((UIImage*)uiElement)->SetImage(fullPath);
	}
}
