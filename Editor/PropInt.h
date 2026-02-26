#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropInt : public Property
{
public:
	PropInt(PropertyEditor* ed, string name, int val = 0);
	~PropInt();

	virtual void update(bool opened) override;
	void setValue(int val);
	int getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, int val)> callback) { onChangeCallback = callback; }

private:
	int value = 0;
	string guid1 = "";

	std::function<void(Property * prop, int val)> onChangeCallback = nullptr;
};

