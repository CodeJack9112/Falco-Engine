#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;

class MeshColliderEditor2 : public ComponentEditor2
{
public:
	MeshColliderEditor2();
	~MeshColliderEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeConvex(Property* prop, bool val);
	void onChangeIsTrigger(Property* prop, bool val);
};