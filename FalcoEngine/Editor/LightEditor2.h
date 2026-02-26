#pragma once

#include "ObjectEditor2.h"

class LightEditor2 : public ObjectEditor2
{
public:
	LightEditor2();
	~LightEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	bool isLightTypesIdentical(std::vector<Ogre::SceneNode*>& nodes);

	void onChangeDiffuseColor(Property * prop, Ogre::ColourValue val);
	void onChangeSpecularColor(Property* prop, Ogre::ColourValue val);
	void onChangeIntensity(Property* prop, float val);
	void onChangeRadius(Property* prop, float val);
	void onChangeInnerAngle(Property* prop, float val);
	void onChangeOuterAngle(Property* prop, float val);
	void onChangeFalloff(Property* prop, float val);
	void onChangeCastShadows(Property* prop, bool val);
	void onChangeType(Property* prop, string val);
	void onChangeMode(Property* prop, string val);
	void onChangeShadowBias(Property* prop, float val);
};