#include "stdafx.h"
#include "TerrainManager.h"
#include "Engine.h"
#include "DeferredShading/TerrainMaterialGeneratorD.h"
#include "IO.h"
#include "../boost/algorithm/string/replace.hpp"
#include "../boost/algorithm/string.hpp"
#include "Mathf.h"

#include "PhysicsManager.h"
#include "../Bullet/Include/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "FBXSceneManager.h"
#include "SceneSerializer.h"

#include "PagedGeometry/include/BatchPage.h"
#include "PagedGeometry/include/ImpostorPage.h"
#include "ResourceMap.h"

#include <OgreSubEntity.h>

TerrainManager* TerrainManager::self;

TerrainManager::TerrainManager()
{
	self = this;
}

TerrainManager::~TerrainManager()
{
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobals;
}

void TerrainManager::InitTerrainGroup()
{
	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(GetEngine->GetSceneManager(), Ogre::Terrain::ALIGN_X_Z, 129, 3000.0);
	mTerrainGroup->setResourceGroup("Assets");
	//mTerrainGroup->setFilenameConvention(Ogre::String("terrain"), Ogre::String("terrain"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	configureTerrainDefaults();

}

void TerrainManager::InitPagedGeometry()
{
	//Paged geometry for detail meshes config
	pagedGeometryDetail = new PagedGeometry(nullptr, GetEngine->GetSceneManager(), "detail", 100.0f);
	pagedGeometryDetail->addDetailLevel<BatchPage>(detailMeshesDrawDistance, 0);
	//pagedGeometryDetail->addDetailLevel<ImpostorPage>(detailMeshesDrawDistance * 2.0f, 0);
	
	//Paged geometry for grass config
	pagedGeometryGrass = new PagedGeometry(nullptr, GetEngine->GetSceneManager(), "grass", 100.0f);
	pagedGeometryGrass->addDetailLevel<GrassPage>(grassDrawDistance, 0);
	
	//Detail mesh loader config
	treeLoader = new TreeLoader2D(pagedGeometryDetail, TBounds(-1500, -1500, 1500, 1500));
	treeLoader->setMaximumScale(10.0f);
	treeLoader->setHeightFunction(getTerrainHeight);

	//Grass loader config
	grassLoader = new GrassLoader(pagedGeometryGrass);
	grassLoader->setHeightFunction(getTerrainHeight);

	pagedGeometryGrass->setPageLoader(grassLoader);
	pagedGeometryDetail->setPageLoader(treeLoader);
}

