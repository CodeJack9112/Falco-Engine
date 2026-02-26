#pragma once

#include <OgrePrerequisites.h>
#include <OgreTexture.h>

class TerrainWindow
{
public:
	TerrainWindow();
	~TerrainWindow();

	void init();
	void update();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

private:
	bool opened = false;

	Ogre::TexturePtr raiseTexture;
	Ogre::TexturePtr lowerTexture;
	Ogre::TexturePtr smoothTexture;
	Ogre::TexturePtr paintTexture;
	Ogre::TexturePtr treeTexture;
	Ogre::TexturePtr grassTexture;
	Ogre::TexturePtr settingsTexture;

	float brushSize = 1.0f;
	float brushStrength = 1.0f;

	int editTexture = -1;
	int editGrass = -1;
	int editMesh = -1;

	std::vector<int> terrainSizes = { 64, 128, 256, 512, 1024, 2048 };

	void textureEditorWindow();
	void grassEditorWindow();
	void meshEditorWindow();

	void addTerrainTexture();
	void removeTerrainTexture(int index);

	void addTerrainGrass();
	void removeTerrainGrass(int index);

	void addTerrainMesh();
	void removeTerrainMesh(int index);
};