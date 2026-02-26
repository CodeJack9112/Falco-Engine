#pragma once

#include "API.h"

class API_Text
{
public:
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Text::get_text", (void*)getText);
		mono_add_internal_call("FalcoEngine.Text::set_text", (void*)setText);
		mono_add_internal_call("FalcoEngine.Text::get_font", (void*)getFont);
		mono_add_internal_call("FalcoEngine.Text::set_font", (void*)setFont);
		mono_add_internal_call("FalcoEngine.Text::get_fontSize", (void*)getFontSize);
		mono_add_internal_call("FalcoEngine.Text::set_fontSize", (void*)setFontSize);
		mono_add_internal_call("FalcoEngine.Text::INTERNAL_get_horizontalAlignment", (void*)getHorizontalAlignment);
		mono_add_internal_call("FalcoEngine.Text::INTERNAL_set_horizontalAlignment", (void*)setHorizontalAlignment);
		mono_add_internal_call("FalcoEngine.Text::INTERNAL_get_verticalAlignment", (void*)getVerticalAlignment);
		mono_add_internal_call("FalcoEngine.Text::INTERNAL_set_verticalAlignment", (void*)setVerticalAlignment);
	}

private:
	static MonoString* getText(MonoObject* this_ptr);
	static void setText(MonoObject* this_ptr, MonoString* text);

	static MonoString* getFont(MonoObject* this_ptr);
	static void setFont(MonoObject* this_ptr, MonoString* text);

	static float getFontSize(MonoObject* this_ptr);
	static void setFontSize(MonoObject* this_ptr, float size);

	static int getHorizontalAlignment(MonoObject* this_ptr);
	static void setHorizontalAlignment(MonoObject* this_ptr, int alignment);

	static int getVerticalAlignment(MonoObject* this_ptr);
	static void setVerticalAlignment(MonoObject* this_ptr, int alignment);
};

