#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropString : public Property
{
public:
	PropString(PropertyEditor* ed, string name, string val = "");
	~PropString();

	virtual void update(bool opened) override;
	void setValue(string val);
	string getValue() { return value; }
	void setReadonly(bool val) { readOnly = val; }
	void setMultiline(bool val) { multiline = val; }

	void setOnChangeCallback(std::function<void(Property * prop, string val)> callback) { onChangeCallback = callback; }

private:
	string value = "";
	string guid1 = "";
	bool readOnly = false;
	bool multiline = false;

	std::function<void(Property * prop, string val)> onChangeCallback = nullptr;
};

