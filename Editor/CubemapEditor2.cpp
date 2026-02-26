#include "stdafx.h"
#include "CubemapEditor2.h"

#include "TreeView.h"
#include "TreeNode.h"

#include "MainWindow.h"
#include "InspectorWindow.h"
#include "PropString.h"

using namespace Ogre;

CubemapEditor2::CubemapEditor2()
{
	getTreeView()->setOnEndUpdateCallback([=]() { onTreeViewEndUpdate(); });
}

CubemapEditor2::~CubemapEditor2()
{
}

void CubemapEditor2::init(CubemapPtr cm)
{
	__super::init();

	cubemap = cm;

	if (cubemap != nullptr)
	{
		Property* group = new Property(this, cubemap->getName());

		PropString * tex_front = new PropString(this, "Front", cubemap->GetTextureFrontName());
		tex_front->setReadonly(true);
		tex_front->setSupportedFormats(MainWindow::GetImagesFileFormats());
		tex_front->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTextureFront(prop, from); });

		PropString* tex_back = new PropString(this, "Back", cubemap->GetTextureBackName());
		tex_back->setReadonly(true);
		tex_back->setSupportedFormats(MainWindow::GetImagesFileFormats());
		tex_back->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTextureBack(prop, from); });

		PropString* tex_left = new PropString(this, "Left", cubemap->GetTextureLeftName());
		tex_left->setReadonly(true);
		tex_left->setSupportedFormats(MainWindow::GetImagesFileFormats());
		tex_left->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTextureLeft(prop, from); });

		PropString* tex_right = new PropString(this, "Right", cubemap->GetTextureRightName());
		tex_right->setReadonly(true);
		tex_right->setSupportedFormats(MainWindow::GetImagesFileFormats());
		tex_right->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTextureRight(prop, from); });

		PropString* tex_up = new PropString(this, "Up", cubemap->GetTextureUpName());
		tex_up->setReadonly(true);
		tex_up->setSupportedFormats(MainWindow::GetImagesFileFormats());
		tex_up->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTextureUp(prop, from); });

		PropString* tex_down = new PropString(this, "Down", cubemap->GetTextureDownName());
		tex_down->setReadonly(true);
		tex_down->setSupportedFormats(MainWindow::GetImagesFileFormats());
		tex_down->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropTextureDown(prop, from); });

		group->addChild(tex_front);
		group->addChild(tex_back);
		group->addChild(tex_left);
		group->addChild(tex_right);
		group->addChild(tex_up);
		group->addChild(tex_down);

		addProperty(group);
	}
}

void CubemapEditor2::update()
{
	__super::update();
}

void CubemapEditor2::updateEditor()
{
	updateState = true;
}

void CubemapEditor2::onTreeViewEndUpdate()
{
	if (updateState)
	{
		float sp = MainWindow::GetInspectorWindow()->getScrollPos();
		MainWindow::GetInspectorWindow()->saveCollapsedProperties();

		auto props = getTreeView()->getRootNode()->children;

		for (auto it = props.begin(); it != props.end(); ++it)
			removeProperty((Property*)*it);

		init(cubemap);

		MainWindow::GetInspectorWindow()->loadCollapsedProperties();
		MainWindow::GetInspectorWindow()->setScrollPos(sp);

		updateState = false;
	}
}

void CubemapEditor2::onDropTextureFront(TreeNode* prop, TreeNode* from)
{
	if (cubemap != nullptr)
	{
		string fullPath = from->getPath();
		((PropString*)prop)->setValue(fullPath);

		cubemap->SetTextureFrontName(fullPath);

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, MainWindow::GetProjectAssetsDir() + cubemap->getOrigin());
	}
}

void CubemapEditor2::onDropTextureBack(TreeNode* prop, TreeNode* from)
{
	if (cubemap != nullptr)
	{
		string fullPath = from->getPath();
		((PropString*)prop)->setValue(fullPath);

		cubemap->SetTextureBackName(fullPath);

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, MainWindow::GetProjectAssetsDir() + cubemap->getOrigin());
	}
}

void CubemapEditor2::onDropTextureLeft(TreeNode* prop, TreeNode* from)
{
	if (cubemap != nullptr)
	{
		string fullPath = from->getPath();
		((PropString*)prop)->setValue(fullPath);

		cubemap->SetTextureLeftName(fullPath);

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, MainWindow::GetProjectAssetsDir() + cubemap->getOrigin());
	}
}

void CubemapEditor2::onDropTextureRight(TreeNode* prop, TreeNode* from)
{
	if (cubemap != nullptr)
	{
		string fullPath = from->getPath();
		((PropString*)prop)->setValue(fullPath);

		cubemap->SetTextureRightName(fullPath);

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, MainWindow::GetProjectAssetsDir() + cubemap->getOrigin());
	}
}

void CubemapEditor2::onDropTextureUp(TreeNode* prop, TreeNode* from)
{
	if (cubemap != nullptr)
	{
		string fullPath = from->getPath();
		((PropString*)prop)->setValue(fullPath);

		cubemap->SetTextureUpName(fullPath);

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, MainWindow::GetProjectAssetsDir() + cubemap->getOrigin());
	}
}

void CubemapEditor2::onDropTextureDown(TreeNode* prop, TreeNode* from)
{
	if (cubemap != nullptr)
	{
		string fullPath = from->getPath();
		((PropString*)prop)->setValue(fullPath);

		cubemap->SetTextureDownName(fullPath);

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, MainWindow::GetProjectAssetsDir() + cubemap->getOrigin());
	}
}
