#include "stdafx.h"
#include "PropString.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropString::PropString(PropertyEditor* ed, string name, string val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropString::~PropString()
{
}

void PropString::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();

	int flags = 0;

	if (readOnly)
		flags = ImGuiInputTextFlags_ReadOnly;

	ImGui::PushItemWidth(-1);
	if (multiline)
	{
		if (ImGui::InputTextMultiline(guid1.c_str(), &value, ImVec2(0, 0), flags))
		{
			if (onChangeCallback != nullptr)
			{
				onChangeCallback(this, value);
			}
		}
	}
	else
	{
		if (ImGui::InputText(guid1.c_str(), &value, flags))
		{
			if (onChangeCallback != nullptr)
			{
				onChangeCallback(this, value);
			}
		}
	}
	ImGui::PopItemWidth();
}

void PropString::setValue(string val)
{
	value = val;
}
