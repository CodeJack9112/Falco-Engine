#pragma once

#include "Property.h"
#include <string>

class PropertyEditor;

using namespace std;

class PropEditorHost : public Property
{
public:
	PropEditorHost(PropertyEditor* ed, string name, PropertyEditor* hosted = nullptr);
	~PropEditorHost();

	virtual void update(bool opened) override;

	void setHostedEditor(PropertyEditor* hosted);
	PropertyEditor* getHostedEditor() { return hostedEditor; }

private:
	PropertyEditor* hostedEditor = nullptr;
};

