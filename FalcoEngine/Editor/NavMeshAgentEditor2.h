#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;

class NavMeshAgentEditor2 : public ComponentEditor2
{
public:
	NavMeshAgentEditor2();
	~NavMeshAgentEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeRadius(Property* prop, float val);
	void onChangeHeight(Property* prop, float val);
	void onChangeSpeed(Property* prop, float val);
	void onChangeAcceleration(Property* prop, float val);
	void onChangeRotationSpeed(Property* prop, float val);
};

