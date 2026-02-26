#include "stdafx.h"
#include "PropComboBox.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

PropComboBox::PropComboBox(PropertyEditor* ed, string name, std::vector<string> val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropComboBox::~PropComboBox()
{
}

void PropComboBox::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();

	const char* current = "";

	if (item_current >= value.size())
		current = "";
	else
		current = value[item_current].c_str();

	ImGui::PushItemWidth(-1);
	if (ImGui::BeginCombo(guid1.c_str(), current))
	{
		for (int n = 0; n < value.size(); n++)
		{
			bool is_selected = (current == value[n]);

			if (ImGui::Selectable(value[n].c_str(), is_selected))
			{
				current = value[n].c_str();
				item_current = n;

				if (onChangeCallback != nullptr)
				{
					onChangeCallback(this, value[n]);
				}

				if (onChangeCallback2 != nullptr)
				{
					onChangeCallback2(this, n);
				}
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}

void PropComboBox::setValue(std::vector<string> val)
{
	value = val;
}

void PropComboBox::setCurrentItem(string val)
{
	auto it = find(value.begin(), value.end(), val);

	if (it != value.end())
		item_current = distance(value.begin(), it);
}
