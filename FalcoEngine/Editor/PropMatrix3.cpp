#include "stdafx.h"
#include "PropMatrix3.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropMatrix3::PropMatrix3(PropertyEditor* ed, string name, Matrix3 val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
	guid2 = "##" + genGuid();
	guid3 = "##" + genGuid();
}

PropMatrix3::~PropMatrix3()
{
}

void PropMatrix3::update(bool opened)
{
	__super::update(opened);

	ImGui::PushItemWidth(-1);
	if (ImGui::InputFloat3(guid1.c_str(), &value[0][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}

	if (ImGui::InputFloat3(guid2.c_str(), &value[1][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}

	if (ImGui::InputFloat3(guid3.c_str(), &value[2][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}
	ImGui::PopItemWidth();
}

void PropMatrix3::setValue(Matrix3 val)
{
	value = val;
}
