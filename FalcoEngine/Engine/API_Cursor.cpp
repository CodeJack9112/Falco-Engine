#include "stdafx.h"
#include "API_Cursor.h"

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_video.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_syswm.h"

bool API_Cursor::isCursorVisible = true;

bool API_Cursor::getVisible()
{
	return isCursorVisible;
}

void API_Cursor::setVisible(bool value)
{
	isCursorVisible = value;
	SDL_ShowCursor(value);
}

bool API_Cursor::getLocked()
{
	return SDL_GetRelativeMouseMode();
}

void API_Cursor::setLocked(bool locked)
{
	SDL_SetRelativeMouseMode(SDL_bool(locked));
	SDL_ShowCursor(isCursorVisible);
}
