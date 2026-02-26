#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;
class TreeNode;

class VehicleEditor2 : public ComponentEditor2
{
public:
	VehicleEditor2();
	~VehicleEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);

	void onChangeAxis(Property* prop, string val);
	void onChangeInvertForward(Property* prop, bool val);
	
	void onDropConnectedBody(TreeNode* prop, TreeNode* from);
	void onClickAutoConfigure(Property* prop);

	void onChangeRadius(TreeNode* prop, float value);
	void onChangeWidth(TreeNode* prop, float value);
	void onChangeSuspensionStiffness(TreeNode* prop, float value);
	void onChangeSuspensionDamping(TreeNode* prop, float value);
	void onChangeSuspensionCompression(TreeNode* prop, float value);
	void onChangeSuspensionRestLength(TreeNode* prop, float value);
	void onChangeFriction(TreeNode* prop, float value);
	void onChangeRollInfluence(TreeNode* prop, float value);
	void onChangeDirection(TreeNode* prop, Vector3 value);
	void onChangeAxle(TreeNode* prop, Vector3 value);
	void onChangeConnectionPoint(TreeNode* prop, Vector3 value);
	void onChangeIsFrontWheel(TreeNode* prop, bool value);

	void onClickAddWheel(TreeNode* prop);
	void onClickWheelPopup(TreeNode* prop, int index);
};