#pragma once

#include "Property.h"
#include <string>
#include <OgreSceneNode.h>

class PropertyEditor;

using namespace std;

class PropGameObject : public Property
{
public:
	PropGameObject(PropertyEditor* ed, string name, SceneNode * val = nullptr);
	~PropGameObject();

	virtual void update(bool opened) override;
	void setValue(SceneNode* val);
	SceneNode* getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, SceneNode * val)> callback) { onChangeCallback = callback; }

private:
	SceneNode* value = nullptr;
	string guid1 = "";

	std::function<void(Property * prop, SceneNode * val)> onChangeCallback = nullptr;
};

