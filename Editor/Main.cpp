#include "stdafx.h"

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_video.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_syswm.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_opengl.h"

#include "NewProjectSelector.h"
#include "MainWindow.h"

bool run = false;

int main(int argc, char* argv[])
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	ProjectSelector* projectSelector = new ProjectSelector();
	
	if (!projectSelector->Run())
	{
		MainWindow* mainWindow = new MainWindow();
		mainWindow->Run();
	}

	SDL_Quit();

	return 0;
}