#pragma once

#include <OgreRoot.h>
#include <OgreViewport.h>

class Screen
{
public:
	static float GetScreenScaleFactor();
	static Vector2 GetScreenSize(Viewport* viewport = nullptr);
};