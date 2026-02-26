#pragma once

#include "API.h"

class API_GUI
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_add_font", (void*)addFont);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_set_font", (void*)setFont);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_unset_font", (void*)unsetFont);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_gui_label", (void*)guiLabel);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_gui_button", (void*)guiButton);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_gui_begin_window", (void*)guiBeginWindow);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_gui_end_window", (void*)guiEndWindow);
		mono_add_internal_call("FalcoEngine.GUI::INTERNAL_gui_image", (void*)guiImage);
	}

private:
	static MonoObject * addFont(MonoObject * fileName, int size); 
	static void setFont(MonoObject * font);
	static void unsetFont();
	static void guiLabel(MonoObject * text, API::Vector2 * ref_pos, API::Color * ref_color);
	static bool guiButton(MonoObject * text, API::Vector2 * ref_pos, API::Vector2 * ref_size);
	static bool guiBeginWindow(MonoObject * text, API::Vector2 * ref_pos, API::Vector2 * ref_size, float alpha, bool closeButton, int flags);
	static void guiEndWindow();
	static void guiImage(MonoObject * texture, API::Vector2 * ref_pos, API::Vector2 * ref_size, API::Color * ref_color);
};