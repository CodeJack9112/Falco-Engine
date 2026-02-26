#include "stdafx.h"
#include "API_GUI.h"
#include "Engine.h"
#include "GUI/ImguiManager.h"
#include "StringConverter.h"
#include "IO.h"
#include "../imgui/imgui_internal.h"

MonoObject * API_GUI::addFont(MonoObject * fileName, int size)
{
	std::string _path = (const char*)mono_string_to_utf8((MonoString*)fileName);
	_path = CP_SYS(_path);

	MonoString * str = (MonoString*)fileName;

	if (!IO::FileExists(GetEngine->GetAssetsPath() + _path))
		return nullptr;
	
	MonoClass * _class = GetEngine->GetMonoRuntime()->font_class;
	MonoObject * font = mono_object_new(GetEngine->GetMonoRuntime()->GetDomain(), _class);
	MonoClassField * fntPath = mono_class_get_field_from_name(_class, "_file_path");
	MonoClassField * nativeFntPtr = mono_class_get_field_from_name(_class, "_nativeFntPtr");
	MonoClassField * fntPtr = mono_class_get_field_from_name(_class, "_fntPtr");

	ImFont * _fntPtr = ImguiManager::getSingleton().addFont(_path, "Assets", size);
	FontPtr nativeFontPtr = FontManager::getSingleton().getByName(_path, "Assets");
	Font* nativeFont = nativeFontPtr.getPointer();

	mono_field_set_value(font, fntPath, str);
	mono_field_set_value(font, nativeFntPtr, reinterpret_cast<void*>(&nativeFont));
	mono_field_set_value(font, fntPtr, reinterpret_cast<void*>(&_fntPtr));

	return font;
}

void API_GUI::setFont(MonoObject * font)
{
	if (font == nullptr)
		return;

	MonoClass * _class = mono_object_get_class(font);
	MonoClassField * fntPath = mono_class_get_field_from_name(_class, "_fntPtr");

	ImFont * _fntPtr = nullptr;

	mono_field_get_value(font, fntPath, reinterpret_cast<void*>(&_fntPtr));
	ImGui::PushFont(_fntPtr);
}

void API_GUI::unsetFont()
{
	ImGuiContext& g = *GImGui;
	if (g.FontStack.size() > 0)
		ImGui::PopFont();
}

void API_GUI::guiLabel(MonoObject * text, API::Vector2 * ref_pos, API::Color * ref_color)
{
	std::string _text = (const char*)mono_string_to_utf8((MonoString*)text);
	//_text = CP_SYS(_text);

	ImGui::SetCursorPos(ImVec2(ref_pos->x, ref_pos->y));
	ImGui::TextColored(ImVec4(ref_color->r, ref_color->g, ref_color->b, ref_color->a), _text.c_str());
}

bool API_GUI::guiButton(MonoObject * text, API::Vector2 * ref_pos, API::Vector2 * ref_size)
{
	std::string _text = (const char*)mono_string_to_utf8((MonoString*)text);
	//_text = CP_SYS(_text);

	ImGui::SetCursorPos(ImVec2(ref_pos->x, ref_pos->y));
	return ImGui::Button(_text.c_str(), ImVec2(ref_size->x, ref_size->y));
}

bool API_GUI::guiBeginWindow(MonoObject * text, API::Vector2 * ref_pos, API::Vector2 * ref_size, float alpha, bool closeButton, int flags)
{
	std::string _text = (const char*)mono_string_to_utf8((MonoString*)text);
	//_text = CP_SYS(_text);

	ImGui::SetNextWindowPos(ImVec2(ref_pos->x, ref_pos->y));
	ImGui::SetNextWindowSize(ImVec2(ref_size->x, ref_size->y));
	ImGui::SetNextWindowBgAlpha(alpha);
	return ImGui::Begin(_text.c_str(), &closeButton, (ImGuiWindowFlags)flags);
}

void API_GUI::guiEndWindow()
{
	ImGui::End();
}

void API_GUI::guiImage(MonoObject * texture, API::Vector2 * ref_pos, API::Vector2 * ref_size, API::Color* ref_color)
{
	if (texture == nullptr)
		return;

	MonoClass * _class = mono_object_get_class(texture);
	MonoClassField * _texPtr = mono_class_get_field_from_name(_class, "_texPtr");

	Texture * texPtr = nullptr;
	mono_field_get_value(texture, _texPtr, reinterpret_cast<void*>(&texPtr));

	if (texPtr != nullptr)
	{
		ImGui::SetCursorPos(ImVec2(ref_pos->x, ref_pos->y));
		ImGui::Image((void*)texPtr->getHandle(), ImVec2(ref_size->x, ref_size->y), ImVec2(0, 0), ImVec2(1, 1), ImVec4(ref_color->r, ref_color->g, ref_color->b, ref_color->a));
	}
}
