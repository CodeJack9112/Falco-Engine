#pragma once

#include <OgreResourceManager.h>
#include <OgreLogManager.h>
#include <OgreVector3.h>
#include <OgreVector2.h>
#include <fbxsdk/core/math/fbxaffinematrix.h>

#include "AnimationList.h"

using namespace Ogre;

namespace fbxsdk
{
	class FbxImporter;
	class FbxNode;
	class FbxLayerElementUV;
	class FbxMesh;
	class FbxScene;
	class FbxAnimEvaluator;
	class FbxCluster;
}

using namespace fbxsdk;

struct BlendData
{
public:
	BlendData() {};

	double weight = 0.0;
	std::string boneName;
	int index = 0;
};

struct MeshData
{
public:
	MeshData() { }

	Vector3 vertice;
	Vector2 uv;
	Vector3 normal;
	int sourceIndex = 0;
	int index = 0;
	std::string materialName;
	std::vector<BlendData> blendData;
};

struct SubMeshInfo
{
public:
	bool animated = false;
	//bool fromCache = false;
	std::map<int, std::vector<MeshData>> subMeshInfo;
};

struct AnimationData
{
public:
	AnimationData() {}

	std::string name = "Default";
	std::string boneName;
	int length = 0;
	int frame = 0;
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
	bool skeletal = true;
};

struct ClusterData
{
public:
	ClusterData() {}
	
	double weight = 0;
	std::string jointname = "";
};

class SkeletonBone;
class AnimationList;
class FBXCache;

class ClusterMemory
{
public:
	std::map<int, std::pair<FbxCluster*, FbxMesh*>> clusters;
};

class FBXScene : public Resource
{
public:
	FBXScene(Ogre::ResourceManager *creator, const Ogre::String &name,
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		Ogre::ManualResourceLoader *loader = 0);

	virtual ~FBXScene();

	SceneNode * ConvertToNativeFormat(std::string name = "", int searchIndex = -1);
	void PostLoad(SceneNode * rootNode, SkeletonPtr skeleton = SkeletonPtr());

	void RemoveMeshesFromCache();
	void LoadCurves(SceneNode * sceneNode, int fbxIndex, AnimationList * list, AnimationList::AnimationData & anim);
	void LoadSkeletal(SceneNode * sceneNode, int fbxIndex, AnimationList * list, AnimationList::AnimationData & anim);

	void ImportKeyFrames(AnimationList * list, SceneNode * parent, SkeletonPtr skeleton, SceneNode * onlyThis = nullptr);
	void SetSkeleton(SceneNode * root, SkeletonPtr skeleton, Entity *& outEntity);

	//std::string GetFbxName() { return fbxName; }
	void ClearAnimationData();
	void SetSceneManager(SceneManager* mgr);
	SceneManager* GetSceneManager() { return sceneManager; }

	std::string getCachePath();
	void clearCache();
	void saveCache();
	void loadCache();

private:
	int index = -1;
	int boneIndex = -1;
	bool needSaveCache = false;
	bool indexesSaved = false;
	//std::string fbxName = "";
	std::string defName = "";
	SceneNode * root = nullptr;
	SceneManager * sceneManager = nullptr;
	FbxScene * lScene = nullptr;
	std::map<int, MeshPtr> allMeshes;
	std::map<std::string, SubMeshInfo> subMeshesList;
	std::vector<AnimationData> animationData;
	std::map<int, int> indexMap;
	std::map<FbxNode*, std::pair<int, std::vector<FbxNode*>>> childMap;
	AnimationList * animList;
	//std::map<std::string, FBXCache*> animCache;
	//std::map<std::string, FBXCache*> skelAnimCache;
	FBXCache * dataCache = nullptr;
	FBXCache * animationCache = nullptr;
	FBXCache * animationSkeletalCache = nullptr;
	std::map<int, std::pair<FbxCluster*, FbxMesh*>> clusters;

	void CalcChildCound(FbxNode* fbxNode);
	void CreateObjects(SceneNode * parent, FbxNode * fbxNode, int searchIndex = -1);
	void ImportSkeleton(SceneNode * parent, SkeletonPtr skeleton, Bone * rootBone, bool & hasAnim);
	Quaternion GetQuaternionRotation(FbxNode * fbxNode);
	Quaternion GetQuaternionRotation(FbxAMatrix mat);
	Quaternion GetQuaternionRotation(FbxVector4 vec);
	void ImportAnimationCurves(FbxNode * fbxNode, SceneNode * sceneNode, AnimationList * list, AnimationList::AnimationData & anim);
	void ImportAnimationCurvesKeys(FbxNode * fbxNode, SceneNode * sceneNode, SceneNode * root, std::string animName, int length, int startTime, int endTime);
	void CacheAnimationCurvesKeys(FbxNode * fbxNode, int length, int startTime, int endTime);
	void ImportAnimationSkeletal(FbxNode * fbxNode, SceneNode * sceneNode, AnimationList * list, AnimationList::AnimationData & anim);
	void ImportAnimationSkeletalKeys(FbxNode * fbxNode, FbxNode * pBoneNode, SceneNode * sceneNode, SceneNode * root, std::string animName, int length, int startTime, int endTime);
	void CacheAnimationSkeletalKeys(FbxNode* fbxNode, FbxCluster* cluster, int length, int startTime, int endTime);
	void AddAnimationComponent(SceneNode * node, std::string animName, int startTime, int endTime);
	FbxNode * GetByIndex(FbxNode * root, int index);
	void GetByIndexRecursive(FbxNode * root, int index, FbxNode *& outNode, int & outIndex);
	void GetIndexFromRootRecursive(SceneNode * root, SceneNode * need, int & index, int & outIndex);
	void GetNodeByIndexFromRootRecursive(SceneNode * root, SceneNode *& outNode, int index, int & outIndex);

protected:
	void loadImpl();
	void unloadImpl();
	size_t calculateSize() const;
	FbxImporter * fbxImporter = nullptr;
};

typedef SharedPtr<FBXScene> FBXScenePtr;