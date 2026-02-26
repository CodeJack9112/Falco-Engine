#pragma once
#include "Property.h"
#include <OgreVector4.h>

class PropertyEditor;

using namespace Ogre;
using namespace std;

class PropVector4 : public Property
{
public:
	PropVector4(PropertyEditor* ed, string name, Vector4 val = Vector4::ZERO);
	~PropVector4();

	virtual void update(bool opened) override;
	void setValue(Vector4 val);
	Vector4 getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, Vector4 val)> callback) { onChangeCallback = callback; }

private:
	Vector4 value = Vector4::ZERO;
	float _value[4];

	std::function<void(Property * prop, Vector4 val)> onChangeCallback = nullptr;
};