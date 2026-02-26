#include "stdafx.h"
#include "DialogTextInput.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "MainWindow.h"

DialogTextInput::DialogTextInput()
{
}

DialogTextInput::~DialogTextInput()
{
}

void DialogTextInput::show(string txt, string val, std::function<void(string val, bool okPressed)> callback)
{
	text = txt;
	value = val;
	onCloseCallback = callback;
	visible = true;
}

void DialogTextInput::update()
{
	if (!visible)
		return;

	ImGui::SetNextWindowSize(ImVec2(300.0f, 180.0f));
	ImGui::SetNextWindowPos(ImVec2(MainWindow::GetWindowSize().x / 2, MainWindow::GetWindowSize().y / 2), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
	ImGui::Begin(text.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

	ImGui::InputText("##dialogPromptText", &value);

	bool closed = false;
	bool ok = false;

	if (ImGui::Button("Ok"))
	{
		closed = true;
		ok = true;
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel"))
	{
		closed = true;
		ok = false;
	}

	ImGui::End();

	if (closed)
	{
		if (onCloseCallback != nullptr)
		{
			onCloseCallback(value, ok);
		}
	}
}
