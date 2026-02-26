#pragma once

#include <string>
#include <vector>

class TreeView;
class TreeNode;
class Property;

using namespace std;

class PropertyEditor
{
public:
	PropertyEditor();
	virtual ~PropertyEditor();

	virtual void init() {}
	virtual void update();
	virtual void updateEditor() {}
	
	void setEditorName(std::string name) { editorName = name; }
	std::string getEditorName() { return editorName; }

	void addProperty(Property* prop);
	void removeProperty(Property* prop);
	TreeView* getTreeView() { return treeView; }
	void setParentEditor(PropertyEditor* ed, TreeNode* node) { parentEditor = ed; parentEditorNode = node; }
	PropertyEditor* getParentEditor() { return parentEditor; }
	TreeNode* getParentEditorNode() { return parentEditorNode; }

private:
	bool isWindowDragging = false;
	TreeView* treeView = nullptr;
	TreeNode* parentEditorNode = nullptr;
	std::vector<Property*> properties;
	PropertyEditor* parentEditor = nullptr;
	std::string editorName = "PropertyEditor";
};

