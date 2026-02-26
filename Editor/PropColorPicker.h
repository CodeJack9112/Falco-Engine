#pragma once

#include <OgreColourValue.h>
#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;
using namespace Ogre;

class PropColorPicker : public Property
{
public:
	PropColorPicker(PropertyEditor* ed, string name, ColourValue val = ColourValue::White);
	~PropColorPicker();

	virtual void update(bool opened) override;
	void setValue(ColourValue val);
	ColourValue getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, ColourValue val)> callback) { onChangeCallback = callback; }

private:
	ColourValue value = ColourValue::White;
	string guid1 = "";

	std::function<void(Property * prop, ColourValue val)> onChangeCallback = nullptr;
};