#include "stdafx.h"
#include "PropertyEditor.h"
#include "PropVector2.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "TreeView.h"

PropVector2::PropVector2(PropertyEditor* ed, string name, Vector2 val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;
}

PropVector2::~PropVector2()
{
}

void PropVector2::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::InputFloat2("", _value, 6))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, Vector2(_value));
		}
	}
	ImGui::PopItemWidth();
}

void PropVector2::setValue(Vector2 val)
{
	value = val;
	_value[0] = value.x;
	_value[1] = value.y;
}
