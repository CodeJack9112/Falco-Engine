#pragma once

#include <string>
#include <vector>
#include <OgreTexture.h>

enum ToastIcon { TI_NONE, TI_SAVE, TI_IMAGE, TI_COMPILE };

class Toast
{
private:
	class ToastMessage
	{
	public:
		std::string text = "";
		float time = 0.0f;
		bool open = true;
		ToastIcon icon = TI_NONE;
	};

	static std::vector<ToastMessage*> messages;
	static std::vector<Ogre::TexturePtr> icons;

public:
	static void init();
	static void update();
	static void showMessage(std::string text, ToastIcon icon = TI_NONE);
};