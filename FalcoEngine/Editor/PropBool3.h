#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropBool3 : public Property
{
public:
	PropBool3(PropertyEditor* ed, string name, string lbl1, string lbl2, string lbl3, bool val[3] = { false });
	~PropBool3();

	virtual void update(bool opened) override;
	void setValue(bool val[3]);
	bool getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, bool val[3])> callback) { onChangeCallback = callback; }

private:
	bool value[3] = { false };
	string guid1 = "";
	string guid2 = "";
	string guid3 = "";

	string label1 = "X";
	string label2 = "Y";
	string label3 = "Z";

	std::function<void(Property * prop, bool val[3])> onChangeCallback = nullptr;
};
