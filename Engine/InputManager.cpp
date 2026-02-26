#include "stdafx.h"
#include "InputManager.h"
#include <iostream>
#include "Engine.h"

std::pair<int, int> InputManager::mouseDir;
std::pair<int, int> InputManager::mousePos;
std::pair<int, int> InputManager::relativeMousePos;
std::pair<int, int> InputManager::mouseWheel;
std::map<int, bool>  InputManager::keyStates;
std::map<int, bool>  InputManager::keyDownStates;
std::map<int, bool>  InputManager::keyUpStates;
bool InputManager::mouseStates[32];
bool InputManager::mouseDownStates[32];
bool InputManager::mouseUpStates[32];

InputManager::InputManager()
{
	for (int i = 0; i < 1024; ++i)
		keyStates[i] = false;

	ResetKeys();
}

InputManager::~InputManager()
{
	
}

void InputManager::MouseMoveEvent(int x, int y)
{
	for (std::vector<MouseEvent>::iterator it = mouseMoveEvents.begin(); it != mouseMoveEvents.end(); ++it)
	{
		if ((*it) != nullptr)
			(*it)(x, y);
	}
}

void InputManager::MouseDownEvent(MouseButton mb, int x, int y)
{
	for (std::vector<MouseButtonEvent>::iterator it = mouseDownEvents.begin(); it != mouseDownEvents.end(); ++it)
	{
		if ((*it) != nullptr)
			(*it)(mb, x, y);
	}
}

void InputManager::MouseUpEvent(MouseButton mb, int x, int y)
{
	for (std::vector<MouseButtonEvent>::iterator it = mouseUpEvents.begin(); it != mouseUpEvents.end(); ++it)
	{
		if ((*it) != nullptr)
			(*it)(mb, x, y);
	}
}

void InputManager::MouseWheelEvent(int x, int y)
{
	for (std::vector<MouseEvent>::iterator it = mouseWheelEvents.begin(); it != mouseWheelEvents.end(); ++it)
	{
		if ((*it) != nullptr)
			(*it)(x, y);
	}
}

bool InputManager::GetMouseButton(int button)
{
	return mouseStates[button];
}

void InputManager::SetMouseButton(int button, bool pressed)
{
	mouseStates[button] = pressed;
}

bool InputManager::GetMouseButtonDown(int button)
{
	return mouseDownStates[button];
}

void InputManager::SetMouseButtonDown(int button, bool state)
{
	mouseDownStates[button] = state;
}

bool InputManager::GetMouseButtonUp(int button)
{
	return mouseUpStates[button];
}

void InputManager::SetMouseButtonUp(int button, bool state)
{
	mouseUpStates[button] = state;
}

std::pair<int, int> InputManager::GetMouseWheel()
{
	return mouseWheel;
}

void InputManager::SetMouseWheel(int x, int y)
{
	mouseWheel = std::make_pair(x, y);
}

std::pair<int, int> InputManager::GetMouseDirection()
{
	return mouseDir;
}

void InputManager::SetMouseDirection(int x, int y)
{
	mouseDir = std::make_pair(x, y);
}

std::pair<int, int> InputManager::GetMousePosition()
{
	return mousePos;
}

void InputManager::SetMousePosition(int x, int y)
{
	mousePos = std::make_pair(x, y);
}

std::pair<int, int> InputManager::GetMouseRelativePosition()
{
	return relativeMousePos;
}

void InputManager::SetMouseRelativePosition(int x, int y)
{
	relativeMousePos = std::make_pair(x, y);
}

bool InputManager::GetKey(int key)
{
	return keyStates[key];
}

void InputManager::SetKey(int key, bool pressed)
{
	keyStates[key] = pressed;
}

bool InputManager::GetKeyDown(int key)
{
	return keyDownStates[key];
}

void InputManager::SetKeyDown(int key, bool state)
{
	keyDownStates[key] = state;
}

bool InputManager::GetKeyUp(int key)
{
	return keyUpStates[key];
}

void InputManager::SetKeyUp(int key, bool state)
{
	keyUpStates[key] = state;
}

void InputManager::ResetKeys()
{
	for (std::map<int, bool>::iterator it = keyDownStates.begin(); it != keyDownStates.end(); ++it)
		it->second = false;

	for (std::map<int, bool>::iterator it = keyUpStates.begin(); it != keyUpStates.end(); ++it)
		it->second = false;

	for (int i = 0; i < 32; ++i)
	{
		mouseDownStates[i] = false;
		mouseUpStates[i] = false;
	}
}
