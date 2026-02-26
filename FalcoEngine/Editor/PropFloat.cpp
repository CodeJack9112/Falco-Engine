#include "stdafx.h"
#include "PropFloat.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropFloat::PropFloat(PropertyEditor* ed, string name, float val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropFloat::~PropFloat()
{
}

void PropFloat::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::InputFloat(guid1.c_str(), &value, 0, 0, "%.6f"))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::PopItemWidth();
}

void PropFloat::setValue(float val)
{
	value = val;
}
