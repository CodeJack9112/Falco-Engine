#pragma once

#include <OgreVector2.h>
#include <OgreTexture.h>

class ImGuiWindow;

class SceneWindow
{
public:
	SceneWindow();
	~SceneWindow();

	void init();
	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	Ogre::Vector2 getPosition();
	Ogre::Vector2 getSize();
	bool isFocused();
	bool isHovered();

	float getToolbarHeight();

private:
	bool opened = true;
	Ogre::Vector2 position = Ogre::Vector2(0, 0);
	Ogre::Vector2 size = Ogre::Vector2(0, 0);
	Ogre::Vector2 oldSize = Ogre::Vector2(0, 0);
	
	Ogre::TexturePtr cameraIcon;
	Ogre::TexturePtr shadowsIcon;
	Ogre::TexturePtr gridIcon;
	Ogre::TexturePtr navMeshIcon;

	float cameraSpeed1 = 0.0f;
	float cameraSpeed2 = 0.0f;
	float cameraSpeed3 = 0.0f;
	int cameraSpeedPreset = 0;

	bool focused = false;
	bool hovered = false;

	void dropObject(std::string path, Ogre::Vector2 point);
	void dropPrefab(std::string path, Ogre::Vector2 point);

	void updateToolbar();

	void saveCameraSettings();
	void loadCameraSettings();
};