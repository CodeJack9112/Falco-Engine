#pragma once

#include "UIElementEditor2.h"

class UIButtonEditor2 : public UIElementEditor2
{
public:
	UIButtonEditor2();
	~UIButtonEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	void onDropNormalImage(TreeNode * prop, TreeNode * from);
	void onDropHoverImage(TreeNode * prop, TreeNode * from);
	void onDropPressedImage(TreeNode * prop, TreeNode * from);
	void onDropDisabledImage(TreeNode * prop, TreeNode * from);
	void onDropObjectDownEvent(TreeNode * prop, TreeNode * from);
	void onDropObjectUpEvent(TreeNode * prop, TreeNode * from);

	void onChangePointerDownEventFunc(Property * prop, std::string val);
	void onChangePointerUpEventFunc(Property * prop, std::string val);

	void onEventObjectDownPopup(TreeNode* node, int val);
	void onEventObjectUpPopup(TreeNode* node, int val);

	void onChangeColorNormal(Property* prop, Ogre::ColourValue value);
	void onChangeColorHover(Property* prop, Ogre::ColourValue value);
	void onChangeColorPressed(Property* prop, Ogre::ColourValue value);
	void onChangeColorDisabled(Property* prop, Ogre::ColourValue value);

	void onChangeInteractable(Property* prop, bool value);
};