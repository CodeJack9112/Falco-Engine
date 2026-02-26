#include "stdafx.h"
#include "PropMatrix4.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropMatrix4::PropMatrix4(PropertyEditor* ed, string name, Matrix4 val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
	guid2 = "##" + genGuid();
	guid3 = "##" + genGuid();
	guid4 = "##" + genGuid();
}

PropMatrix4::~PropMatrix4()
{
}

void PropMatrix4::update(bool opened)
{
	__super::update(opened);

	ImGui::PushItemWidth(-1);
	if (ImGui::InputFloat4(guid1.c_str(), &value[0][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}

	if (ImGui::InputFloat4(guid2.c_str(), &value[1][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}

	if (ImGui::InputFloat4(guid3.c_str(), &value[2][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}

	if (ImGui::InputFloat4(guid4.c_str(), &value[3][0], 6))
	{
		if (onChangeCallback != nullptr)
			onChangeCallback(this, value);
	}
	ImGui::PopItemWidth();
}

void PropMatrix4::setValue(Matrix4 val)
{
	value = val;
}
