#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;

class AudioListenerEditor2 : public ComponentEditor2
{
public:
	AudioListenerEditor2();
	~AudioListenerEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangeVolume(Property* prop, float val);
};

