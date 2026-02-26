#pragma once

#include "ObjectEditor2.h"

class UIElementEditor2 : public ObjectEditor2
{
public:
	UIElementEditor2();
	~UIElementEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	void onChangeColor(Property* prop, Ogre::ColourValue val);
	void onChangeAlignment(Property* prop, string val);
	void onChangeSize(Property* prop, Ogre::Vector2 val);
	void onChangeAnchor(Property* prop, Ogre::Vector2 val);
};

