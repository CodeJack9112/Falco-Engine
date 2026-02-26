#pragma once

#include <OgrePrerequisites.h>
#include "ComponentEditor2.h"

using namespace Ogre;
using namespace std;

class Property;
class TreeNode;

class AudioSourceEditor2 : public ComponentEditor2
{
public:
	AudioSourceEditor2();
	~AudioSourceEditor2();

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes);

private:
	void onChangeEnabled(Property* prop, bool val);
	void onChangePlayOnStart(Property* prop, bool val);
	void onChangeLoop(Property* prop, bool val);
	void onChangeVolume(Property* prop, float val);
	void onChangeMinDistance(Property* prop, float val);
	void onChangeMaxDistance(Property* prop, float val);
	void onChangeIs2D(Property* prop, bool val);

	void onDropSoundFile(TreeNode * prop, TreeNode * from);
	void onClickSoundFile(Property * prop);
};

