#include "stdafx.h"
#include "DialogProgress.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "MainWindow.h"

#include <OgreRoot.h>

void DialogProgress::show()
{
	visible = true;
	progress = 0;
}

void DialogProgress::hide()
{
	visible = false;
}

void DialogProgress::setProgress(float p)
{
	progress = p;
	Ogre::Root::getSingleton().renderOneFrame();
}

void DialogProgress::update()
{
	if (!visible)
		return;

	ImGui::SetNextWindowSize(ImVec2(300.0f, 180.0f));
	ImGui::SetNextWindowPos(ImVec2(MainWindow::GetWindowSize().x / 2 - 150.0f, MainWindow::GetWindowSize().y / 2 - 90.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

	ImGui::Text(statusText.c_str());
	ImGui::ProgressBar(progress);

	bool closed = false;
	bool ok = false;

	if (ImGui::Button("Cancel"))
	{
		closed = true;
		ok = false;
	}

	ImGui::End();
}