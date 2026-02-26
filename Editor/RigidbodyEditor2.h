#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;

class RigidbodyEditor2 : public ComponentEditor2
{
public:
	RigidbodyEditor2();
	~RigidbodyEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeMass(Property* prop, float val);
	void onChangeFreezePosition(Property* prop, bool val[3]);
	void onChangeFreezeRotation(Property* prop, bool val[3]);
	void onChangeKinematic(Property* prop, bool val);
	void onChangeStatic(Property* prop, bool val);
	void onChangeUseOwnGravity(Property* prop, bool val);
	void onChangeGravity(Property* prop, Vector3 val);
	void onChangeFriction(Property* prop, float val);
	void onChangeBounciness(Property* prop, float val);
	void onChangeLinearDamping(Property* prop, float val);
	void onChangeAngularDamping(Property* prop, float val);
};