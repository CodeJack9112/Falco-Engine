#pragma once

#include "UIElementEditor2.h"

class UIImageEditor2 : public UIElementEditor2
{
public:
	UIImageEditor2();
	~UIImageEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	void onDropImage(TreeNode* prop, TreeNode * from);
};
