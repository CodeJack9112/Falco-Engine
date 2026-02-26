#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropComboBox : public Property
{
public:
	PropComboBox(PropertyEditor* ed, string name, std::vector<string> val);
	~PropComboBox();

	virtual void update(bool opened) override;
	void setValue(std::vector<string> val);
	void setCurrentItem(int val) { item_current = val; }
	void setCurrentItem(string val);
	std::vector<string> getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, string val)> callback) { onChangeCallback = callback; }
	void setOnChangeCallback(std::function<void(Property * prop, int val)> callback) { onChangeCallback2 = callback; }

private:
	std::vector<string> value;
	int item_current = 0;
	string guid1 = "";

	std::function<void(Property * prop, string val)> onChangeCallback = nullptr;
	std::function<void(Property * prop, int val)> onChangeCallback2 = nullptr;
};