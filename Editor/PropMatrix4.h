#pragma once

#include <OgreMatrix4.h>
#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;
using namespace Ogre;

class PropMatrix4 : public Property
{
public:
	PropMatrix4(PropertyEditor* ed, string name, Matrix4 val = Matrix4::IDENTITY);
	~PropMatrix4();

	virtual void update(bool opened) override;
	void setValue(Matrix4 val);
	Matrix4 getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, Matrix4 val)> callback) { onChangeCallback = callback; }

private:
	Matrix4 value = Matrix4::IDENTITY;
	string guid1 = "";
	string guid2 = "";
	string guid3 = "";
	string guid4 = "";

	std::function<void(Property * prop, Matrix4 val)> onChangeCallback = nullptr;
};

