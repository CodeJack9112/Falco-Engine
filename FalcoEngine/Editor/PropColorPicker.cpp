#include "stdafx.h"
#include "PropColorPicker.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

PropColorPicker::PropColorPicker(PropertyEditor* ed, string name, ColourValue val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropColorPicker::~PropColorPicker()
{
}

void PropColorPicker::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::ColorEdit4(guid1.c_str(), value.ptr()))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::PopItemWidth();
}

void PropColorPicker::setValue(ColourValue val)
{
	value = val;
}
