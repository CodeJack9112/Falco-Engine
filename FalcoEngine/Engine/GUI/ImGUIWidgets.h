#pragma once

#include "../imgui/imgui.h"
#include <imgui\imgui_internal.h>
#include <utility>
#include <vector>
#include <string>

namespace ImGui
{
	IMGUI_API bool ListBox2(const char* label, int* current_item, std::vector<std::pair<std::string, std::string>> const items, int height_items);
	IMGUI_API bool TreeNodeEx2(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...);
	IMGUI_API bool TreeNodeExV2(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);
	IMGUI_API bool TreeNodeBehavior2(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end);
	IMGUI_API bool InvisibleButton2(const char* str_id, const ImVec2& size);
	IMGUI_API bool ImageButtonWithID(ImTextureID user_texture_id, const ImVec2& size, ImGuiID ID, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
}