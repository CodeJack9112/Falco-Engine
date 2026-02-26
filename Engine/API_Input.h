#pragma once

#include "API.h"

class API_Input
{
public:
	//Register methods
	static void Register();

private:
	//Get mouse movement
	static void getCursorDirection(API::Vector2 * out_pos);
	static void getCursorPosition(API::Vector2 * out_pos);
	static void getCursorRelativePosition(API::Vector2 * out_pos);
	static bool getKey(int key);
	static bool getKeyDown(int key);
	static bool getKeyUp(int key);
	static bool getMouseButton(int button);
	static bool getMouseButtonDown(int button);
	static bool getMouseButtonUp(int button);
	static void getMouseWheel(API::Vector2 * out_value);
};