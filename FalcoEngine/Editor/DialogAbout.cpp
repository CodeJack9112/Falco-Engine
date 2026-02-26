#include "stdafx.h"
#include "DialogAbout.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "MainWindow.h"
#include "EditorClasses.h"
#include "../Player/Helpers.h"

ImVec4 linkColor = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
string version = "";

DialogAbout::DialogAbout()
{
	
}

DialogAbout::~DialogAbout()
{
}

bool DialogAbout::GetVersionInfo(LPCTSTR filename, int& major, int& minor, int& build, int& revision)
{
    DWORD   verBufferSize;
    char    verBuffer[2048];

    //  Get the size of the version info block in the file
    verBufferSize = GetFileVersionInfoSize(filename, NULL);
    if (verBufferSize > 0 && verBufferSize <= sizeof(verBuffer))
    {
        //  get the version block from the file
        if (TRUE == GetFileVersionInfo(filename, NULL, verBufferSize, verBuffer))
        {
            UINT length;
            VS_FIXEDFILEINFO* verInfo = NULL;

            //  Query the version information for neutral language
            if (TRUE == VerQueryValue(
                verBuffer,
                _T("\\"),
                reinterpret_cast<LPVOID*>(&verInfo),
                &length))
            {
                //  Pull the version values.
                major = HIWORD(verInfo->dwProductVersionMS);
                minor = LOWORD(verInfo->dwProductVersionMS);
                build = HIWORD(verInfo->dwProductVersionLS);
                revision = LOWORD(verInfo->dwProductVersionLS);
                return true;
            }
        }
    }

    return false;
}

void DialogAbout::show()
{
	logoTexture = TextureManager::getSingleton().load("Logo.png", "Editor");
	visible = true;

    string path = Helper::ExeName();
    int major = 0;
    int minor = 0;
    int build = 0;
    int revision = 0;
    GetVersionInfo(CString(path.c_str()), major, minor, build, revision);
    version = to_string(major) + "." + to_string(minor) + "." + to_string(build) + "." + to_string(revision);
}

void DialogAbout::update()
{
	if (!visible)
		return;

	ImGui::SetNextWindowSize(ImVec2(300.0f, 210.0f));
	ImGui::SetNextWindowPos(ImVec2(MainWindow::GetWindowSize().x / 2, MainWindow::GetWindowSize().y / 2), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
	ImGui::Begin("About Falco Engine", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

	ImGui::Image((void*)logoTexture->getHandle(), ImVec2(64, 64));
    ImGui::Text("Falco Engine, version"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.0f, 1.0f), version.c_str());
	ImGui::Text("Copyright: Falco Software Company");
	ImGui::TextColored(linkColor, "https://www.falco3d.com");
	if (ImGui::IsItemHovered())
		linkColor = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
	else
		linkColor = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);

	if (ImGui::IsItemClicked())
		ShellExecute(0, 0, L"https://www.falco3d.com", 0, 0, SW_SHOW);

	ImGui::Text("(C) 2018 - 2020");

	if (ImGui::Button("Close"))
	{
		visible = false;
	}

	ImGui::End();
}
