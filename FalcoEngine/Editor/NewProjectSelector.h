#pragma once

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_video.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_syswm.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_opengl.h"

#include "ProjectData.h"

class ProjectSelector
{
private:
	SDL_GLContext gl_context;
	SProjectInfo projectsInfo;

	int OpenProject(SProject & project);
	int OpenNewProject();
	int CreateNewProject(std::string projName, std::string projPath);
	bool IsProjectExists(string path);
	void LoadProjects();
	void SaveProjects();

public:
	bool Run();

	SDL_Window* window = nullptr;
};

