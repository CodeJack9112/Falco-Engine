#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;

class SphereColliderEditor2 : public ComponentEditor2
{
public:
	SphereColliderEditor2();
	~SphereColliderEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeRadius(Property* prop, float val);
	void onChangeOffsetPosition(Property* prop, Vector3 val);
	void onChangeIsTrigger(Property* prop, bool val);
};