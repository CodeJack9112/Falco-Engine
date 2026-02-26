#pragma once

#include "API.h"

class API_Button
{
public:
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Button::get_normalStateImage", (void*)getNormalStateImage);
		mono_add_internal_call("FalcoEngine.Button::set_normalStateImage", (void*)setNormalStateImage);
		mono_add_internal_call("FalcoEngine.Button::get_hoverStateImage", (void*)getHoverStateImage);
		mono_add_internal_call("FalcoEngine.Button::set_hoverStateImage", (void*)setHoverStateImage);
		mono_add_internal_call("FalcoEngine.Button::get_pressedStateImage", (void*)getPressedStateImage);
		mono_add_internal_call("FalcoEngine.Button::set_pressedStateImage", (void*)setPressedStateImage);
		mono_add_internal_call("FalcoEngine.Button::get_disabledStateImage", (void*)getDisabledStateImage);
		mono_add_internal_call("FalcoEngine.Button::set_disabledStateImage", (void*)setDisabledStateImage);
		mono_add_internal_call("FalcoEngine.Button::get_normalStateColor", (void*)getNormalStateColor);
		mono_add_internal_call("FalcoEngine.Button::set_normalStateColor", (void*)setNormalStateColor);
		mono_add_internal_call("FalcoEngine.Button::get_hoverStateColor", (void*)getHoverStateColor);
		mono_add_internal_call("FalcoEngine.Button::set_hoverStateColor", (void*)setHoverStateColor);
		mono_add_internal_call("FalcoEngine.Button::get_pressedStateColor", (void*)getPressedStateColor);
		mono_add_internal_call("FalcoEngine.Button::set_pressedStateColor", (void*)setPressedStateColor);
		mono_add_internal_call("FalcoEngine.Button::get_disabledStateColor", (void*)getDisabledStateColor);
		mono_add_internal_call("FalcoEngine.Button::set_disabledStateColor", (void*)setDisabledStateColor);

		mono_add_internal_call("FalcoEngine.Button::get_interactable", (void*)getInteractable);
		mono_add_internal_call("FalcoEngine.Button::set_interactable", (void*)setInteractable);
	}

private:
	static MonoObject* getNormalStateImage(MonoObject* this_ptr);
	static void setNormalStateImage(MonoObject* this_ptr, MonoObject* texture);

	static MonoObject* getHoverStateImage(MonoObject* this_ptr);
	static void setHoverStateImage(MonoObject* this_ptr, MonoObject* texture);

	static MonoObject* getPressedStateImage(MonoObject* this_ptr);
	static void setPressedStateImage(MonoObject* this_ptr, MonoObject* texture);

	static MonoObject* getDisabledStateImage(MonoObject* this_ptr);
	static void setDisabledStateImage(MonoObject* this_ptr, MonoObject* texture);

	//------------

	static void getNormalStateColor(MonoObject* this_ptr, API::Color* value);
	static void setNormalStateColor(MonoObject* this_ptr, API::Color* value);

	static void getHoverStateColor(MonoObject* this_ptr, API::Color* value);
	static void setHoverStateColor(MonoObject* this_ptr, API::Color* value);

	static void getPressedStateColor(MonoObject* this_ptr, API::Color* value);
	static void setPressedStateColor(MonoObject* this_ptr, API::Color* value);

	static void getDisabledStateColor(MonoObject* this_ptr, API::Color* value);
	static void setDisabledStateColor(MonoObject* this_ptr, API::Color* value);

	//------------

	static bool getInteractable(MonoObject* this_ptr);
	static void setInteractable(MonoObject* this_ptr, bool value);
};

