#include "stdafx.h"
#include "ConsoleWindow.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../boost/regex.hpp"

using namespace std;
using namespace Ogre;

ConsoleWindow::ConsoleWindow()
{
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::update()
{
	if (opened)
	{
		if (ImGui::Begin("Console", &opened, ImGuiWindowFlags_NoCollapse))
		{
			if (ImGui::Button("Clear"))
			{
				clear();
			}

			ImGui::SameLine();

			if (ImGui::Checkbox("Clear on play", &clearOnPlay))
			{

			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			ImGuiWindow* window = GImGui->CurrentWindow;
			ImGuiID id = window->GetIDNoKeepAlive("##ConsoleVS");
			ImGui::BeginChild(id);

			int i = 0;
			for (auto it = logData.begin(); it != logData.end(); ++it, ++i)
			{
				bool item_selected = (i == current_item);

				if (ImGui::Selectable(("##logValue_" + to_string(i)).c_str(), item_selected))
				{
					current_item = i;
				}

				ImGui::SameLine();
				ImGui::TextColored(ImVec4(it->color.x, it->color.y, it->color.z, it->color.w), it->text.c_str());
			}

			ImGui::Text("");

			if (scrollTo)
			{
				ImGui::SetScrollHereY();
				scrollTo = false;
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}
}

void ConsoleWindow::log(std::string value, LogMessageType type)
{
	if (value.empty())
		return;

	show(true);

	LogMessageType _type = type;

	Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	if (type == LMT_AUTODETECT)
	{
		boost::regex xRegEx(".*warning[[:space:]]CS\d*.*");
		boost::smatch xResults;

		if (boost::regex_match(value, xResults, xRegEx))
		{
			for (int i = 0; i < xResults.length(); ++i)
			{
				if (xResults[i].length() > 0)
					_type = LMT_WARNING;
			}
		}

		xRegEx = boost::regex(".*error[[:space:]]CS\d*.*");

		if (boost::regex_match(value, xResults, xRegEx))
		{
			for (int i = 0; i < xResults.length(); ++i)
			{
				if (xResults[i].length() > 0)
					_type = LMT_ERROR;
			}
		}
	}

	if (_type == LMT_INFO)
		color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	if (_type == LMT_WARNING)
		color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	if (_type == LMT_SUCCESS)
		color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	if (_type == LMT_ERROR)
		color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	logData.push_back(LogMessage(value, color, _type));

	if (logData.size() > 999)
		logData.erase(logData.begin());

	scrollTo = true;
}

void ConsoleWindow::clear()
{
	logData.clear();
}
