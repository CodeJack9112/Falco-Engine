#pragma once

#include "UIElementEditor2.h"

class UITextEditor2 : public UIElementEditor2
{
public:
	UITextEditor2();
	~UITextEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	void onDropFont(TreeNode* prop, TreeNode* from);

	void onChangeHAlign(Property* prop, std::string val);
	void onChangeVAlign(Property* prop, std::string val);
	void onChangeTextSize(Property* prop, float val);
	void onChangeText(Property* prop, std::string val);
};