#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;
class TreeNode;

class HingeJointEditor2 : public ComponentEditor2
{
public:
	HingeJointEditor2();
	~HingeJointEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeConnectedObject(Property* prop, SceneNode * val);
	void onChangeLinkedBodiesCollision(Property* prop, bool val);
	void onChangeAnchor(Property* prop, Vector3 val);
	void onChangeConnectedAnchor(Property* prop, Vector3 val);
	void onChangeAxis(Property* prop, Vector3 val);
	void onChangeLimits(Property* prop, Vector2 val);

	void onDropConnectedBody(TreeNode* prop, TreeNode* from);
	void onClickAutoConfigure(Property* prop);
};