#pragma once

#include <OgrePrerequisites.h>
#include <OgreColourValue.h>
#include <string>

class DialogProgress;

class LightingWindow
{
public:
	LightingWindow();
	~LightingWindow();

	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	void setSkyBoxMaterial(std::string value) { skyboxMaterial = value; }
	void setAmbientLightColor(Ogre::ColourValue value) { ambientLightColor = value; }
	void setDefaultLightmapSize(int value) { defaultLightmapSize = value; }
	void setRegionLightmapSize(int value) { regionLightmapSize = value; }
	void setLightmapBlurRadius(float value) { lightmapBlurRadius = value; }

private:
	bool opened = true;
	std::string skyboxMaterial = "None";
	Ogre::ColourValue ambientLightColor = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f);
	int defaultLightmapSize = 0;
	int regionLightmapSize = 0;
	float lightmapBlurRadius = 0;
	DialogProgress* progressDialog = nullptr;

	std::vector<int> lightmapSizes = { 64, 128, 256, 512, 1024, 2048 };
	std::vector<int> regionLightmapSizes = { 512, 1024, 2048, 4096 };

	void clearBakedData();
	void bakeLighting();
	void getSceneNodesWithEntities(Ogre::SceneNode* root, std::vector<Ogre::SceneNode*>& outList);
};

