#pragma once

#include "../Ogre/source/OgreMain/custom/Cubemap.h"
#include "PropertyEditor.h"

class TreeNode;

class CubemapEditor2 : public PropertyEditor
{
public:
	CubemapEditor2();
	~CubemapEditor2();

	virtual void init() {}
	virtual void init(CubemapPtr cm);
	virtual void update();
	virtual void updateEditor();

private:
	bool updateState = false;

	CubemapPtr cubemap;

	void onTreeViewEndUpdate();

	void onDropTextureFront(TreeNode* prop, TreeNode* from);
	void onDropTextureBack(TreeNode* prop, TreeNode* from);
	void onDropTextureLeft(TreeNode* prop, TreeNode* from);
	void onDropTextureRight(TreeNode* prop, TreeNode* from);
	void onDropTextureUp(TreeNode* prop, TreeNode* from);
	void onDropTextureDown(TreeNode* prop, TreeNode* from);
};

