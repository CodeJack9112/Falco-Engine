#pragma once

#include "Property.h"
#include <OgreVector2.h>

class PropertyEditor;

using namespace Ogre;
using namespace std;

class PropVector2 : public Property
{
public:
	PropVector2(PropertyEditor* ed, string name, Vector2 val = Vector2::ZERO);
	~PropVector2();

	virtual void update(bool opened) override;
	void setValue(Vector2 val);
	Vector2 getValue() { return value; }
	void setOnChangeCallback(std::function<void(Property * prop, Vector2 val)> callback) { onChangeCallback = callback; }

private:
	Vector2 value = Vector2::ZERO;
	float _value[2];

	std::function<void(Property * prop, Vector2 val)> onChangeCallback = nullptr;
};
