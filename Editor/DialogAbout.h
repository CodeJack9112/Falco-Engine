#pragma once

#include <string>
#include <OgreTexture.h>

class DialogAbout
{
public:
	DialogAbout();
	~DialogAbout();

	void show();
	void update();

private:
	bool visible = false;
	Ogre::TexturePtr logoTexture;

	bool GetVersionInfo(LPCTSTR filename, int& major, int& minor, int& build, int& revision);
};