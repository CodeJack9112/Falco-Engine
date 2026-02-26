#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

class Property;

class NavMeshObstacleEditor2 : public ComponentEditor2
{
public:
	NavMeshObstacleEditor2();
	~NavMeshObstacleEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeSize(Property* prop, Ogre::Vector3 val);
	void onChangeOffset(Property* prop, Ogre::Vector3 val);
};

