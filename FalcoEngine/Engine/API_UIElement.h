#pragma once

#include "API.h"

class API_UIElement
{
public:
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_getColor", (void*)getColor);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_setColor", (void*)setColor);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_getAnchor", (void*)getAnchor);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_setAnchor", (void*)setAnchor);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_getAlignment", (void*)getAlignment);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_setAlignment", (void*)setAlignment);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_getSize", (void*)getSize);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_setSize", (void*)setSize);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_getPosition", (void*)getPosition);
		mono_add_internal_call("FalcoEngine.UIElement::INTERNAL_setPosition", (void*)setPosition);
	}

private:
	//getColor
	static void getColor(MonoObject* this_ptr, API::Color* out_color);

	//setColor
	static void setColor(MonoObject* this_ptr, API::Color* ref_color);

	//getAnchor
	static void getAnchor(MonoObject* this_ptr, API::Vector2* out_value);

	//setAnchor
	static void setAnchor(MonoObject* this_ptr, API::Vector2* ref_value);

	//getAlignment
	static int getAlignment(MonoObject* this_ptr);

	//setAlignment
	static void setAlignment(MonoObject* this_ptr, int value);

	//getSize
	static void getSize(MonoObject* this_ptr, API::Vector2* out_value);

	//setSize
	static void setSize(MonoObject* this_ptr, API::Vector2* ref_value);

	//getPosition
	static void getPosition(MonoObject* this_ptr, API::Vector2* out_value);

	//setPosition
	static void setPosition(MonoObject* this_ptr, API::Vector2* ref_value);
};

