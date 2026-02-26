#pragma once

#include <vector>

#include "SSceneNode.h"
#include "SEntity.h"
#include "SLight.h"
#include "SEmpty.h"
#include "SCamera.h"
#include "SColor.h"
#include "SUICanvas.h"
#include "SUIButton.h"
#include "SUIText.h"
#include "SUIImage.h"
#include "SParticleSystem.h"
#include "SNavMeshSettings.h"
#include "SLightmapSettings.h"
#include "STerrain.h"
#include "SDetailMeshData.h"
#include "SGrassData.h"
#include "STextureData.h"

#include "../boost/serialization/vector.hpp"

using namespace std;

class SScene
{
public:
	SScene() {}
	~SScene() 
	{
		entities.clear();
		lights.clear();
		empties.clear();
		cameras.clear();
		uiCanvases.clear();
		uiButtons.clear();
		uiTexts.clear();
		uiImages.clear();
		particleSystems.clear();
		terrains.clear();
		terrainTextures.clear();
		detailMeshesData.clear();
		detailMeshDataBrushes.clear();
		grassDataBrushes.clear();
	}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(entities);
		ar & BOOST_SERIALIZATION_NVP(lights);
		ar & BOOST_SERIALIZATION_NVP(empties);
		ar & BOOST_SERIALIZATION_NVP(cameras);
		ar & BOOST_SERIALIZATION_NVP(uiCanvases);
		ar & BOOST_SERIALIZATION_NVP(uiButtons);
		ar & BOOST_SERIALIZATION_NVP(uiTexts);
		ar & BOOST_SERIALIZATION_NVP(uiImages);
		if (version > 2) ar & BOOST_SERIALIZATION_NVP(particleSystems);
		ar & skyboxMaterialName;
		ar & ambientColor;
		if (version > 1) ar & navMeshSettings;
		if (version > 3) ar & BOOST_SERIALIZATION_NVP(terrains);
		if (version > 4) ar & BOOST_SERIALIZATION_NVP(detailMeshesData);
		if (version > 4) ar & BOOST_SERIALIZATION_NVP(detailMeshDataBrushes);
		if (version > 5) ar & BOOST_SERIALIZATION_NVP(grassDataBrushes);
		if (version > 6) ar & lightmapSettings;
		if (version > 7) ar & skyboxMaterialGuid;
		if (version > 8) ar & terrainTextures;
		if (version > 9)
		{
			ar& detailMeshesDrawDistance;
			ar& grassDrawDistance;
		}
	}

	//Serialize members
	std::vector<SEntity> entities;
	std::vector<SLight> lights;
	std::vector<SEmpty> empties;
	std::vector<SCamera> cameras;
	std::vector<SUICanvas> uiCanvases;
	std::vector<SUIButton> uiButtons;
	std::vector<SUIText> uiTexts;
	std::vector<SUIImage> uiImages;
	std::vector<SParticleSystem> particleSystems;

	//NavMesh settings
	SNavMeshSettings navMeshSettings;

	//Lighting settings
	std::string skyboxMaterialName;
	std::string skyboxMaterialGuid;

	SColor ambientColor;
	SLightmapSettings lightmapSettings;



	//Terrain data
	std::vector<STerrain> terrains;
	std::vector<STextureData> terrainTextures;
	std::vector<SDetailMeshData> detailMeshesData;
	std::vector<SDetailMeshDataBrush> detailMeshDataBrushes;
	std::vector<SGrassDataBrush> grassDataBrushes;
	int detailMeshesDrawDistance = 750;
	int grassDrawDistance = 700;
};

BOOST_CLASS_VERSION(SScene, 10)