#include "stdafx.h"
#include "PropertyEditor.h"
#include "PropVector3.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "TreeView.h"

PropVector3::PropVector3(PropertyEditor* ed, string name, Vector3 val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;
}

PropVector3::~PropVector3()
{
	
}

void PropVector3::update(bool opened)
{
	__super::update(opened);

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::InputFloat3("", _value, 6))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, Vector3(_value));
		}
	}
	ImGui::PopItemWidth();
}

void PropVector3::setValue(Vector3 val)
{
	value = val;
	_value[0] = value.x;
	_value[1] = value.y;
	_value[2] = value.z;
}
