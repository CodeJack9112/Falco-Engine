#include "stdafx.h"
#include "PropBool3.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

PropBool3::PropBool3(PropertyEditor* ed, string name, string lbl1, string lbl2, string lbl3, bool val[3]) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
	guid2 = "##" + genGuid();
	guid3 = "##" + genGuid();

	label1 = lbl1;
	label2 = lbl2;
	label3 = lbl3;
}

PropBool3::~PropBool3()
{
}

void PropBool3::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::Text(label1.c_str());
	ImGui::SameLine();
	if (ImGui::Checkbox(guid1.c_str(), &value[0]))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::SameLine();
	ImGui::Text(label2.c_str());
	ImGui::SameLine();
	if (ImGui::Checkbox(guid2.c_str(), &value[1]))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::SameLine();
	ImGui::Text(label3.c_str());
	ImGui::SameLine();
	if (ImGui::Checkbox(guid3.c_str(), &value[2]))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
}

void PropBool3::setValue(bool val[3])
{
	value[0] = val[0];
	value[1] = val[1];
	value[2] = val[2];
}
