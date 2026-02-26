#include "stdafx.h"
#include "API_Input.h"
#include "InputManager.h"

void API_Input::Register()
{
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_cursor_direction", (void*)getCursorDirection);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_cursor_position", (void*)getCursorPosition);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_cursor_relative_position", (void*)getCursorRelativePosition);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_key", (void*)getKey);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_key_down", (void*)getKeyDown);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_key_up", (void*)getKeyUp);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_mouse_button", (void*)getMouseButton);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_mouse_button_down", (void*)getMouseButtonDown);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_mouse_button_up", (void*)getMouseButtonUp);
	mono_add_internal_call("FalcoEngine.Input::INTERNAL_get_mouse_wheel", (void*)getMouseWheel);
}

void API_Input::getCursorDirection(API::Vector2 * out_pos)
{
	auto dir = InputManager::GetMouseDirection();
	out_pos->x = dir.first;
	out_pos->y = dir.second;
}

void API_Input::getCursorPosition(API::Vector2* out_pos)
{
	auto pos = InputManager::GetMousePosition();
	out_pos->x = pos.first;
	out_pos->y = pos.second;
}

void API_Input::getCursorRelativePosition(API::Vector2* out_pos)
{
	auto relPos = InputManager::GetMouseRelativePosition();
	out_pos->x = relPos.first;
	out_pos->y = relPos.second;
}

bool API_Input::getKey(int key)
{
	return InputManager::GetKey(key);
}

bool API_Input::getKeyDown(int key)
{
	return InputManager::GetKeyDown(key);
}

bool API_Input::getKeyUp(int key)
{
	return InputManager::GetKeyUp(key);
}

bool API_Input::getMouseButton(int button)
{
	return InputManager::GetMouseButton(button);
}

bool API_Input::getMouseButtonDown(int button)
{
	return InputManager::GetMouseButtonDown(button);
}

bool API_Input::getMouseButtonUp(int button)
{
	return InputManager::GetMouseButtonUp(button);
}

void API_Input::getMouseWheel(API::Vector2* out_value)
{
	auto wheel = InputManager::GetMouseWheel();
	out_value->x = wheel.first;
	out_value->y = wheel.second;
}
