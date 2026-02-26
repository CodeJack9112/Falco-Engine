#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;
class TreeNode;

class AnimationListEditor2 : public ComponentEditor2
{
public:
	AnimationListEditor2();
	~AnimationListEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	bool isAnimationsIdentical();

	void onChangeEnabled(Property* prop, bool val);
	void onChangeName(Property* prop, string val);
	void onChangeStartFrame(Property* prop, int val);
	void onChangeEndFrame(Property* prop, int val);
	void onChangeDefaultAnimation(Property* prop, int val);
	void onChangePlayAtStart(Property* prop, bool val);
	void onChangeSpeed(Property* prop, float val);
	void onChangeLoop(Property* prop, bool val);

	void onAddAnimation(TreeNode* prop);
	void onPopupSelectedAnimation(TreeNode* prop, int val);
	void onDropFbx(TreeNode * prop, TreeNode * from);
};