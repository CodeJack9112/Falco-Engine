#pragma once

#include "Property.h"
#include <OgreVector3.h>

class PropertyEditor;

using namespace Ogre;
using namespace std;

class PropVector3 : public Property
{
public:
	PropVector3(PropertyEditor* ed, string name, Vector3 val = Vector3::ZERO);
	~PropVector3();

	virtual void update(bool opened) override;
	void setValue(Vector3 val);
	Vector3 getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, Vector3 val)> callback) { onChangeCallback = callback; }

private:
	Vector3 value = Vector3::ZERO;
	float _value[3];

	std::function<void(Property * prop, Vector3 val)> onChangeCallback = nullptr;
};