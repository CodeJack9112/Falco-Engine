#include "stdafx.h"
#include "PropGameObject.h"

#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../imgui/imgui_internal.h"

PropGameObject::PropGameObject(PropertyEditor* ed, string name, SceneNode* val) : Property(ed, name)
{
	setValue(val);

	flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	fullSpaceWidth = false;

	guid1 = "##" + genGuid();
}

PropGameObject::~PropGameObject()
{
}

void PropGameObject::update(bool opened)
{
	__super::update(opened);

	string text = "None [GameObject]";

	if (value != nullptr)
		text = value->getAlias();

	//ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::InputText(guid1.c_str(), &text, ImGuiInputTextFlags_ReadOnly))
	{
		if (onChangeCallback != nullptr)
		{
			onChangeCallback(this, value);
		}
	}
	ImGui::PopItemWidth();
}

void PropGameObject::setValue(SceneNode* val)
{
	value = val;
}
