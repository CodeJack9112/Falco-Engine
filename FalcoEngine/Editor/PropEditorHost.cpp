#include "stdafx.h"
#include "PropEditorHost.h"
#include "PropertyEditor.h"
#include "TreeView.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropEditorHost::PropEditorHost(PropertyEditor* ed, string name, PropertyEditor* hosted) : Property(ed, name)
{
	setClassName("PropEditorHost");

	setHostedEditor(hosted);
}

PropEditorHost::~PropEditorHost()
{
	if (hostedEditor != nullptr)
		delete hostedEditor;

	hostedEditor = nullptr;
}

void PropEditorHost::update(bool opened)
{
	__super::update(opened);

	if (opened)
	{
		if (hostedEditor != nullptr)
			hostedEditor->update();
	}
}

void PropEditorHost::setHostedEditor(PropertyEditor* hosted)
{
	hostedEditor = hosted;
	hostedEditor->setParentEditor(getEditor(), this);

	if (hostedEditor != nullptr)
		alwaysShowArrow = true;
	else
		alwaysShowArrow = false;
}
