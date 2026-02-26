#pragma once

#include "ObjectEditor2.h"

class UICanvasEditor2 : public ObjectEditor2
{
public:
	UICanvasEditor2();
	~UICanvasEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	void onChangeScaleMode(Property* prop, std::string val);
	void onChangeScreenSize(Property* prop, Ogre::Vector2 val);
};

