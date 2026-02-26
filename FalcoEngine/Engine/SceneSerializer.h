#pragma once

/*
-------Scene serializer class
-------Author: tracer0707

-------Provides functions for saving/loading scenes to/from file.
-------Uses boost serialization libraries.
*/

//Engine
#include "Engine.h"

//Boost headers
#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"

//Serialize members
#include "SScene.h"
#include "SVector.h"
#include "SNavMeshSettings.h"
#include "SQuaternion.h"
#include "SComponent.h"
#include "SAudioSource.h"
#include "SAudioListener.h"
#include "SRigidBody.h"
#include "SMeshCollider.h"
#include "SUIEvent.h"
#include "SSkeletonBone.h"
#include "SAnimationList.h"
#include "SNavMeshAgent.h"

using namespace std;
using namespace Ogre;

class AnimationList;

class SceneSerializer
{
public:
	SceneSerializer();
	~SceneSerializer();

	void Serialize(SceneManager* manager, string path);
	SScene SerializeToMemory(SceneManager* manager);

	void Deserialize(SceneManager* manager, SScene& scene, string path = "");
	void Deserialize(SceneManager* manager, string path);
	void DeserializeFromBuffer(SceneManager* manager, char * buffer, int bufSize, string path);

	void SerializeToPrefab(SceneNode* root, string path);
	SceneNode* DeserializeFromPrefab(SceneManager* manager, string path, bool onlyMeshes = false);

	void LoadAnimationAndSkinningData(SceneNode * node);
	void RestoreAnimations(AnimationList* animList, std::string animName = "");

private:
	static std::map<std::string, SScene> prefabCache;

	void SerializeChild(SceneNode* root, SScene* scene);
	void RestoreNodeRelationship(SceneManager* manager, string nodeName, string parentName, Vector3 position, Quaternion rotation, Vector3 scale);

	void SaveScripts(SceneNode* _node, SSceneNode& node);
	void SaveComponents(SceneNode* _node, SSceneNode& node);

	void SaveSceneData(SceneManager* manager, SScene& scene);
	void RestoreSceneData(SceneManager* manager, SScene& scene);

	void RestoreScripts(SceneNode* _node, SSceneNode& node, std::map<std::string, SceneNode*>& remapList);
	void RestoreComponents(SceneNode* _node, SSceneNode& node, std::map<std::string, SceneNode*>& remapList);

	void RestoreAnimationsRecursive(AnimationList* animList, SceneNode* root, std::string animName = "");

	void RestoreObjects(SScene& scene, SceneManager* manager, std::vector<std::pair<SceneNode*, SSceneNode>>& allNodes, std::map<std::string, SceneNode*>& remapList, bool onlyMeshes = false);

	std::string GenName(std::string srcName, SceneManager* manager);
	void GenIndexedPointers(AnimationList* animList, SceneNode* root);

	void ClearPrefabLinks(SceneNode* root);
	void GetAllSceneNodes(SceneNode* root, std::vector<SceneNode*> & outList);
};