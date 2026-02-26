#include "stdafx.h"
#include "PropBool.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

PropBool::PropBool(PropertyEditor* ed, string name, bool val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropBool::~PropBool()
{
}

void PropBool::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::Checkbox(guid1.c_str(), &value))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::PopItemWidth();
}

void PropBool::setValue(bool val)
{
	value = val;
}
