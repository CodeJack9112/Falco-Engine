#pragma once

#include <vector>

class PropertyEditor;
class TreeNode;

class InspectorWindow
{
public:
	InspectorWindow();
	~InspectorWindow();

	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	void setEditor(PropertyEditor* editor);
	void updateCurrentEditor();
	void updateObjectEditorTransform();
	float getScrollPos() { return scrollPos; }
	void setScrollPos(float value) { setScrollPosValue = value; }
	void saveCollapsedProperties();
	void loadCollapsedProperties();

private:
	bool opened = true;
	PropertyEditor* currentEditor = nullptr;
	float scrollPos = 0;
	float setScrollPosValue = -1;

	void getAllProperties(TreeNode * root, std::vector<TreeNode*> & list);
};

