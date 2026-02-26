#pragma once

#include <OgreSceneNode.h>
#include <OgreManualObject.h>

class TerrainEditor
{
public:
	enum TerrainEditMode
	{
		EM_RAISE,
		EM_LOWER,
		EM_SMOOTH,
		EM_PAINT_TEXTURE,
		EM_PAINT_DETAIL_MESHES,
		EM_PAINT_GRASS,
		EM_SETTINGS
	};

	static void SetTerrainMode(bool mode) { terrainMode = mode; }
	static bool GetTerrainMode() { return terrainMode; }
	static void SetTerrainEditMode(TerrainEditMode mode) { terrainEditMode = mode; }
	static TerrainEditMode GetTerrainEditMode() { return terrainEditMode; }
	static void SetTerrainBrushSize(float value) { terrainBrushSize = value; }
	static void SetTerrainBrushStrength(float value) { terrainBrushStrength = value; }
	static float GetTerrainBrushSize() { return terrainBrushSize; }
	static float GetTerrainBrushStrength() { return terrainBrushStrength; }
	static int GetSelectedTexture() { return selectedTexture; }
	static int GetSelectedGrass() { return selectedGrass; }
	static int GetSelectedMesh() { return selectedMesh; }
	static void SetSelectedTexture(int value) { selectedTexture = value; }
	static void SetSelectedGrass(int value) { selectedGrass = value; }
	static void SetSelectedMesh(int value) { selectedMesh = value; }

	static void OnLMouseDown(int x, int y);
	static void OnLMouseUp(int x, int y);
	static void OnMouseMove(int x, int y);
	static void UpdateBrushGizmo(int x, int y);
	static void HideBrushGizmo();

private:
	static bool terrainMode;
	static TerrainEditMode terrainEditMode;
	static float terrainBrushSize;
	static float terrainBrushStrength;

	static int selectedTexture;
	static int selectedGrass;
	static int selectedMesh;

	static Ogre::SceneNode* brushNode;
	static Ogre::ManualObject * brushGizmo;
};