std::string TerrainManager::CreateTerrain(int x, int y, std::string filename)
{
	std::string repl = IO::ReplaceBackSlashes(filename);
	repl = boost::replace_all_copy(repl, GetEngine->GetAssetsPath(), "");

	std::string fn = defineTerrain(x, y, repl);

	mTerrainGroup->loadAllTerrains(true);

	Terrain* terrain = mTerrainGroup->getTerrain(0, 0);

	terrain->load();

	if (GetEngine->GetUseUnpackedResources())
		terrain->save(filename);

	terrainList[terrain] = repl;

	//Setup collision
	int terrainPageSize = terrain->getSize();
	
	float* pTerrainHeightData = terrain->getHeightData();
	float* pTerrainHeightDataConvert = new float[terrainPageSize * terrainPageSize];

	float _min = terrain->getMinHeight();
	float _max = terrain->getMaxHeight();

	for (int i = 0; i < terrainPageSize; ++i)
	{
		memcpy(pTerrainHeightDataConvert + terrainPageSize * i,
			pTerrainHeightData + terrainPageSize * (terrainPageSize - i - 1),
			sizeof(float) * (terrainPageSize));
	}

	btHeightfieldTerrainShape * mesh = new btHeightfieldTerrainShape(terrainPageSize, terrainPageSize, pTerrainHeightDataConvert, 1.0f, _min, _max, 1, PHY_FLOAT, true);

	float unitsBetweenVertices = terrain->getWorldSize() / (terrainPageSize - 1.0f);
	btVector3 scaling(unitsBetweenVertices, 1.0f, unitsBetweenVertices);
	mesh->setLocalScaling(scaling);
	mesh->setUseDiamondSubdivision(true);

	Ogre::Vector3 terrainPosition = terrain->getPosition();
	btTransform groundTransform;

	groundTransform.setOrigin(btVector3(terrainPosition.x,
		terrainPosition.y + ((_max - _min) / 2.0f) + _min,
		terrainPosition.z));

	groundTransform.setRotation(btQuaternion(Ogre::Quaternion::IDENTITY.x,
		Ogre::Quaternion::IDENTITY.y,
		Ogre::Quaternion::IDENTITY.z,
		Ogre::Quaternion::IDENTITY.w));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody* pBody = new btRigidBody(0.0, myMotionState, mesh);

	pBody->setCollisionFlags(pBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	GetEngine->GetPhysicsManager()->GetWorld()->addRigidBody(pBody);

	mTerrainGroup->freeTemporaryResources();
	return fn;
}

void TerrainManager::RemoveAllTerrains()
{
	mTerrainGroup->removeAllTerrains();
	terrainList.clear();

	RemoveAllPagedGeometry();
}

void TerrainManager::RemoveAllPagedGeometry()
{
	detailMeshList.clear();
	textureList.clear();

	for (std::map<std::string, std::vector<Entity*>>::iterator it = loadedEntities.begin(); it != loadedEntities.end(); ++it)
	{
		for (std::vector<Entity*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			GetEngine->GetSceneManager()->destroyEntity(*it2);
		}

		it->second.clear();
	}

	loadedEntities.clear();

	if (treeLoader != nullptr)
		treeLoader->deleteTrees(TBounds(-1500, -1500, 1500, 1500));

	if (grassLoader != nullptr)
	{
		for (int i = 0; i < grassLoader->getLayerList().size(); ++i)
		{
			RemoveGrass(i);
		}
	}

	grassList.clear();
}

void TerrainManager::CleanUp()
{
	detailMeshesDrawDistance = 750;
	grassDrawDistance = 700;

	RemoveAllPagedGeometry();

	if (pagedGeometryGrass != nullptr)
	{
		pagedGeometryGrass->removeDetailLevels();
		delete pagedGeometryGrass;
		pagedGeometryGrass = nullptr;
	}

	if (pagedGeometryDetail != nullptr)
	{
		pagedGeometryDetail->removeDetailLevels();
		delete pagedGeometryDetail;
		pagedGeometryDetail = nullptr;
	}

	RemoveAllPagedGeometry();

	if (grassLoader != nullptr)
	{
		delete grassLoader;
		grassLoader = nullptr;
	}

	if (treeLoader != nullptr)
	{
		delete treeLoader;
		treeLoader = nullptr;
	}

	RemoveAllTerrains();
}

void TerrainManager::SaveAllTerrains()
{
	for (std::map<Terrain*, std::string>::iterator it = terrainList.begin(); it != terrainList.end(); ++it)
	{
		it->first->save(GetEngine->GetAssetsPath() + it->second);
	}
}

bool TerrainManager::IsTerrainLoaded(int x, int y)
{
	return mTerrainGroup->getTerrain(0, 0) != nullptr;
}

GrassLayer* TerrainManager::GetGrassLayer(int index)
{
	return *std::next(grassLoader->getLayerList().begin(), index);
}

void TerrainManager::UpdatePagedGeometry()
{
	if (pagedGeometryGrass != nullptr && pagedGeometryGrass->getCamera() != nullptr && pagedGeometryGrass->getCamera()->getParentSceneNode() != nullptr)
		pagedGeometryGrass->update();

	if (pagedGeometryDetail != nullptr && pagedGeometryDetail->getCamera() != nullptr && pagedGeometryDetail->getCamera()->getParentSceneNode() != nullptr)
		pagedGeometryDetail->update();
}

void TerrainManager::AddGrass(GrassData data)
{
	int wsz = GetTerrainGroup()->getTerrain(0, 0)->getWorldSize();

	grassList.push_back(data);

	GrassLayer* layer = grassLoader->addLayer();
	layer->setMapBounds(TBounds(-wsz / 2, -wsz / 2, wsz / 2, wsz / 2));
	layer->setAnimationEnabled(data.animation);
	layer->setDensity(data.density);
	layer->setMinimumSize(data.minSize.x, data.minSize.y);
	layer->setMaximumSize(data.maxSize.x, data.maxSize.y);
	layer->setSwayDistribution(data.swayDistribution);
	layer->setSwayLength(data.swayLength);
	layer->setSwaySpeed(data.swaySpeed);

	string _path = ResourceMap::getResourceNameFromGuid(data.guid);
	if (_path.empty())
		_path = data.path;

	//if (!MaterialManager::getSingleton().resourceExists(_path, "Assets"))
	//	_path = IO::GetFileName(_path);

	if (MaterialManager::getSingleton().resourceExists(_path, "Assets"))
	{
		MaterialManager::getSingleton().getByName(_path, "Assets")->load();
		layer->setMaterialName(_path);
	}

	int sz = GetTerrainGroup()->getTerrain(0, 0)->getSize();

	TexturePtr map = TextureManager::getSingleton().createManual("TerrainDensityMap_" + to_string(grassList.size() - 1),
		Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		sz - 1,
		sz - 1,
		0,
		PixelFormat::PF_FLOAT16_R);

	layer->setDensityMap(map);

	Terrain* terrain = GetTerrainGroup()->getTerrain(0, 0);
	for (int i = 0; i < terrain->getSize(); ++i)
	{
		for (int j = 0; j < terrain->getSize(); ++j)
		{
			layer->getDensityMap()->setDensityAt(i, j, FloatRect(0, 0, terrain->getSize(), terrain->getSize()), 0.0f);
		}
	}

	TextureManager::getSingleton().remove(map);
}

void TerrainManager::RemoveGrass(int index)
{
	grassList.erase(grassList.begin() + index);
	GrassLayer* layer = *std::next(grassLoader->getLayerList().begin(), index);
	grassLoader->deleteLayer(layer);

	pagedGeometryGrass->reloadGeometry();
}

void TerrainManager::ReplaceGrass(int index, GrassData data)
{
	int sz = GetTerrainGroup()->getTerrain(0, 0)->getWorldSize();

	grassList[index] = data;

	if (MaterialManager::getSingleton().resourceExists(data.path, "Assets"))
	{
		GrassLayer* layer = *std::next(grassLoader->getLayerList().begin(), index);
		layer->setMaterialName(data.path);
		layer->setMapBounds(TBounds(-sz / 2, -sz / 2, sz / 2, sz / 2));
		layer->setAnimationEnabled(data.animation);
		layer->setDensity(data.density);
		layer->setMinimumSize(data.minSize.x, data.minSize.y);
		layer->setMaximumSize(data.maxSize.x, data.maxSize.y);
		layer->setSwayDistribution(data.swayDistribution);
		layer->setSwayLength(data.swayLength);
		layer->setSwaySpeed(data.swaySpeed);
	}

	pagedGeometryGrass->reloadGeometry();
}

void TerrainManager::AddDetailMesh(DetailMeshData data)
{
	detailMeshList.push_back(data);
}

void TerrainManager::RemoveDetailMesh(int index)
{
	detailMeshList.erase(detailMeshList.begin() + index);
}

void TerrainManager::ReplaceDetailMesh(int index, DetailMeshData data)
{
	detailMeshList[index] = data;
}

void TerrainManager::AddTexture(TextureData data)
{
	textureList.push_back(data);
}

void TerrainManager::RemoveTexture(int index)
{
	textureList.erase(textureList.begin() + index);
}

void TerrainManager::ReplaceTexture(int index, TextureData data)
{
	textureList[index] = data;
}

void TerrainManager::PlaceDetailMesh(int index, Vector3 pos)
{
	if (index >= 0 && index < detailMeshList.size())
	{
		std::string path = detailMeshList[index].path;
		float minScale = detailMeshList[index].minScale;
		float maxScale = detailMeshList[index].maxScale;
		float scale = Mathf::RandomFloat(minScale, maxScale);
		Degree yaw = Degree(Mathf::RandomFloat(0.0f, 360.0f));

		PlaceDetailMesh(path, pos, scale, yaw);
	}
}

void TerrainManager::PlaceDetailMesh(std::string path, Vector3 position, float scale, Degree yaw)
{
	DetailMeshUserData* dt = new DetailMeshUserData();
	dt->path = path;

	std::string ext = IO::GetFileExtension(path);
	
	if (ext == "fbx")
	{
		FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(path, "Assets");
		SceneNode* node = fbx->ConvertToNativeFormat();

		dt->guid = ResourceMap::guidMap[fbx.getPointer()];

		std::vector<Entity*> addEntities;
		if (loadedEntities.find(path) != loadedEntities.end())
			addEntities = loadedEntities[path];
		else
			ExtractEntitiesRecursive(path, node, addEntities);

		for (std::vector<Entity*>::iterator it = addEntities.begin(); it != addEntities.end(); ++it)
		{
			treeLoader->addTree(*it, position, yaw, scale, static_cast<void*>(dt));
		}

		Engine::Destroy(node);
	}
	if (ext == "prefab")
	{
		SceneSerializer ser;
		SceneManager* sm = GetEngine->GetSceneManager();
		SceneNode* node = ser.DeserializeFromPrefab(sm, GetEngine->GetAssetsPath() + path);
		GetEngine->GetSceneManager()->getRootSceneNode()->removeChild(node);

		dt->guid = ResourceMap::getResourceGuidFromName(path);

		std::vector<Entity*> addEntities;
		if (loadedEntities.find(path) != loadedEntities.end())
			addEntities = loadedEntities[path];
		else
			ExtractEntitiesRecursive(path, node, addEntities);

		for (std::vector<Entity*>::iterator it = addEntities.begin(); it != addEntities.end(); ++it)
		{
			treeLoader->addTree(*it, position, yaw, scale, static_cast<void*>(dt));
		}

		Engine::Destroy(node);
	}
}

void TerrainManager::SetPagedGeometryCamera(Camera* camera)
{
	if (pagedGeometryDetail != nullptr)
		pagedGeometryDetail->setCamera(camera);

	if (pagedGeometryGrass != nullptr)
		pagedGeometryGrass->setCamera(camera);
}

void TerrainManager::SetDetailMeshesDrawDistance(int val)
{
	if (val > 10000)
		val = 10000;

	if (val > 0)
	{
		detailMeshesDrawDistance = val;

		pagedGeometryDetail->removeDetailLevels();
		pagedGeometryDetail->addDetailLevel<BatchPage>(detailMeshesDrawDistance, 0);

		pagedGeometryDetail->resetPreloadedGeometry();
		pagedGeometryDetail->reloadGeometry();
	}
	else
	{
		pagedGeometryDetail->resetPreloadedGeometry();
		pagedGeometryDetail->removeDetailLevels();
		pagedGeometryDetail->reloadGeometry();
	}
}

void TerrainManager::SetGrassDrawDistance(int val)
{
	if (val > 10000)
		val = 10000;

	if (val > 0)
	{
		grassDrawDistance = val;

		pagedGeometryGrass->removeDetailLevels();
		pagedGeometryGrass->addDetailLevel<GrassPage>(grassDrawDistance, 0);

		pagedGeometryGrass->resetPreloadedGeometry();
		pagedGeometryGrass->reloadGeometry();
	}
	else
	{
		pagedGeometryGrass->resetPreloadedGeometry();
		pagedGeometryGrass->removeDetailLevels();
	}
}

void TerrainManager::RecreateGrassDensityMaps()
{
	int sz = GetTerrainGroup()->getTerrain(0, 0)->getSize();
	int wsz = GetTerrainGroup()->getTerrain(0, 0)->getWorldSize();
	int i = 0;

	for (auto it = grassLoader->getLayerList().begin(); it != grassLoader->getLayerList().end(); ++it, ++i)
	{
		(*it)->setMapBounds(TBounds(-wsz / 2, -wsz / 2, wsz / 2, wsz / 2));

		TexturePtr map = TextureManager::getSingleton().createManual("TerrainDensityMap_" + to_string(i),
			Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D,
			sz - 1,
			sz - 1,
			0,
			PixelFormat::PF_FLOAT16_R);

		TextureManager::getSingleton().remove(map);

		(*it)->setDensityMap(map);

		for (int i = 0; i < sz; ++i)
		{
			for (int j = 0; j < sz; ++j)
			{
				(*it)->getDensityMap()->setDensityAt(i, j, FloatRect(0, 0, sz, sz), 0.0f);
			}
		}
	}
}

void TerrainManager::UpdateGrassBounds()
{
	int wsz = GetTerrainGroup()->getTerrain(0, 0)->getWorldSize();

	for (auto it = grassLoader->getLayerList().begin(); it != grassLoader->getLayerList().end(); ++it)
	{
		(*it)->setMapBounds(TBounds(-wsz / 2, -wsz / 2, wsz / 2, wsz / 2));
	}
}

void TerrainManager::ExtractEntitiesRecursive(std::string path, SceneNode* root, std::vector<Entity*> & outEntities)
{
	if (root->getAttachedObjects().size() > 0)
	{
		if (root->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
		{
			Entity* entity = (Entity*)root->getAttachedObject(0);
			Entity* treeEntity = entity->clone(entity->getName() + "_tree_Instance");

			for (int i = 0; i < treeEntity->getNumSubEntities(); ++i)
			{
				SubEntity* se = treeEntity->getSubEntity(i);
				se->setMaterial(se->getMaterial());
				se->setOriginalMaterial(se->getMaterial());
			}

			loadedEntities[path].push_back(treeEntity);

			outEntities.push_back(treeEntity);
		}
	}

	VectorIterator it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* child = (SceneNode*)it.getNext();

		ExtractEntitiesRecursive(path, child, outEntities);
	}
}

void TerrainManager::getTerrainImage(bool flipX, bool flipY, Ogre::Image & img)
{
	//img.load("Terrain/terrain.png", "Assets");

	/*if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();*/
}

std::string TerrainManager::defineTerrain(long x, long y, std::string filename)
{
	//Ogre::String filename = mTerrainGroup->generateFilename(x, y);

	bool exists = Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename);

	if (exists)
		mTerrainGroup->defineTerrain(x, y, filename);
	else
	{
		//Ogre::Image img;
		//getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		//mTerrainGroup->defineTerrain(x, y, &img);
		mTerrainGroup->defineTerrain(x, y, 0.0f);
	}

	return filename;
}

void TerrainManager::configureTerrainDefaults()
{
	TerrainMaterialGeneratorD * generator = new TerrainMaterialGeneratorD();

	mTerrainGlobals->setMaxPixelError(2);
	mTerrainGlobals->setCompositeMapDistance(3000);
	mTerrainGlobals->setDefaultMaterialGenerator(Ogre::TerrainMaterialGeneratorPtr(generator));
	mTerrainGlobals->setCastsDynamicShadows(true);
	mTerrainGlobals->setUseVertexCompressionWhenAvailable(false);
	//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);

	//mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(GetEngine->GetSceneManager()->getAmbientLight());
	//mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	Ogre::Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
	importData.terrainSize = 129;
	importData.worldSize = 3000.0;
	importData.inputScale = 600.0;
	importData.minBatchSize = 33;
	importData.maxBatchSize = 65;
}

float TerrainManager::getTerrainHeight(const float x, const float z, void* userData)
{
	return self->mTerrainGroup->getHeightAtWorldPosition(x, 0, z);
}