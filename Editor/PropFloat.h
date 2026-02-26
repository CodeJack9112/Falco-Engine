#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropFloat : public Property
{
public:
	PropFloat(PropertyEditor* ed, string name, float val = 0);
	~PropFloat();

	virtual void update(bool opened) override;
	void setValue(float val);
	float getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, float val)> callback) { onChangeCallback = callback; }

private:
	float value = 0;
	string guid1 = "";

	std::function<void(Property * prop, float val)> onChangeCallback = nullptr;
};

