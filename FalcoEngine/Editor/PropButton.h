#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropButton : public Property
{
public:
	PropButton(PropertyEditor* ed, string name, string val = "");
	~PropButton();

	virtual void update(bool opened) override;
	void setValue(string val);
	string getValue() { return value; }
	Ogre::TexturePtr getImage() { return image; }
	void setImage(Ogre::TexturePtr val) { image = val; }

	void setOnClickCallback(std::function<void(Property* prop)> callback) { onClickCallback = callback; }

private:
	string value = "";

	std::function<void(Property* prop)> onClickCallback = nullptr;
	Ogre::TexturePtr image;
};

