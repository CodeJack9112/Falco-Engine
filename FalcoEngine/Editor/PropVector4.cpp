#include "stdafx.h"
#include "PropertyEditor.h"
#include "PropVector4.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "TreeView.h"

PropVector4::PropVector4(PropertyEditor* ed, string name, Vector4 val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;
}

PropVector4::~PropVector4()
{
}

void PropVector4::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::InputFloat4("", _value, 6))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, Vector4(_value));
		}
	}
	ImGui::PopItemWidth();
}

void PropVector4::setValue(Vector4 val)
{
	value = val;
	_value[0] = value.x;
	_value[1] = value.y;
	_value[2] = value.z;
	_value[3] = value.w;
}
