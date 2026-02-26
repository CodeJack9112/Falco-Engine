#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;
class TreeNode;

class CharacterJointEditor2 : public ComponentEditor2
{
public:
	CharacterJointEditor2();
	~CharacterJointEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeConnectedObject(Property* prop, SceneNode* val);
	void onChangeLinkedBodiesCollision(Property* prop, bool val);
	void onChangeAnchor(Property* prop, Vector3 val);
	void onChangeConnectedAnchor(Property* prop, Vector3 val);
	void onChangeLimitMin(Property* prop, Vector3 val);
	void onChangeLimitMax(Property* prop, Vector3 val);

	void onDropConnectedBody(TreeNode* prop, TreeNode* from);
	void onClickAutoConfigure(Property* prop);
};

