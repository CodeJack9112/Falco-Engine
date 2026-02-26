#pragma once

#include <OgreMatrix3.h>
#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;
using namespace Ogre;

class PropMatrix3 : public Property
{
public:
	PropMatrix3(PropertyEditor* ed, string name, Matrix3 val = Matrix3::IDENTITY);
	~PropMatrix3();

	virtual void update(bool opened) override;
	void setValue(Matrix3 val);
	Matrix3 getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, Matrix3 val)> callback) { onChangeCallback = callback; }

private:
	Matrix3 value = Matrix3::IDENTITY;
	string guid1 = "";
	string guid2 = "";
	string guid3 = "";

	std::function<void(Property * prop, Matrix3 val)> onChangeCallback = nullptr;
};

