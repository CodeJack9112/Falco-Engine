#pragma once

#include "../Ogre/source/Components/Terrain/include/OgreTerrain.h"
#include "../Ogre/source/Components/Terrain/include/OgreTerrainGroup.h"
#include "PagedGeometry/include/PagedGeometry.h"
#include "PagedGeometry/include/TreeLoader2D.h"
#include "PagedGeometry/include/GrassLoader.h"

using namespace Forests;

class TerrainManager
{
public:
	struct TextureData
	{
	public:
		std::string diffusePath = "";
		std::string diffuseGuid = "";
		std::string normalPath = "";
		std::string normalGuid = "";
		float worldSize = 100.0f;
	};

	struct DetailMeshData
	{
	public:
		std::string path = "";
		std::string guid = "";
		float minScale = 1.0f;
		float maxScale = 1.5f;
	};

	struct DetailMeshUserData
	{
	public:
		std::string path = "";
		std::string guid = "";
	};

	struct GrassData
	{
	public:
		std::string path = "";
		std::string guid = "";
		Vector2 minSize = Vector2(1, 1);
		Vector2 maxSize = Vector2(2, 2);
		bool animation = true;
		float swayDistribution = 1;
		float swayLength = 1;
		float swaySpeed = 1;
		float density = 1;
	};

	TerrainManager();
	~TerrainManager();
	
	void InitTerrainGroup();
	void InitPagedGeometry();

	std::string CreateTerrain(int x, int y, std::string filename);
	void RemoveAllTerrains();
	void RemoveAllPagedGeometry();
	void CleanUp();
	void SaveAllTerrains();
	std::map<Terrain*, std::string> & GetTerrainList() { return terrainList; }
	TerrainGroup * GetTerrainGroup() { return mTerrainGroup; }
	bool IsTerrainLoaded(int x, int y);

	PagedGeometry* GetDetailPagedGeometry() { return pagedGeometryDetail; }
	PagedGeometry* GetGrassPagedGeometry() { return pagedGeometryGrass; }
	TreeLoader2D* GetTreeLoader() { return treeLoader; }
	GrassLoader* GetGrassLoader() { return grassLoader; }

	GrassLayer* GetGrassLayer(int index);

	std::map<std::string, std::vector<Entity*>> getLoadedEntities() { return loadedEntities; }

	void UpdatePagedGeometry();

	void AddGrass(GrassData data);
	void RemoveGrass(int index);
	void ReplaceGrass(int index, GrassData data);

	void AddDetailMesh(DetailMeshData data);
	void RemoveDetailMesh(int index);
	void ReplaceDetailMesh(int index, DetailMeshData data);

	void AddTexture(TextureData data);
	void RemoveTexture(int index);
	void ReplaceTexture(int index, TextureData data);

	void PlaceDetailMesh(int index, Vector3 pos);
	void PlaceDetailMesh(std::string path, Vector3 position, float scale, Degree yaw);

	void SetPagedGeometryCamera(Camera * camera);

	int GetDetailMeshesDrawDistance() { return detailMeshesDrawDistance; }
	void SetDetailMeshesDrawDistance(int val);
	int GetGrassDrawDistance() { return grassDrawDistance; }
	void SetGrassDrawDistance(int val);

	void RecreateGrassDensityMaps();
	void UpdateGrassBounds();

	std::vector<TextureData>& GetTextures() { return textureList; }
	std::vector<DetailMeshData>& GetDetailMeshes() { return detailMeshList; }
	std::vector<GrassData>& GetGrass() { return grassList; }

private:
	static TerrainManager* self;

	Ogre::TerrainGroup* mTerrainGroup = nullptr;
	Ogre::TerrainGlobalOptions* mTerrainGlobals = nullptr;

	PagedGeometry* pagedGeometryDetail = nullptr;
	PagedGeometry* pagedGeometryGrass = nullptr;

	TreeLoader2D* treeLoader = nullptr;
	GrassLoader* grassLoader = nullptr;

	std::map<Terrain*, std::string> terrainList;
	std::vector<TextureData> textureList;
	std::vector<DetailMeshData> detailMeshList;
	std::vector<GrassData> grassList;

	std::map<std::string, std::vector<Entity*>> loadedEntities;

	int detailMeshesDrawDistance = 750;
	int grassDrawDistance = 700;

	void ExtractEntitiesRecursive(std::string path, SceneNode * root, std::vector<Entity*> & outEntities);
	static float getTerrainHeight(const float x, const float z, void* userData = NULL);

private:
	void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);
	std::string defineTerrain(long x, long y, std::string filename);
	void configureTerrainDefaults();
};