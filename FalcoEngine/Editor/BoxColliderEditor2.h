#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;

class BoxColliderEditor2 : public ComponentEditor2
{
public:
	BoxColliderEditor2();
	~BoxColliderEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeSize(Property* prop, Vector3 val);
	void onChangeOffsetPosition(Property* prop, Vector3 val);
	void onChangeOffsetRotation(Property* prop, Vector3 val);
	void onChangeIsTrigger(Property* prop, bool val);
};

