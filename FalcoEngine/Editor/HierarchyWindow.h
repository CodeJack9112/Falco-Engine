#pragma once

#include <string>
#include <vector>
#include <OgrePrerequisites.h>
#include <OgreTexture.h>

class TreeView;
class TreeNode;

class HierarchyWindow
{
public:
	HierarchyWindow();
	~HierarchyWindow();

	void init();
	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	void addNode(Ogre::SceneNode* objectNode, std::string parentName = "", bool recursive = false);
	TreeView* getTreeView() { return treeView; }

	void refreshHierarchy();

private:
	bool opened = true;
	bool isWindowDragging = false;

	Ogre::TexturePtr addIcon;
	Ogre::TexturePtr closeIcon;

	TreeView* treeView = nullptr;
	
	void onTreeViewEndUpdate();
	void updateObjectMenu();
	bool isNodeChildOf(Ogre::SceneNode * node, Ogre::SceneNode * parent);

	static void onNodesSelected(std::vector<std::string> names, void * userData);
	void onNodeBeginDrag(TreeNode * node);
	void onDropNode(TreeNode * node, TreeNode * from);
	void onReorder(TreeNode* node, int newIndex);
	std::vector<bool> onNodesMoved(std::vector<std::string> items, TreeNode* moveto);
	bool onNodeRenamed(TreeNode* item, std::string oldName);
	void onNodePopupMenu(TreeNode* node, int val);
};

