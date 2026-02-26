#include "stdafx.h"
#include "PropButton.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropButton::PropButton(PropertyEditor* ed, string name, string val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;
}

PropButton::~PropButton()
{
}

void PropButton::update(bool opened)
{
	__super::update(opened);

	ImGui::PushItemWidth(-1);
	ImVec2 sz = ImGui::GetContentRegionAvail();

	bool btn = false;

	if (image == nullptr)
		btn = ImGui::Button(value.c_str(), ImVec2(sz.x, 0));
	else
		btn = ImGui::ImageButton((void*)image->getHandle(), ImVec2(60, 64));

	if (btn)
	{
		if (onClickCallback != nullptr)
		{
			onClickCallback(this);
		}
	}
	ImGui::PopItemWidth();
}

void PropButton::setValue(string val)
{
	value = val;
}
