#include "stdafx.h"
#include "PropInt.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropInt::PropInt(PropertyEditor* ed, string name, int val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropInt::~PropInt()
{
}

void PropInt::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::InputInt(guid1.c_str(), &value))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::PopItemWidth();
}

void PropInt::setValue(int val)
{
	value = val;
}
