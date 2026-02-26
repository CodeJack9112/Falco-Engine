#pragma once

#include "ObjectEditor2.h"

class CameraEditor2 : public ObjectEditor2
{
public:
	CameraEditor2();
	~CameraEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	void onChangeIsMainCamera(Property * prop, bool val);
	void onChangeNearClipPlane(Property* prop, float val);
	void onChangeFarClipPlane(Property* prop, float val);
	void onChangeFOV(Property* prop, float val);
	void onChangeClearColor(Property* prop, Ogre::ColourValue val);
	void onChangeProjectionType(Property* prop, int val);
	void onChangeOrthoSize(Property* prop, float val);
};