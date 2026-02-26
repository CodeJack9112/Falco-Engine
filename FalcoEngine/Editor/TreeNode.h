#pragma once

#include <string>
#include <vector>
#include <OgreTexture.h>

class TreeView;

class TreeNode
{
friend class TreeView;

public:
	enum CustomPopupMenuPosition { CPMP_BEGIN, CPMP_END };

	TreeNode(TreeView* owner);
	virtual ~TreeNode();

	TreeView* treeView = nullptr;
	std::string alias = "";
	std::string name = "";
	std::string guid1 = "";
	TreeNode* parent = nullptr;
	std::vector<TreeNode*> children;
	bool expanded = false;
	Ogre::TexturePtr icon;
	int flags = 0;
	bool fullSpaceWidth = true;
	bool alwaysShowArrow = false;
	bool enableDrag = false;
	bool checked = false;
	bool showCheckBox = false;
	int popupButton = 1;
	std::string format = "";
	int popupCheckedItem = -1;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	void setHasButton(bool val) { hasButton = val; popupButton = 0; }
	void setButtonText(std::string val) { buttonText = val; }
	void setOnButtonClickCallback(std::function<void(TreeNode * prop)> callback) { onButtonClickCallback = callback; }
	
	virtual void update(bool opened);
	void addChild(TreeNode* child);
	void removeChild(TreeNode* child);
	void genName(std::string name);
	std::string genGuid();
	void setUserData(void* data) { userData = data; }
	void* getUserData() { return userData; }
	void setPopupMenu(std::vector<std::string> items, std::function<void(TreeNode * node, int val)> callback);
	void setCustomPopupMenuProcedure(std::function<void(TreeNode * node)> procedure) { customPopupMenuProcedure = procedure; }
	std::vector<std::string> & getSupportedFormats() { return supportedFormats; }
	void setSupportedFormats(std::vector<std::string> formats) { supportedFormats = formats; }
	bool isFormatSupported(std::string fmt, TreeView * tree);
	void setOnDropCallback(std::function<void(TreeNode * prop, TreeNode * from)> callback) { dropCallback = callback; }
	void setOnEndEditCallback(std::function<bool(TreeNode * node, std::string prevAlias)> callback) { endEditCallback = callback; }
	void setOnDoubleClickCallback(std::function<void(TreeNode * node)> callback) { doubleClickCallback = callback; }
	bool isChildOf(TreeNode * node);

	std::string getPath();
	void setEditMode(bool mode, bool cancel = false);
	void setEditable(bool value) { editable = value; }
	bool getEditable() { return editable; }

	void setClassName(std::string value) { className = value; }
	std::string getClassName() { return className; }

	CustomPopupMenuPosition getCustomPopupMenuPosition() { return customPopupMenuPos; }
	void setCustomPopupMenuPosition(CustomPopupMenuPosition value) { customPopupMenuPos = value; }

private:
	void* userData = nullptr;
	bool editMode = false;
	bool editable = true;
	std::string className = "TreeNode";

	bool hasButton = false;
	std::string buttonText = "...";

	std::function<void(TreeNode * prop)> onButtonClickCallback = nullptr;

	int _oldFlags = 0;
	bool _oldFullSpaceWidth = false;

	std::string editText = "";
	CustomPopupMenuPosition customPopupMenuPos = CustomPopupMenuPosition::CPMP_END;
	
	std::vector<std::string> popupMenuItems;
	std::vector<std::string> supportedFormats;

	std::function<void(TreeNode * node, int val)> onPopupSelectedCallback = nullptr;
	std::function<void(TreeNode * prop, TreeNode * from)> dropCallback = nullptr;
	std::function<bool(TreeNode * node, std::string prevAlias)> endEditCallback = nullptr;
	std::function<void(TreeNode * node)> doubleClickCallback = nullptr;
	std::function<void(TreeNode * node)> customPopupMenuProcedure = nullptr;
};