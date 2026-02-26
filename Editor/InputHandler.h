#pragma once

#include <OgreVector2.h>

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"

using namespace Ogre;

class InputHandler
{
private:
	static bool mLButtonDown;
	static bool mMButtonDown;
	static bool mRButtonDown;
	static bool mWasGizmoUndo;

	static Vector2 mPrevMousePoint;
	static Vector2 mStartMousePoint;

public:
	static bool wPressed;
	static bool sPressed;
	static bool aPressed;
	static bool dPressed;
	static bool qPressed;
	static bool ePressed;
	static bool keysPressed;
	static bool ctrlPressed;
	static bool shiftPressed;

public:
	static float cameraSpeed;
	static float cameraSpeedNormal;
	static float cameraSpeedFast;

	static bool IsCtrlPressed() { return ctrlPressed; }
	static bool IsShiftPressed() { return shiftPressed; }
	static bool IsLMousePressed() { return mLButtonDown; }
	static bool IsRMousePressed() { return mRButtonDown; }
	static bool IsMMousePressed() { return mMButtonDown; }

	static void OnLMouseDown(int x, int y);
	static void OnLMouseUp(int x, int y);
	static void OnRMouseDown(int x, int y);
	static void OnRMouseUp(int x, int y);
	static void OnMMouseDown(int x, int y);
	static void OnMMouseUp(int x, int y);
	static void OnMouseMove(int x, int y);
	static void OnKeyDown(SDL_Scancode code);
	static void OnKeyUp(SDL_Scancode code);
	static void OnMouseWheel(int value);
	static void Update();
};

