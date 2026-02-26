#include "stdafx.h"
#include "FBXScene.h"
#include "FBXSceneManager.h"
#include "Engine.h"
#include "IO.h"
#include <OgreMeshManager.h>
#include <OgreMesh.h>
#include <OgreSkeletonManager.h>
#include <OgreSkeleton.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreMaterialSerializer.h>
#include <OgreSubMesh.h>
#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreKeyFrame.h>

#include "../boost/algorithm/string.hpp"
#include "../boost/algorithm/string/replace.hpp"
#include "SkeletonBone.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include "StringConverter.h"
#include "../Ogre/source/OgreMain/include/OgreTagPoint.h"
#include "FBXCache.h"
#include "TrianglePacker/xatlas.h"
#include <OgreRoot.h>
#include <Engine\Procedural\ProceduralSphereGenerator.h>
#include "md5.h"
#include <Engine\ResourceMap.h>
#include "ZipHelper.h"
#include "GUIDGenerator.h"

int curPos = 0;

class BufferedFbx  : public FbxStream
{
public:
	BufferedFbx(FbxManager* pSdkManager)
	{
	}

	~BufferedFbx()
	{
		Close();
	}

public:
	char* buffer = nullptr;
	int size = 0;

	virtual EState GetState()
	{
		return buffer ? FbxStream::eOpen : eClosed;
	}

	virtual bool Open(void* /*pStreamData*/)
	{
		curPos = 0;
		return (buffer != NULL);
	}

	virtual bool Close()
	{
		//buffer = NULL;
		return true;
	}

	virtual bool Flush()
	{
		return true;
	}

	virtual int Write(const void* pData, int pSize)
	{
		return 0;
	}

	virtual int Read(void* pData, int pSize) const
	{
		if (buffer == NULL)
			return 0;

		memcpy(pData, reinterpret_cast<void*>(buffer + curPos), pSize);
		
		int read = pSize;

		if (curPos + pSize > size)
			read = size - curPos;

		curPos += read;

		return read;
	}

	virtual int GetReaderID() const
	{
		return 0;
	}

	virtual int GetWriterID() const
	{
		return 0;
	}

	void Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos)
	{
		switch (pSeekPos)
		{
		case FbxFile::eBegin:
			curPos = pOffset;
			break;
		case FbxFile::eCurrent:
			curPos += pOffset;
			break;
		case FbxFile::eEnd:
			curPos = size - pOffset;
			break;
		}
	}

	virtual long GetPosition() const
	{
		return curPos;
	}
	virtual void SetPosition(long pPosition)
	{
		curPos = pPosition;
	}

	virtual int GetError() const
	{
		return 0;
	}
	virtual void ClearError()
	{
		
	}
};

FBXScene::FBXScene(Ogre::ResourceManager * creator, const Ogre::String & name, Ogre::ResourceHandle handle, const Ogre::String & group, bool isManual, Ogre::ManualResourceLoader * loader) : Ogre::Resource(creator, name, handle, group, isManual, loader)
{
	createParamDictionary("FBXScene");
	sceneManager = GetEngine->GetSceneManager();
}

FBXScene::~FBXScene()
{
	if (lScene != nullptr)
	{
		lScene->Destroy(true);
		lScene = nullptr;
	}

	allMeshes.clear();

	unload();
}

void FBXScene::SetSceneManager(SceneManager* mgr)
{
	sceneManager = mgr;
}

std::string FBXScene::getCachePath()
{
	std::string fbxNameRepl = boost::replace_all_copy(getName(), "/", "_") + "_data";
	std::string cachePath = GetEngine->GetCachePath() + "Models/" + md5(fbxNameRepl) + ".cache";

	return cachePath;
}

void FBXScene::clearCache()
{
	string cachePath = getCachePath();
	if (IO::FileExists(cachePath))
		IO::FileDelete(cachePath);

	delete dataCache;
	delete animationCache;
	delete animationSkeletalCache;

	animationData.clear();
	subMeshesList.clear();
	indexMap.clear();
	clusters.clear();
	childMap.clear();
	allMeshes.clear();

	animList = nullptr;
	dataCache = nullptr;
	animationCache = nullptr;
	animationSkeletalCache = nullptr;

	indexesSaved = false;
	needSaveCache = false;

	if (lScene != nullptr)
	{
		lScene->Destroy(true);
		lScene = nullptr;
	}
}

void FBXScene::saveCache()
{
	if (needSaveCache)
	{
		std::wstring dir = ::StringConvert::s2ws(GetEngine->GetAssetsPath() + "Cache/", GetACP());
		std::wstring dir1 = ::StringConvert::s2ws(GetEngine->GetAssetsPath() + "Cache/Models/", GetACP());
		CreateDirectory(dir.c_str(), nullptr);
		CreateDirectory(dir1.c_str(), nullptr);

		std::string fbxNameRepl = boost::replace_all_copy(getName(), "/", "_") + "_data";
		std::string cachePath = GetEngine->GetCachePath() + "Models/" + md5(fbxNameRepl) + ".cache";
		dataCache->Save(cachePath);

		needSaveCache = false;
	}
}

void FBXScene::loadCache()
{
	if (dataCache == nullptr)
	{
		dataCache = new FBXCache();

		std::string fbxNameRepl = boost::replace_all_copy(getName(), "/", "_") + "_data";
		std::string cachePath = GetEngine->GetCachePath() + "Models/" + md5(fbxNameRepl) + ".cache";

		if (IO::FileExists(cachePath))
		{
			dataCache->Load(cachePath);
		}
	}
}

SceneNode * FBXScene::ConvertToNativeFormat(std::string name, int searchIndex)
{
	FbxManager * sdkManager = FBXSceneManager::getSingleton().GetSDKManager();

	if (lScene == nullptr)
	{
		fbxImporter = FbxImporter::Create(sdkManager, "");
		FbxIOSettings * ioSettings = FBXSceneManager::getSingleton().GetIOSettings();
		
		bool lImportStatus = false;

		BufferedFbx* buf = nullptr;
		char* buffer = nullptr;

		if (GetEngine->GetUseUnpackedResources())
		{
			lImportStatus = fbxImporter->Initialize(CP_UNI(mOrigin).c_str(), -1, ioSettings);
		}
		else
		{
			if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), getName()))
			{
				int sz = 0;
				buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), getName(), sz);

				buf = new BufferedFbx(sdkManager);
				buf->buffer = buffer;
				buf->size = sz;

				lImportStatus = fbxImporter->Initialize(buf, 0, -1, ioSettings);
			}
			else if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipBuiltin(), getName()))
			{
				int sz = 0;
				buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipBuiltin(), getName(), sz);

				buf = new BufferedFbx(sdkManager);
				buf->buffer = buffer;
				buf->size = sz;

				lImportStatus = fbxImporter->Initialize(buf, 0, -1, ioSettings);
			}
		}
		
		if (!lImportStatus)
		{
			LogManager::getSingleton().logError("FBX error: " + std::string(fbxImporter->GetStatus().GetErrorString()));
			fbxImporter->Destroy();

			if (!GetEngine->GetUseUnpackedResources())
			{
				if (buf != nullptr)
					delete buf;

				if (buffer != nullptr)
					delete[] buffer;
			}

			return nullptr;
		}

		lScene = FbxScene::Create(sdkManager, mName.c_str());

		// Import the contents of the file into the scene.
		fbxImporter->Import(lScene);
		fbxImporter->Destroy();

		bool optimize = false;
		FbxNode* _nd = nullptr;
		if (searchIndex == -1)
		{
			optimize = true;
		}
		else
		{
			int _idx = 0;
			
			GetByIndexRecursive(lScene->GetRootNode(), searchIndex, _nd, _idx);
			if (_nd != nullptr)
			{
				FbxMesh* m = _nd->GetMesh();
				if (m != nullptr)
				{
					optimize = true;
				}
			}
		}

		if (optimize)
		{
			FbxGeometryConverter clsConverter(sdkManager);
			clsConverter.RemoveBadPolygonsFromMeshes(lScene);
			clsConverter.Triangulate(lScene, true);
		}

		if (!GetEngine->GetUseUnpackedResources())
		{
			if (buf != nullptr)
				delete buf;

			if (buffer != nullptr)
				delete[] buffer;
		}
	}

	defName = name;
	root = nullptr;

	index = -1;
	boneIndex = 0;

	//animationData.clear();
	//subMeshesList.clear();

	if (!indexesSaved)
	{
		indexMap.clear();
		clusters.clear();
		childMap.clear();

		for (int ind = 0; ind < lScene->GetNodeCount(); ++ind)
		{
			indexMap[lScene->GetNode(ind)->GetUniqueID()] = ind;

			FbxMesh* pMesh = lScene->GetNode(ind)->GetMesh();

			if (pMesh != nullptr)
			{
				for (int deformerIndex = 0; deformerIndex < pMesh->GetDeformerCount(); ++deformerIndex)
				{
					FbxSkin* lSkinDeformer = reinterpret_cast<FbxSkin*>(pMesh->GetDeformer(deformerIndex/*, FbxDeformer::eSkin*/));
					if (!lSkinDeformer)
						continue;

					int lClusterCount = lSkinDeformer->GetClusterCount();
					for (int i = 0; i < lClusterCount; ++i)
					{
						FbxCluster* pCluster = lSkinDeformer->GetCluster(i);
						FbxNode* lnk = pCluster->GetLink();

						if (!lnk)
							continue;

						if (clusters.find(lnk->GetUniqueID()) == clusters.end())
							clusters[lnk->GetUniqueID()] = std::make_pair(pCluster, pMesh);
					}
				}
			}
		}

		CalcChildCound(lScene->GetRootNode());

		indexesSaved = true;
	}

	loadCache();

	std::string d = (IO::GetFilePath(mOrigin) + "Materials/");
	std::wstring dir = ::StringConvert::s2ws(d, GetACP());
	CreateDirectory(dir.c_str(), nullptr);

	CreateObjects(root, lScene->GetRootNode(), searchIndex);

	if (searchIndex == -1)
	{
		std::vector<SceneNode*> nstack;
		nstack.push_back(root);

		int _idx = -1;
		while (nstack.size() > 0)
		{
			SceneNode* curNode = *nstack.begin();
			nstack.erase(nstack.begin());

			++_idx;

			auto children = curNode->getChildren();
			int cnt = 0;
			for (auto i = children.begin(); i != children.end(); ++i)
			{
				SkeletonBone* bone = (SkeletonBone*)((SceneNode*)*i)->GetComponent(SkeletonBone::COMPONENT_TYPE);
				if (bone != nullptr)
				{
					if (bone->GetIsRootBone())
						++cnt;
				}
			}

			if (cnt > 1)
			{
				SkeletonBone* skeletonBone = new SkeletonBone(curNode);
				skeletonBone->SetIndex(_idx);
				skeletonBone->SetModelFileName(getName());
				skeletonBone->SetRootNodeName(root->getName());

				std::string _nm = to_string(_idx);
				skeletonBone->SetName(_nm);
				skeletonBone->SetIsRootBone(true);

				for (auto i = children.begin(); i != children.end(); ++i)
				{
					SkeletonBone* bone = (SkeletonBone*)((SceneNode*)*i)->GetComponent(SkeletonBone::COMPONENT_TYPE);
					if (bone != nullptr)
					{
						bone->SetIsRootBone(false);
					}
				}

				curNode->AddComponent(skeletonBone);

				break;
			}

			int j = 0;
			for (auto i = children.begin(); i != children.end(); ++i, ++j)
			{
				SceneNode* _nd = (SceneNode*)*i;
				nstack.insert(nstack.begin() + j, _nd);
			}
		}

		nstack.clear();
	}

	if (searchIndex > -1)
	{
		if (root == nullptr)
		{
			std::string nm = "";
			std::string al = "";

			if (!defName.empty())
			{
				nm = defName;
			}

			root = sceneManager->createSceneNode(nm);

			root->fbxFromFile = getName();
			root->fbxFromGuid = ResourceMap::guidMap[this];

			if (!al.empty())
				root->setAlias(al);

			Empty* empty = (Empty*)sceneManager->createMovableObject(root->getName(), EmptyObjectFactory::FACTORY_TYPE_NAME);
			root->attachObject(empty);
		}
	}

	if (searchIndex == -1)
	{
		PostLoad(root);
		ClearAnimationData();

		if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		{
			SkeletonPtr skeleton = SkeletonManager::getSingleton().getByName(getName() + "_skeleton", "Assets");
			Entity* _ent = nullptr;
			SetSkeleton(root, skeleton, _ent);
		}

		saveCache();
	}

	return root;
}

void FBXScene::PostLoad(SceneNode * rootNode, SkeletonPtr skeleton)
{
	root = rootNode;
	bool hasAnimation = false;

	//SkeletonPtr skeleton = SkeletonPtr();

	if (skeleton.getPointer() == nullptr)
	{
		if (SkeletonManager::getSingleton().resourceExists(getName() + "_skeleton", "Assets"))
		{
			skeleton = SkeletonManager::getSingleton().getByName(getName() + "_skeleton", "Assets");
			hasAnimation = skeleton->getNumAnimations() > 0;
		}
		else
		{
			skeleton = SkeletonManager::getSingleton().create(getName() + "_skeleton", "Assets", true);
			Bone * rootBone = skeleton->createBone("TheMainAndOnlyOneRootBone_563421116", 0);

			ImportSkeleton(rootNode, skeleton, rootBone, hasAnimation);
		}
	}
	else
	{
		hasAnimation = true;
	}

	//skeleton->load();
	animList = (AnimationList*)root->GetComponent(AnimationList::COMPONENT_TYPE);

	if (hasAnimation)
	{
		skeleton->setBindingPose();
	}
}

void FBXScene::RemoveMeshesFromCache()
{
	std::vector<int> to_remove;

	for (std::map<int, MeshPtr>::iterator it = allMeshes.begin(); it != allMeshes.end(); ++it)
	{
		MeshPtr mesh = it->second;

		bool inUse = false;

		MapIterator _it = sceneManager->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

		while (_it.hasMoreElements())
		{
			Entity * ent = (Entity*)_it.getNext();
			if (ent->getMesh() != nullptr)
			{
				if (ent->getMesh()->getOrigin() == mesh->getOrigin())
				{
					if (ent->getMesh()->fbxIndex == mesh->fbxIndex)
					{
						inUse = true;
						break;
					}
				}
			}
		}

		if (!inUse)
		{
			if (mesh != nullptr)
			{
				//MeshManager::getSingletonPtr()->unload(mesh->getHandle());
				MeshManager::getSingletonPtr()->remove(mesh);
			}

			to_remove.push_back(mesh->fbxIndex);
		}
	}

	for (std::vector<int>::iterator it = to_remove.begin(); it != to_remove.end(); ++it)
	{
		allMeshes.erase(allMeshes.find(*it));
	}

	to_remove.clear();
}

FbxAMatrix GetGeometry(FbxNode * fbxNode)
{
	FbxVector4 lT = fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 lR = fbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 lS = fbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxAMatrix lReferenceGeometry = FbxAMatrix(lT, lR, lS);

	return lReferenceGeometry;
}

FbxAMatrix GetGlobalPosition(FbxNode* pNode) {

	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();

	if (pNode->GetNodeAttribute())
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}

	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

	FbxAMatrix matrix;

	FbxNode* pParentNode = pNode->GetParent();
	if (pParentNode != nullptr)
	{
		FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();

		while ((pParentNode = pParentNode->GetParent()) != NULL)
		{
			parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
		}

		matrix = parentMatrix * localMatrix * matrixGeo;
	}
	else
	{
		matrix = localMatrix * matrixGeo;
	}

	return matrix;
}

void FBXScene::CalcChildCound(FbxNode* fbxNode)
{
	std::vector<FbxNode*> nstack;
	nstack.push_back(fbxNode);

	while (nstack.size() > 0)
	{
		FbxNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		int childCount = curNode->GetChildCount();
		childMap[curNode].first = childCount;

		////////////////////
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* _fbxNode = curNode->GetChild(i);
			childMap[curNode].second.push_back(_fbxNode);
			nstack.insert(nstack.begin() + i, _fbxNode);
		}
	}
}

struct ThreadStruct
{
public:
	ThreadStruct(
		std::map<int, std::map<int, std::pair<Vector3, int>>> * _verticesNormals,
		int _first,
		int _last,
		int _polyCount,
		float _cosineThreshold,
		std::vector<std::vector<int>> _vertexCache,
		std::vector<Vector3> _faceNormals
	)
	{
		verticesNormals = _verticesNormals;
		first = _first;
		last = _last;
		polyCount = _polyCount;
		cosineThreshold = _cosineThreshold;
		vertexCache = _vertexCache;
		faceNormals = _faceNormals;
	}

	~ThreadStruct()
	{
		vertexCache.clear();
		faceNormals.clear();
	}

	std::map<int, std::map<int, std::pair<Vector3, int>>> * verticesNormals;
	int first;
	int last;
	int polyCount;
	float cosineThreshold;
	std::vector<std::vector<int>> vertexCache;
	std::vector<Vector3> faceNormals;
};

void threadFunc(void *param)
{
	ThreadStruct _param = *(ThreadStruct*)(param);

	for (int lPolygonIndex = _param.first; lPolygonIndex < _param.last; ++lPolygonIndex)
	{
		int id0 = _param.vertexCache[lPolygonIndex][0];
		int id1 = _param.vertexCache[lPolygonIndex][1];
		int id2 = _param.vertexCache[lPolygonIndex][2];

		(*_param.verticesNormals)[lPolygonIndex][id0].first += _param.faceNormals[lPolygonIndex];
		(*_param.verticesNormals)[lPolygonIndex][id0].second += 1;
		(*_param.verticesNormals)[lPolygonIndex][id1].first += _param.faceNormals[lPolygonIndex];
		(*_param.verticesNormals)[lPolygonIndex][id1].second += 1;
		(*_param.verticesNormals)[lPolygonIndex][id2].first += _param.faceNormals[lPolygonIndex];
		(*_param.verticesNormals)[lPolygonIndex][id2].second += 1;

		for (int lPolygonIndex1 = 0; lPolygonIndex1 < _param.polyCount; ++lPolygonIndex1)
		{
			if (lPolygonIndex != lPolygonIndex1)
			{
				float dot = (_param.faceNormals)[lPolygonIndex].absDotProduct((_param.faceNormals)[lPolygonIndex1]);
				if (dot >= _param.cosineThreshold)
				{
					for (int j = 0; j < 3; ++j)
					{
						int id00 = _param.vertexCache[lPolygonIndex1][j];

						if (id0 == id00)
						{
							(*_param.verticesNormals)[lPolygonIndex][id0].first += _param.faceNormals[lPolygonIndex1];
							(*_param.verticesNormals)[lPolygonIndex][id0].second += 1;
						}

						if (id1 == id00)
						{
							(*_param.verticesNormals)[lPolygonIndex][id1].first += _param.faceNormals[lPolygonIndex1];
							(*_param.verticesNormals)[lPolygonIndex][id1].second += 1;
						}

						if (id2 == id00)
						{
							(*_param.verticesNormals)[lPolygonIndex][id2].first += _param.faceNormals[lPolygonIndex1];
							(*_param.verticesNormals)[lPolygonIndex][id2].second += 1;
						}
					}
				}
			}
		}
	}

	ExitThread(0);
}

void FBXScene::CreateObjects(SceneNode * parent, FbxNode * fbxNode, int searchIndex)
{
	std::vector<std::pair<SceneNode*, FbxNode*>> nstack;
	nstack.push_back(make_pair(parent, fbxNode));

	while (nstack.size() > 0)
	{
		SceneNode* curNode = nstack.begin()->first;
		FbxNode* curFbxNode = nstack.begin()->second;
		nstack.erase(nstack.begin());

		////////////////////
		index += 1;

		bool _continue = true;

		if (searchIndex > -1)
		{
			if (index != searchIndex)
			{
				_continue = false;
			}
		}

		SceneNode * node = nullptr;

		if (_continue)
		{
			string materialFolderPath = boost::replace_all_copy(IO::GetFilePath(mOrigin), GetEngine->GetAssetsPath(), "") + "Materials/";
			std::vector<std::string> existsMaterials;
			std::map<int, std::pair<FbxSurfaceMaterial*, string>> fbxMatCache;

			std::string nm = "";
			std::string al = "";

			if (!defName.empty())
			{
				if (searchIndex > -1)
				{
					nm = defName;
				}
				else
				{
					nm = curFbxNode->GetName();
					al = curFbxNode->GetName();

					if (nm.empty())
					{
						nm = "NoName";
						al = "NoName";
					}

					while (sceneManager->hasSceneNode(nm))
					{
						nm = curFbxNode->GetName();
						if (nm.empty())
							nm = "NoName";
					
						nm = nm + "_" + GetEngine->GenerateNewName(sceneManager);
					}
				}
			}
			else
			{
				nm = curFbxNode->GetName();
				al = curFbxNode->GetName();

				if (nm.empty())
				{
					nm = "NoName";
					al = "NoName";
				}

				while (sceneManager->hasSceneNode(nm))
				{
					nm = curFbxNode->GetName();
					if (nm.empty())
						nm = "NoName";

					nm = nm + "_" + GetEngine->GenerateNewName(sceneManager);
				}
			}

			if (curNode != nullptr)
				node = curNode->createChildSceneNode(nm);
			else
				node = sceneManager->createSceneNode(nm);

			node->fbxFromFile = getName();
			node->fbxFromGuid = ResourceMap::guidMap[this];

			if (!al.empty())
				node->setAlias(al);

			if (root == nullptr)
				root = node;

			MeshPtr _mesh = MeshPtr();

			//Check cache
			if (allMeshes.find(index) != allMeshes.end())
			{
				_mesh = allMeshes[index];
			}

			FbxMesh * fbxMesh = curFbxNode->GetMesh();
			FbxSkeleton * skeleton = curFbxNode->GetSkeleton();
		
			SkeletonBone * skeletonBone = nullptr;
		
			//Build skeleton
			if (skeleton != nullptr && searchIndex == -1)
			{
				root->skinned = true;

				bool binded = false;

				skeletonBone = new SkeletonBone(node);
				skeletonBone->SetIndex(index);
				skeletonBone->SetModelFileName(getName());
				skeletonBone->SetRootNodeName(root->getName());
			
				int nodesCount = lScene->GetNodeCount();
				FbxAMatrix translation = GetGlobalPosition(curFbxNode);

				std::string _nm = to_string(indexMap[curFbxNode->GetUniqueID()]);
				skeletonBone->SetName(_nm);

				if (clusters.find(curFbxNode->GetUniqueID()) != clusters.end())
				{
					FbxCluster* pCluster = clusters[curFbxNode->GetUniqueID()].first;
					FbxMesh* pMesh = clusters[curFbxNode->GetUniqueID()].second;

					FbxAMatrix transformLinkMatrix;

					pCluster->GetTransformLinkMatrix(transformLinkMatrix);

					//Binding position
					FbxAMatrix lClusterInitPosition = transformLinkMatrix;

					Vector3 pos = Vector3(lClusterInitPosition.GetT().mData[0], lClusterInitPosition.GetT().mData[1], lClusterInitPosition.GetT().mData[2]);
					Quaternion rot = Quaternion(lClusterInitPosition.GetQ().mData[3], lClusterInitPosition.GetQ().mData[0], lClusterInitPosition.GetQ().mData[1], lClusterInitPosition.GetQ().mData[2]);
					Vector3 scl = Vector3(lClusterInitPosition.GetS().mData[0], lClusterInitPosition.GetS().mData[1], lClusterInitPosition.GetS().mData[2]);

					skeletonBone->SetBindingPosition(pos);
					skeletonBone->SetBindingRotation(rot);
					skeletonBone->SetBindingScale(scl);

					skeletonBone->SetPosition(pos);
					skeletonBone->SetRotation(rot);
					skeletonBone->SetScale(scl);
				
					if (skeleton->IsSkeletonRoot())
						skeletonBone->SetIsRootBone(true);

					binded = true;
				}

				if (!binded)
				{
					skeletonBone->SetBindingPosition(Vector3(translation.GetT().mData[0], translation.GetT().mData[1], translation.GetT().mData[2]));
					skeletonBone->SetBindingRotation(Quaternion(translation.GetQ().mData[3], translation.GetQ().mData[0], translation.GetQ().mData[1], translation.GetQ().mData[2]));
					skeletonBone->SetBindingScale(Vector3(translation.GetS().mData[0], translation.GetS().mData[1], translation.GetS().mData[2]));

					skeletonBone->SetPosition(Vector3(translation.GetT().mData[0], translation.GetT().mData[1], translation.GetT().mData[2]));
					skeletonBone->SetRotation(Quaternion(translation.GetQ().mData[3], translation.GetQ().mData[0], translation.GetQ().mData[1], translation.GetQ().mData[2]));
					skeletonBone->SetScale(Vector3(translation.GetS().mData[0], translation.GetS().mData[1], translation.GetS().mData[2]));
				}

				node->components.push_back(skeletonBone);
			}

			FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(lScene->GetSrcObject<FbxAnimStack>());
			if (pAnimStack != nullptr)
			{
				if (searchIndex == -1)
				{
					if (node == root)
					{
						FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>();

						FbxTakeInfo* takeinfo = curFbxNode->GetScene()->GetTakeInfo(pAnimStack->GetName());
						if (takeinfo != nullptr)
						{
							fbxsdk::FbxTime start = takeinfo->mLocalTimeSpan.GetStart();
							fbxsdk::FbxTime end = takeinfo->mLocalTimeSpan.GetStop();

							AddAnimationComponent(node, pAnimStack->GetName(), start.GetFrameCount(fbxsdk::FbxTime::eFrames30), end.GetFrameCount(fbxsdk::FbxTime::eFrames30));
							animList = (AnimationList*)root->GetComponent(AnimationList::COMPONENT_TYPE);
						}
					}
				}

				if (animList != nullptr)
				{
					node->fbxCurvesFrom = index;
					node->fbxCurvesFromFile = getName();
					node->fbxCurvesFromGuid = ResourceMap::guidMap[this];
				}
			}

			//Build mesh
			if (fbxMesh != nullptr)
			{
				FbxVector4 * controlPoints = fbxMesh->GetControlPoints();
				int controlPointsCount = fbxMesh->GetControlPointsCount();

				FbxStringList lUVNames;
				fbxMesh->GetUVSetNames(lUVNames);
				const char * lUVName = NULL;
				if (lUVNames.GetCount())
				{
					lUVName = lUVNames[0];
				}

				int upSign = 1;
				int frontSign = 1;
				FbxAxisSystem::EUpVector upVec = lScene->GetGlobalSettings().GetAxisSystem().GetUpVector(upSign);
				FbxAxisSystem::EFrontVector frontVec = lScene->GetGlobalSettings().GetAxisSystem().GetFrontVector(frontSign);
			
				if (controlPoints != nullptr)
				{
					std::map<int, std::vector<MeshData>> subMeshes;
					std::map<int, std::vector<BlendData>> blendData;

					int polyCount = fbxMesh->GetPolygonCount();
					int * vertices = fbxMesh->GetPolygonVertices();

					bool animated = false;
					int ind = indexMap[curFbxNode->GetUniqueID()];

					std::vector<FBXDataNode>::iterator srch = find_if(dataCache->dataCache.begin(), dataCache->dataCache.end(), [ind](const FBXDataNode & _c) -> bool
					{
						return _c.uid == ind;
					});

					bool cacheLoaded = true;

					if (srch == dataCache->dataCache.end())
					{
						FBXDataNode dataNode;
						dataNode.uid = ind;
						dataCache->dataCache.push_back(dataNode);
						cacheLoaded = false;
					}

					srch = find_if(dataCache->dataCache.begin(), dataCache->dataCache.end(), [ind](const FBXDataNode & _c) -> bool
					{
						return _c.uid == ind;
					});

					//Extract materials and build submesh list
					FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
					FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;

					if (_mesh == nullptr)
					{
						if (fbxMesh->GetElementMaterial())
						{
							lMaterialIndice = &fbxMesh->GetElementMaterial()->GetIndexArray();
							lMaterialMappingMode = fbxMesh->GetElementMaterial()->GetMappingMode();

							FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
						}

						std::map<int, std::vector<ClusterData>> clusterData;

						//Blend weights
						if (!cacheLoaded)
						{
							//Store blend weights
							for (int deformerIndex = 0; deformerIndex < fbxMesh->GetDeformerCount(); ++deformerIndex)
							{
								FbxSkin* skin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex/*, FbxDeformer::eSkin*/));
								if (!skin)
									continue;

								for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); ++clusterIndex)
								{
									FbxCluster* cluster = skin->GetCluster(clusterIndex);
									FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();

									if (!cluster->GetLink())
										continue;

									int count = cluster->GetControlPointIndicesCount();

									for (int i = 0; i < count; ++i)
									{
										int vertexid = cluster->GetControlPointIndices()[i];
										double weight = cluster->GetControlPointWeights()[i];

										if (vertexid >= controlPointsCount)
											continue;

										//if (weight == 0.0)
										//	continue;

										std::string _nm = to_string(indexMap[cluster->GetLink()->GetUniqueID()]);
										std::string jointname = _nm;

										ClusterData dt;
										dt.jointname = jointname;
										dt.weight = weight;

										clusterData[vertexid].push_back(dt);

										FBXBlendWeightsCache blendCache;
										blendCache.jointname = jointname;
										blendCache.vertexid = vertexid;
										blendCache.weight = weight;

										srch->blendWeightsCacheData.push_back(blendCache);
									}
								}
							}
						}
						else
						{
							//Load blend weights from cache
							for (std::vector<FBXBlendWeightsCache>::iterator bcit = srch->blendWeightsCacheData.begin(); bcit != srch->blendWeightsCacheData.end(); ++bcit)
							{
								ClusterData dt;
								dt.jointname = bcit->jointname;
								dt.weight = bcit->weight;

								clusterData[bcit->vertexid].push_back(dt);
							}
						}

						//Rotation matrix
						FbxAMatrix lReferenceGeometry = GetGeometry(curFbxNode);
						FbxAMatrix lReference = lScene->GetAnimationEvaluator()->GetNodeGlobalTransform(curFbxNode) * lReferenceGeometry;

						Matrix4 mat;
						mat.makeTransform(
							Vector3(lReference.GetT().mData[0], lReference.GetT().mData[1], lReference.GetT().mData[2]),
							Vector3(lReference.GetS().mData[0], lReference.GetS().mData[1], lReference.GetS().mData[2]),
							Quaternion(lReference.GetQ().mData[3], lReference.GetQ().mData[0], lReference.GetQ().mData[1], lReference.GetQ().mData[2]));

						std::vector<std::vector<int>> vertexCache;
						vertexCache.resize(polyCount);
						std::vector<Vector3> faceNormals;
						faceNormals.resize(polyCount);

						std::map<int, std::map<int, std::pair<Vector3, int>>> verticesNormals;
					
						for (int lPolygonIndex = 0; lPolygonIndex < polyCount; ++lPolygonIndex)
						{
							vertexCache[lPolygonIndex].resize(3);
							vertexCache[lPolygonIndex][0] = fbxMesh->GetPolygonVertex(lPolygonIndex, 0);
							vertexCache[lPolygonIndex][1] = fbxMesh->GetPolygonVertex(lPolygonIndex, 1);
							vertexCache[lPolygonIndex][2] = fbxMesh->GetPolygonVertex(lPolygonIndex, 2);
						}

						if (!cacheLoaded)
						{
							/* Calculate normals */
							for (int lPolygonIndex = 0; lPolygonIndex < polyCount; ++lPolygonIndex)
							{
								int id0 = vertexCache[lPolygonIndex][0];
								int id1 = vertexCache[lPolygonIndex][1];
								int id2 = vertexCache[lPolygonIndex][2];

								FbxVector4 vec1 = controlPoints[id0];
								FbxVector4 vec2 = controlPoints[id1];
								FbxVector4 vec3 = controlPoints[id2];

								Vector3 v0 = (Vector3(vec1.mData[0], vec1.mData[1], vec1.mData[2]));
								Vector3 v1 = (Vector3(vec2.mData[0], vec2.mData[1], vec2.mData[2]));
								Vector3 v2 = (Vector3(vec3.mData[0], vec3.mData[1], vec3.mData[2]));

								if (fbxMesh->GetDeformerCount() > 0)
								{
									v0 = mat * v0;
									v1 = mat * v1;
									v2 = mat * v2;
								}

								Vector3 normalA = (v1 - v0).crossProduct(v2 - v0).normalisedCopy();

								faceNormals[lPolygonIndex] = normalA;
							}

							float cosineThreshold = cos(Math::DegreesToRadians(60.0f));

							for (int lPolygonIndex = 0; lPolygonIndex < polyCount; ++lPolygonIndex)
							{
								for (int k = 0; k < 3; ++k)
								{
									int id0 = vertexCache[lPolygonIndex][k];

									verticesNormals[lPolygonIndex][id0].first = Vector3::ZERO;
									verticesNormals[lPolygonIndex][id0].second = 0;
								}
							}

							SYSTEM_INFO sysinfo;
							GetSystemInfo(&sysinfo);
							const int numCPU = sysinfo.dwNumberOfProcessors;

							std::vector<HANDLE> threads;
							threads.resize(numCPU);

							int slice = floor(polyCount / numCPU);
							std::vector<ThreadStruct*> threadStructs;

							for (int i = 0; i < numCPU; ++i)
							{
								int start = slice * i;
								int end = start + slice;
							
								if (i == numCPU - 1)
									end = polyCount;

								ThreadStruct * ts = new ThreadStruct(&verticesNormals, start, end, polyCount, cosineThreshold, vertexCache, faceNormals);
								threadStructs.push_back(ts);

								threads[i] = (HANDLE)_beginthread(threadFunc, 0, (void*)(ts));
							}

							WaitForMultipleObjects(numCPU, &threads[0], TRUE, INFINITE);

							threads.clear();

							for (auto it = threadStructs.begin(); it != threadStructs.end(); ++it)
								delete* it;

							threadStructs.clear();

							for (int lPolygonIndex = 0; lPolygonIndex < polyCount; ++lPolygonIndex)
							{
								for (int k = 0; k < 3; ++k)
								{
									int id0 = vertexCache[lPolygonIndex][k];
									verticesNormals[lPolygonIndex][id0].first /= (float)verticesNormals[lPolygonIndex][id0].second;
									verticesNormals[lPolygonIndex][id0].first.normalise();
								}
							}
						}

						//Geometry
						for (int lPolygonIndex = 0; lPolygonIndex < polyCount; ++lPolygonIndex)
						{
							int lMaterialIndex = 0;
							
							if (fbxMesh->GetElementMaterial())
								lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);

							//Materials
							FbxSurfaceMaterial* material = nullptr;
							string mat_Name = "";

							if (fbxMatCache.find(lMaterialIndex) != fbxMatCache.end())
							{
								material = fbxMatCache[lMaterialIndex].first;
								mat_Name = fbxMatCache[lMaterialIndex].second;
							}
							else
							{
								material = curFbxNode->GetMaterial(lMaterialIndex);
								if (material != nullptr)
								{
									mat_Name = material->GetName();
									mat_Name = boost::replace_all_copy(mat_Name, "\\", "_");
									mat_Name = boost::replace_all_copy(mat_Name, "/", "_");
									mat_Name = boost::to_lower_copy(mat_Name);

									fbxMatCache[lMaterialIndex] = make_pair(material, mat_Name);
								}
							}

							if (material != nullptr)
							{
								if (GetEngine->GetUseUnpackedResources())
								{
									string mat_FullName = materialFolderPath + mat_Name + ".material";

									if (!MaterialManager::getSingleton().resourceExists(mat_FullName, "Assets"))
									{
										if (find(existsMaterials.begin(), existsMaterials.end(), mat_FullName) == existsMaterials.end())
										{
											if (mat_FullName.find(GetEngine->GetBuiltinResourcesPath()) == string::npos)
											{
												existsMaterials.push_back(mat_FullName);

												if (!IO::FileExists(GetEngine->GetAssetsPath() + mat_FullName))
												{
													MaterialPtr newMaterial = AssetsTools::CreateMaterial(mat_Name, materialFolderPath);

													if (newMaterial != nullptr)
													{
														FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

														// Check if it's layeredtextures
														int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();
														std::string textureName = "";

														if (layeredTextureCount > 0)
														{
															for (int j = 0; j < layeredTextureCount; j++)
															{
																FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
																int lcount = layered_texture->GetSrcObjectCount<FbxFileTexture>();

																for (int k = 0; k < lcount; k++)
																{
																	FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxFileTexture>(k));
																	textureName = IO::GetFileNameWithExt(texture->GetFileName());
																}
															}
														}
														else
														{
															// Directly get textures
															int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
															for (int j = 0; j < textureCount; j++)
															{
																FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxFileTexture>(j));
																textureName = IO::GetFileNameWithExt(texture->GetFileName());
															}
														}

														if (!textureName.empty())
														{
															IO::listFiles(GetEngine->GetAssetsPath(), true, nullptr, [=](std::string d, std::string f)
																{
																	std::string lfname = boost::algorithm::to_lower_copy(f);
																	std::string ltexname = boost::algorithm::to_lower_copy(textureName);

																	if (IO::GetFileName(lfname) == IO::GetFileName(ltexname))
																	{
																		std::string ext = IO::GetFileExtension(lfname);

																		if (ext == "jpg" || ext == "png" || ext == "jpeg" || ext == "bmp" || ext == "psd" || ext == "tga" || ext == "gif" || ext == "pic" || ext == "ppm" || ext == "pgm" || ext == "hdr" || ext == "dds")
																		{
																			for (int ti = 0; ti < newMaterial->getTechnique(0)->getNumPasses(); ++ti)
																			{
																				std::string tPath = d + f;
																				tPath = boost::replace_all_copy(tPath, GetEngine->GetAssetsPath(), "");

																				Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().load(tPath, "Assets");

																				Pass* pass = newMaterial->getTechnique(0)->getPass(ti);
																				TextureUnitState* texUnit = pass->getTextureUnitState("diffuseMap");
																				if (texUnit != nullptr)
																				{
																					texUnit->setTexture(tex);
																				}
																			}
																		}
																	}
																});

															MaterialSerializer serializer;
															serializer.exportMaterial(newMaterial, GetEngine->GetAssetsPath() + newMaterial->getOrigin(), false, false);
														}
													}
												}
											}
										}
									}
								}
							}

							int count = fbxMesh->GetPolygonSize(lPolygonIndex);
	
							for (int k = 0; k < count; ++k)
							{
								int vertexID = vertexCache[lPolygonIndex][k];

								if (vertexID < 0)
									continue;

								FbxVector2 vecUV;
								//FbxVector4 normalUV;
								bool unmapped;
								int uv = fbxMesh->GetPolygonVertexUV(lPolygonIndex, k, lUVName, vecUV, unmapped);
								//bool hasNormal = fbxMesh->GetPolygonVertexNormal(lPolygonIndex, k, normalUV);

								Vector3 _p = Vector3(controlPoints[vertexID].mData[0]/* * _sm.x*/, controlPoints[vertexID].mData[1]/* * _sm.y*/, controlPoints[vertexID].mData[2]/* * _sm.z*/);

								if (fbxMesh->GetDeformerCount() > 0)
									_p = (mat * _p);

								Vector3 outNormal = verticesNormals[lPolygonIndex][vertexID].first;

								MeshData data;
								data.vertice = _p;
								data.uv = Vector2(vecUV.mData[0], 1.0 - vecUV.mData[1]);
								data.normal = outNormal;
								data.sourceIndex = vertexID;

								if (material != nullptr)
								{
									data.materialName = materialFolderPath + mat_Name + ".material";
								}

								if (clusterData[vertexID].size() > 0)
									animated = true;

								for (std::vector<ClusterData>::iterator bb = clusterData[vertexID].begin(); bb != clusterData[vertexID].end(); ++bb)
								{
									BlendData bdata;
									bdata.weight = bb->weight;
									bdata.boneName = bb->jointname;
									data.blendData.push_back(bdata);
								}

								subMeshes[lMaterialIndex].push_back(data);
							}
						}

						vertexCache.clear();
						faceNormals.clear();
						verticesNormals.clear();

						//Animation
						if (searchIndex == -1)
						{
							if (animList != nullptr)
								ImportAnimationSkeletal(curFbxNode, node, animList, animList->GetAnimationDataAt(0));
						}

						//Setup indexes
						for (std::map<int, std::vector<MeshData>>::iterator & dt = subMeshes.begin(); dt != subMeshes.end(); ++dt)
						{
							int j = 0;

							for (std::vector<MeshData>::iterator & md = dt->second.begin(); md != dt->second.end(); ++md)
							{
								(&(*md))->index = j;

								for (std::vector<BlendData>::iterator & bd = md->blendData.begin(); bd != md->blendData.end(); ++bd)
								{
									(&(*bd))->index = j;
								}

								j += 1;
							}
						}

						if (fbxMesh->GetDeformerCount() > 0)
						{
							animated = true;
							node->fbxSkeletonFrom = index;
							node->fbxSkeletonFromFile = getName();
							node->fbxSkeletonFromGuid = ResourceMap::guidMap[this];
						}

						subMeshesList[getName() + "_" + to_string(index)].animated = animated;
						subMeshesList[getName() + "_" + to_string(index)].subMeshInfo = subMeshes;
					}

					//---------

					if (_mesh == nullptr)
					{
						MeshPtr mesh = MeshPtr();

						if (!animated)
						{
							string _meshName = node->getName() + GUIDGenerator::genGuid();

							while (MeshManager::getSingleton().resourceExists(_meshName, "Assets"))
							{
								//mesh = MeshManager::getSingleton().getByName(node->getName(), "Assets");
								_meshName = node->getName() + GetEngine->GenerateNewName(sceneManager);
							}
						
							mesh = MeshManager::getSingleton().createManual(_meshName, "Assets");
						}
						else
						{
							string _meshName = node->getName() + GUIDGenerator::genGuid();

							while (MeshManager::getSingleton().resourceExists(_meshName, "Assets"))
							{
								//mesh = MeshManager::getSingleton().getByName(node->getName() + "_tempMesh", "Assets");
								_meshName = node->getName() + GetEngine->GenerateNewName(sceneManager);
							}
						
							mesh = MeshManager::getSingleton().createManual(_meshName + "_tempMesh", "Assets");
						}

						mesh->fbxIndex = index;
						mesh->isFbx = true;
						mesh->_notifyOrigin(getName());
						ResourceMap::guidMap[mesh.getPointer()] = ResourceMap::guidMap[this];
				
						int n = 0;

						for (std::map<int, std::vector<MeshData>>::iterator & it = subMeshes.begin(); it != subMeshes.end(); ++it)
						{
							FbxSurfaceMaterial * material = curFbxNode->GetMaterial(it->first);

							FBXNormalCache _ncache;
							FBXLightmapUVCache _uvcache;

							if (cacheLoaded)
							{
								_ncache = srch->normalCacheData.at(n);
								_uvcache = srch->lightmapUVCache.at(n);
							}

							//Construct mesh
							int sz = it->second.size() * 3/* + it->second.size() * 3*/;
							int sz2 = it->second.size() * 2;
							int indexSize = it->second.size();
							int bufSize = 3;
							int bufSize2 = 2;

							float * vertices = new float[sz];
							float * normals = new float[sz];
							float * texCoords = new float[sz2];
							float * texCoords2 = new float[sz2];
							uint16 * faces = new uint16[indexSize];

							int verticesCount = it->second.size();

							int i = 0;
							int j = 0;
							int _i = 0;
							int _t = 0;

							for (std::vector<MeshData>::iterator kk = it->second.begin(); kk != it->second.end(); ++kk, _i += 3, _t += 2)
							{
								//vertex
								vertices[i] = kk->vertice.x;
								vertices[i + 1] = kk->vertice.y;
								vertices[i + 2] = kk->vertice.z;

								if (!cacheLoaded)
								{
									//normal
									normals[i] = kk->normal.x;
									normals[i + 1] = kk->normal.y;
									normals[i + 2] = kk->normal.z;

									_ncache.data.push_back(SVector3(kk->normal.x, kk->normal.y, kk->normal.z));
								}
								else
								{
									normals[i] = _ncache.data.at(j).x;
									normals[i + 1] = _ncache.data.at(j).y;
									normals[i + 2] = _ncache.data.at(j).z;
								}

								//uv
								texCoords[_t] = kk->uv.x;
								texCoords[_t + 1] = kk->uv.y;

								//index
								faces[j] = j;

								i += bufSize;
								j += 1;
							}

							if (!cacheLoaded)
							{
								xatlas::Atlas * atlas = xatlas::Create();
								xatlas::MeshDecl mesh;
								mesh.indexCount = indexSize;
								mesh.indexFormat = xatlas::IndexFormat::UInt16;
								mesh.indexData = faces;

								mesh.vertexCount = verticesCount;
								mesh.vertexPositionData = vertices;
								mesh.vertexPositionStride = sizeof(float) * 3;
								mesh.vertexNormalData = normals;
								mesh.vertexNormalStride = sizeof(float) * 3;

								mesh.vertexUvData = texCoords;
								mesh.vertexUvStride = sizeof(float) * 2;

								xatlas::AddMesh(atlas, mesh);

								xatlas::Generate(atlas);

								if (atlas->meshCount > 0)
								{
									xatlas::Mesh output_mesh = atlas->meshes[0];

									for (int j = 0, jj = 0; j < output_mesh.indexCount; ++j, jj += 2)
									{
										int ind = output_mesh.indexArray[j];
										float x = output_mesh.vertexArray[ind].uv[0] / atlas->width;
										float y = output_mesh.vertexArray[ind].uv[1] / atlas->height;

										texCoords2[jj] = x;
										texCoords2[jj + 1] = y;

										_uvcache.data.push_back(SVector2(x, y));
									}
								}

								xatlas::Destroy(atlas);
							}
							else
							{
								int jj = 0;
								for (int j = 0; j < _uvcache.data.size() * 2; j += 2, ++jj)
								{
									texCoords2[j] = _uvcache.data[jj].x;
									texCoords2[j + 1] = _uvcache.data[jj].y;
								}
							}

							SubMesh* sub = mesh->createSubMesh();
							sub->useSharedVertices = false;
							sub->vertexData = new VertexData();
							sub->vertexData->vertexCount = verticesCount;

							//Vertices
							size_t offset = 0;
							sub->vertexData->vertexDeclaration->addElement(0, offset, VET_FLOAT3, VES_POSITION);
							offset += VertexElement::getTypeSize(VET_FLOAT3);

							HardwareVertexBufferSharedPtr vbuf0 = HardwareBufferManager::getSingleton().createVertexBuffer(offset, verticesCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
							vbuf0->writeData(0, vbuf0->getSizeInBytes(), vertices, true);

							//Normals
							offset = 0;
							sub->vertexData->vertexDeclaration->addElement(1, offset, VET_FLOAT3, VES_NORMAL);
							offset += VertexElement::getTypeSize(VET_FLOAT3);

							HardwareVertexBufferSharedPtr vbuf1 = HardwareBufferManager::getSingleton().createVertexBuffer(offset, verticesCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
							vbuf1->writeData(0, vbuf1->getSizeInBytes(), normals, true);

							//Tex coords 1
							offset = 0;
							sub->vertexData->vertexDeclaration->addElement(2, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
							offset += VertexElement::getTypeSize(VET_FLOAT2);

							HardwareVertexBufferSharedPtr vbuf2 = HardwareBufferManager::getSingleton().createVertexBuffer(offset, verticesCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
							vbuf2->writeData(0, vbuf2->getSizeInBytes(), texCoords, true);

							//Tex coords 2
							offset = 0;
							sub->vertexData->vertexDeclaration->addElement(3, 0, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
							offset += VertexElement::getTypeSize(VET_FLOAT2);

							HardwareVertexBufferSharedPtr vbuf3 = HardwareBufferManager::getSingleton().createVertexBuffer(offset, verticesCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
							vbuf3->writeData(0, vbuf3->getSizeInBytes(), texCoords2, true);

							HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT, indexSize, HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
							ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);

							sub->vertexData->vertexBufferBinding->setBinding(0, vbuf0);
							sub->vertexData->vertexBufferBinding->setBinding(1, vbuf1);
							sub->vertexData->vertexBufferBinding->setBinding(2, vbuf2);
							sub->vertexData->vertexBufferBinding->setBinding(3, vbuf3);
							sub->indexData->indexBuffer = ibuf;
							sub->indexData->indexStart = 0;
							sub->indexData->indexCount = indexSize;

							delete[] vertices;
							delete[] normals;
							delete[] texCoords;
							delete[] texCoords2;
							delete[] faces;

							if (material != nullptr)
							{
								string mat_Name = material->GetName();
								mat_Name = boost::replace_all_copy(mat_Name, "\\", "_");
								mat_Name = boost::replace_all_copy(mat_Name, "/", "_");
								mat_Name = boost::to_lower_copy(mat_Name);
								string materialName = materialFolderPath + mat_Name + ".material";

								if (!MaterialManager::getSingleton().resourceExists(materialName, "Assets"))
									materialName = mat_Name;

								sub->setMaterialName(materialName);
							}
							else
								sub->setMaterialName("StandardMaterial", "Assets");

							if (!cacheLoaded)
							{
								srch->normalCacheData.push_back(_ncache);
								srch->lightmapUVCache.push_back(_uvcache);
							}

							++n;
						}

						if (!cacheLoaded)
						{
							/*if (srch->normalCacheData.size() > 0)
							{
							
							}*/

							needSaveCache = true;
						}

						if (subMeshes.size() > 0)
						{
							allMeshes[index] = mesh;
							mesh->load();

							unsigned short coord = 0;
							unsigned short index = 0;

							if (!mesh->suggestTangentVectorBuildParams(VertexElementSemantic::VES_TANGENT, coord, index))
							{
								mesh->buildTangentVectors(VertexElementSemantic::VES_TANGENT, coord, index);
							}

							mesh->_updateBoundsFromVertexBuffers(false);

							Entity * entity = nullptr;

							if (!animated)
								entity = sceneManager->createEntity(node->getName(), mesh);
							else
								entity = sceneManager->createEntity(node->getName() + "_tempEntity", mesh);

							entity->setQueryFlags(1 << 1);
							node->attachObject(entity);
						}
						else
						{
							MeshManager::getSingletonPtr()->unload(mesh->getName(), "Assets");
							MeshManager::getSingletonPtr()->remove(mesh->getName(), "Assets");

							Empty * empty = (Empty*)sceneManager->createMovableObject(node->getName(), EmptyObjectFactory::FACTORY_TYPE_NAME);
							node->attachObject(empty);
						}
					}
					else
					{
						Entity * entity = nullptr;

						if (fbxMesh->GetDeformerCount() > 0)
						{
							animated = true;
							node->fbxSkeletonFrom = index;
							node->fbxSkeletonFromFile = getName();
							node->fbxSkeletonFromGuid = ResourceMap::guidMap[this];
						}

						if (animated)
							entity = sceneManager->createEntity(node->getName() + "_tempEntity", _mesh);
						else
							entity = sceneManager->createEntity(node->getName(), _mesh);

						entity->setQueryFlags(1 << 1);
						node->attachObject(entity);
					}
				}
				else
				{
					Empty * empty = (Empty*)sceneManager->createMovableObject(node->getName(), EmptyObjectFactory::FACTORY_TYPE_NAME);
					node->attachObject(empty);
				}
			}
			else
			{
				Empty * empty = (Empty*)sceneManager->createMovableObject(node->getName(), EmptyObjectFactory::FACTORY_TYPE_NAME);
				node->attachObject(empty);
			}

			if (searchIndex == -1)
			{
				if (animList != nullptr)
					ImportAnimationCurves(curFbxNode, node, animList, animList->GetAnimationDataAt(0));
			}

			//Node position
			if (node != nullptr)
			{
				if (skeletonBone == nullptr)
				{
					FbxAMatrix _transform = lScene->GetAnimationEvaluator()->GetNodeLocalTransform(curFbxNode) * GetGeometry(curFbxNode);

					if (fbxMesh != nullptr && fbxMesh->GetDeformerCount() > 0)
					{
						node->_setDerivedPosition(Vector3(0, 0, 0));
						node->_setDerivedOrientation(Quaternion::IDENTITY);
						node->setInheritScale(false);
						//node->setScale(Vector3(_transform.GetS().mData[0], _transform.GetS().mData[1], _transform.GetS().mData[2]));
						node->setScale(Vector3(1, 1, 1));
						node->setInheritScale(true);
					}
					else
					{
						node->setPosition(Vector3(_transform.GetT().mData[0], _transform.GetT().mData[1], _transform.GetT().mData[2]));
						node->setOrientation(Quaternion(_transform.GetQ().mData[3], _transform.GetQ().mData[0], _transform.GetQ().mData[1], _transform.GetQ().mData[2]));
						node->setScale(Vector3(_transform.GetS().mData[0], _transform.GetS().mData[1], _transform.GetS().mData[2]));
					}
				}
				else
				{
					node->_setDerivedPosition(skeletonBone->GetBindingPosition());
					node->_setDerivedOrientation(skeletonBone->GetBindingRotation());
					node->setInheritScale(false);
					node->setScale(skeletonBone->GetBindingScale());
					node->setInheritScale(true);
				}

				node->setInitialState();
			}

			existsMaterials.clear();
			fbxMatCache.clear();
		}

		if (!_continue || searchIndex == -1)
		{
			auto & _pair = childMap[curFbxNode];

			int j = 0;
			for (auto i = _pair.second.begin(); i != _pair.second.end(); ++i, ++j)
			{
				FbxNode* _fbxNode = *i;
				nstack.insert(nstack.begin() + j, make_pair(node, _fbxNode));
			}
		}
	}
}

void FBXScene::SetSkeleton(SceneNode * root, SkeletonPtr skeleton, Entity *& outEntity)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		curNode->skeletonLoaded = true;

		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* entity = (Entity*)curNode->getAttachedObject(0);
				std::string uid = getName() + "_" + to_string(entity->getMesh()->fbxIndex);

				if (subMeshesList[uid].animated)
				{
					if (allMeshes[entity->getMesh()->fbxIndex]->getSkeletonName().empty())
					{
						entity->getMesh()->setSkeletonName(skeleton->getName());

						int j = 0;
						for (std::map<int, std::vector<MeshData>>::iterator it = subMeshesList[uid].subMeshInfo.begin(); it != subMeshesList[uid].subMeshInfo.end(); ++it)
						{
							for (std::vector<MeshData>::iterator dit = it->second.begin(); dit != it->second.end(); ++dit)
							{
								for (std::vector<BlendData>::iterator bit = dit->blendData.begin(); bit != dit->blendData.end(); ++bit)
								{
									if (skeleton->hasBone(bit->boneName))
									{
										int b = skeleton->getBone(bit->boneName)->getHandle();

										VertexBoneAssignment bAssign;
										bAssign.boneIndex = b;
										bAssign.vertexIndex = bit->index;
										bAssign.weight = bit->weight;
										entity->getSubEntity(j)->getSubMesh()->addBoneAssignment(bAssign);
									}
								}
							}

							j += 1;
						}

						std::string _newName = curNode->getName() + GUIDGenerator::genGuid();

						while (sceneManager->hasSceneNode(_newName))
							_newName = curNode->getName() + "_" + GetEngine->GenerateNewName(sceneManager);

						std::string _newNameMesh = _newName + "_SkinnedMesh";

						/*MeshPtr skinnedMesh = entity->getMesh()->clone(_newName, "Assets");*/

						//Create new entity with animation and delete the old one
						MeshPtr skinnedMesh;

						while (MeshManager::getSingleton().resourceExists(_newNameMesh, "Assets"))
							_newNameMesh = _newName + "_SkinnedMesh" + GetEngine->GenerateNewName(sceneManager);

						skinnedMesh = entity->getMesh()->clone(_newNameMesh, "Assets");

						Entity* skinnedEntity = sceneManager->createEntity(_newName, skinnedMesh);
						skinnedMesh->isFbx = entity->getMesh()->isFbx;
						skinnedMesh->fbxIndex = entity->getMesh()->fbxIndex;
						skinnedMesh->_notifyOrigin(entity->getMesh()->getOrigin());
						ResourceMap::guidMap[skinnedMesh.getPointer()] = ResourceMap::guidMap[entity->getMesh().getPointer()];
						skinnedMesh->_setBoneBoundingRadius(200.0f);

						//skinnedEntity->setDisplaySkeleton(true);
						skinnedEntity->setUpdateBoundingBoxFromSkeleton(true);
						skinnedEntity->setCastShadows(entity->getCastShadows());
						skinnedEntity->setVisible(entity->getVisible());

						for (int m = 0; m < skinnedEntity->getNumSubEntities(); ++m)
						{
							skinnedEntity->getSubEntity(m)->setMaterialName(entity->getSubEntity(m)->getMaterialName());
						}

						allMeshes[skinnedMesh->fbxIndex] = skinnedMesh;
						subMeshesList[uid].subMeshInfo.clear();
						//subMeshesList[uid].fromCache = true;

						bool meshInUse = false;
						MapIterator it = sceneManager->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);
						while (it.hasMoreElements())
						{
							Entity* ent = (Entity*)it.getNext();

							if (ent->getMesh() == entity->getMesh())
							{
								meshInUse = true;
								break;
							}
						}

						if (!meshInUse)
						{
							MeshManager::getSingleton().unload(entity->getMesh()->getHandle());
							MeshManager::getSingleton().remove(entity->getMesh()->getHandle());
						}

						sceneManager->destroyEntity(entity);

						curNode->attachObject(skinnedEntity);
						outEntity = skinnedEntity;
					}
					else
					{
						std::string _newName = curNode->getName();

						while (sceneManager->hasSceneNode(_newName))
							_newName = curNode->getName() + "_" + GetEngine->GenerateNewName(sceneManager);

						Entity* skinnedEntity = sceneManager->createEntity(_newName, allMeshes[entity->getMesh()->fbxIndex]);
						skinnedEntity->setUpdateBoundingBoxFromSkeleton(true);
						skinnedEntity->setCastShadows(entity->getCastShadows());
						skinnedEntity->setVisible(entity->getVisible());
						//skinnedEntity->setDisplaySkeleton(true);

						for (int m = 0; m < skinnedEntity->getNumSubEntities(); ++m)
						{
							skinnedEntity->getSubEntity(m)->setMaterialName(entity->getSubEntity(m)->getMaterialName());
						}

						sceneManager->destroyEntity(entity);

						skinnedEntity->setQueryFlags(1 << 1);
						curNode->attachObject(skinnedEntity);

						outEntity = skinnedEntity;
					}
				}
			}
		}

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			if (nd->fbxFromFile == root->fbxFromFile && nd->fbxFromGuid == root->fbxFromGuid)
				nstack.insert(nstack.begin() + j, nd);
			else
				--j;
		}
	}
}

void FBXScene::ClearAnimationData()
{
	animationData.clear();
}

Quaternion FBXScene::GetQuaternionRotation(FbxNode * fbxNode)
{
	FbxDouble3 translation = fbxNode->LclTranslation.Get();
	FbxVector4 eulerAng = fbxNode->LclRotation.Get();
	FbxMatrix eM;
	eM.SetTRS(translation, eulerAng, FbxVector4(1, 1, 1, 1));
	FbxQuaternion rotation;
	FbxVector4 pos, sp, sc;
	double c;
	eM.GetElements(pos, rotation, sp, sc, c);

	return Quaternion(rotation.mData[3], rotation.mData[0], rotation.mData[1], rotation.mData[2]);
}

Quaternion FBXScene::GetQuaternionRotation(FbxAMatrix mat)
{
	FbxQuaternion rotation = mat.GetQ();

	return Quaternion(rotation.mData[3], rotation.mData[0], rotation.mData[1], rotation.mData[2]);
}

Quaternion FBXScene::GetQuaternionRotation(FbxVector4 vec)
{
	FbxDouble3 translation;
	FbxVector4 eulerAng = vec;
	FbxMatrix eM;
	eM.SetTRS(translation, eulerAng, FbxVector4(1, 1, 1, 1));
	FbxQuaternion rotation;
	FbxVector4 pos, sp, sc;
	double c;
	eM.GetElements(pos, rotation, sp, sc, c);

	return Quaternion(rotation.mData[3], rotation.mData[0], rotation.mData[1], rotation.mData[2]);
}

void FBXScene::ImportSkeleton(SceneNode * parent, SkeletonPtr skeleton, Bone * rootBone, bool & hasAnim)
{
	std::vector<std::pair<SceneNode*, Bone*>> nstack;
	nstack.push_back(make_pair(parent, rootBone));

	while (nstack.size() > 0)
	{
		SceneNode* curNode = nstack.begin()->first;
		Bone* curBone = nstack.begin()->second;
		nstack.erase(nstack.begin());

		////////////////////
		SkeletonBone* bone = (SkeletonBone*)curNode->GetComponent(SkeletonBone::COMPONENT_TYPE);
		Bone* b = curBone;

		if (bone != nullptr)
		{
			boneIndex += 1;

			if (curBone == nullptr)
			{
				b = skeleton->createBone(bone->GetName(), boneIndex);
				skeleton->getRootBones().at(0)->addChild(b);
			}
			else
			{
				b = skeleton->createBone(bone->GetName(), boneIndex);
				curBone->addChild(b);
			}

			if (b != nullptr)
			{
				b->_setDerivedPosition(bone->GetBindingPosition());
				b->_setDerivedOrientation(bone->GetBindingRotation());
				b->setInheritScale(false);
				b->setScale(bone->GetBindingScale());
				b->setInheritScale(true);
			}

			hasAnim = true;
		}
		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			if (nd->fbxFromFile == curNode->fbxFromFile && nd->fbxFromGuid == curNode->fbxFromGuid)
				nstack.insert(nstack.begin() + j, make_pair(nd, b));
			else
				--j;
		}
	}
}

void FBXScene::ImportAnimationCurves(FbxNode * fbxNode, SceneNode * sceneNode, AnimationList * list, AnimationList::AnimationData & anim)
{
	//Animation curves
	int numStacks = lScene->GetSrcObjectCount<FbxAnimStack>();
	FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(lScene->GetSrcObject<FbxAnimStack>());
	if (pAnimStack != nullptr)
	{
		if (list != nullptr)
		{
			FbxTakeInfo* takeinfo = fbxNode->GetScene()->GetTakeInfo(pAnimStack->GetName());
			fbxsdk::FbxTime start = takeinfo->mLocalTimeSpan.GetStart();
			fbxsdk::FbxTime end = takeinfo->mLocalTimeSpan.GetStop();
			FbxLongLong animationlength = end.GetFrameCount(fbxsdk::FbxTime::eFrames30) - start.GetFrameCount(fbxsdk::FbxTime::eFrames30) + 1;

			int ind = indexMap[fbxNode->GetUniqueID()];

			std::string fbxNameRepl = boost::replace_all_copy(getName(), "/", "_") + "_anim";
			std::string cachePath = GetEngine->GetCachePath() + "Models/" + md5(fbxNameRepl) + ".cache";

			if (animationCache == nullptr)
			{
				animationCache = new FBXCache();
				
				if (IO::FileExists(cachePath))
				{
					animationCache->Load(cachePath);
				}
			}

			std::vector<FBXAnimationNode>::iterator srch = find_if(animationCache->animationCache.begin(), animationCache->animationCache.end(), [ind](const FBXAnimationNode & _c) -> bool
			{
				return _c.uid == ind;
			});

			if (srch == animationCache->animationCache.end())
			{
				FBXAnimationNode animNode;
				animNode.uid = ind;
				animationCache->animationCache.push_back(animNode);
				CacheAnimationCurvesKeys(fbxNode, animationlength, start.GetFrameCount(fbxsdk::FbxTime::eFrames30), end.GetFrameCount(fbxsdk::FbxTime::eFrames30));

				std::wstring dir = ::StringConvert::s2ws(GetEngine->GetAssetsPath() + "Cache/", GetACP());
				std::wstring dir1 = ::StringConvert::s2ws(GetEngine->GetAssetsPath() + "Cache/Models/", GetACP());
				CreateDirectory(dir.c_str(), nullptr);
				CreateDirectory(dir1.c_str(), nullptr);

				animationCache->Save(cachePath);
			}

			ImportAnimationCurvesKeys(fbxNode, sceneNode, list->GetParentSceneNode(), anim.name, anim.endFrame - anim.startFrame, anim.startFrame, anim.endFrame);
		}
	}
}

void FBXScene::ImportAnimationCurvesKeys(FbxNode * fbxNode, SceneNode * sceneNode, SceneNode * root, std::string animName, int length, int startTime, int endTime)
{
	if (fbxNode == nullptr)
		return;

	if (fbxNode->GetMesh() != nullptr && fbxNode->GetMesh()->GetDeformerCount() > 0)
		return;

	int key = 0;

	for (int i = startTime; i < endTime; ++i)
	{
		Vector3 _translation;
		Quaternion _rotation;
		Vector3 _scale;

		FBXAnimationCache _ac;

		std::vector<FBXAnimationNode>::iterator srch = find_if(animationCache->animationCache.begin(), animationCache->animationCache.end(), [=](const FBXAnimationNode & _c) -> bool
		{
			return _c.uid == indexMap[fbxNode->GetUniqueID()];
		});

		if (i < srch->keyFrames.size())
			_ac = srch->keyFrames.at(i);
		else
			_ac = srch->keyFrames[srch->keyFrames.size() - 1];

		_translation = Vector3(_ac.position.x, _ac.position.y, _ac.position.z);
		_rotation = Quaternion(_ac.rotation.w, _ac.rotation.x, _ac.rotation.y, _ac.rotation.z);
		_scale = Vector3(_ac.scale.x, _ac.scale.y, _ac.scale.z);

		AnimationData anim;
		anim.skeletal = false;
		anim.name = getName() + "_" + root->getName() + "_anim_" + animName;
		anim.frame = key;
		anim.position = _translation;
		anim.rotation = _rotation;
		anim.scale = _scale;
		anim.boneName = sceneNode->getName();
		anim.length = length;

		//Add key frame to the list
		animationData.push_back(anim);

		key += 1;
	}
}

void FBXScene::CacheAnimationCurvesKeys(FbxNode * fbxNode, int length, int startTime, int endTime)
{
	if (fbxNode == nullptr)
		return;

	if (fbxNode->GetMesh() != nullptr && fbxNode->GetMesh()->GetDeformerCount() > 0)
		return;

	int key = 0;

	for (FbxLongLong i = startTime; i <= endTime; ++i)
	{
		fbxsdk::FbxTime time;
		time.SetFrame(i, fbxsdk::FbxTime::eFrames30);

		Vector3 _translation;
		Quaternion _rotation;
		Vector3 _scale;

		FbxAMatrix mat = fbxNode->EvaluateLocalTransform(time);

		_translation = Vector3(mat.GetT().mData[0], mat.GetT().mData[1], mat.GetT().mData[2]);
		_rotation = Quaternion(mat.GetQ().mData[3], mat.GetQ().mData[0], mat.GetQ().mData[1], mat.GetQ().mData[2]);
		_scale = Vector3(mat.GetS().mData[0], mat.GetS().mData[1], mat.GetS().mData[2]);

		FBXAnimationCache __ac;
		__ac.frame = key;
		__ac.skeletal = false;
		__ac.position = SVector3(_translation.x, _translation.y, _translation.z);
		__ac.rotation = SQuaternion(_rotation.x, _rotation.y, _rotation.z, _rotation.w);
		__ac.scale = SVector3(_scale.x, _scale.y, _scale.z);

		std::vector<FBXAnimationNode>::iterator srch = find_if(animationCache->animationCache.begin(), animationCache->animationCache.end(), [=](const FBXAnimationNode & _c) -> bool
		{
			return _c.uid == indexMap[fbxNode->GetUniqueID()];
		});

		srch->keyFrames.push_back(__ac);

		key += 1;
	}
}

void FBXScene::ImportAnimationSkeletal(FbxNode * fbxNode, SceneNode * sceneNode, AnimationList * list, AnimationList::AnimationData & anim)
{
	FbxMesh * fbxMesh = fbxNode->GetMesh();

	if (fbxMesh == nullptr)
		return;

	for (int deformerIndex = 0; deformerIndex < fbxMesh->GetDeformerCount(); ++deformerIndex)
	{
		FbxSkin* skin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex/*, FbxDeformer::eSkin*/));
		if (!skin)
			continue;

		for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); ++clusterIndex)
		{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			if (!cluster->GetLink())
				continue;

			std::string jointname = cluster->GetLink()->GetName();

			//Animation
			FbxAnimStack* animstack = fbxNode->GetScene()->GetSrcObject<FbxAnimStack>(0);
			if (animstack != nullptr)
			{
				if (list != nullptr)
				{
					std::string animstackname = animstack->GetName();
					FbxTakeInfo* takeinfo = fbxNode->GetScene()->GetTakeInfo(FbxString(animstackname.c_str()));
					fbxsdk::FbxTime start = takeinfo->mLocalTimeSpan.GetStart();
					fbxsdk::FbxTime end = takeinfo->mLocalTimeSpan.GetStop();
					FbxLongLong animationlength = end.GetFrameCount(fbxsdk::FbxTime::eFrames30) - start.GetFrameCount(fbxsdk::FbxTime::eFrames30) + 1;

					int ind = indexMap[cluster->GetLink()->GetUniqueID()];

					std::string fbxNameRepl = boost::replace_all_copy(getName(), "/", "_") + "_skelanim";
					std::string cachePath = GetEngine->GetCachePath() + "Models/" + md5(fbxNameRepl) + ".cache";

					if (animationSkeletalCache == nullptr)
					{
						animationSkeletalCache = new FBXCache();

						if (IO::FileExists(cachePath))
						{
							animationSkeletalCache->Load(cachePath);
						}
					}

					std::vector<FBXAnimationNode>::iterator srch = find_if(animationSkeletalCache->animationCache.begin(), animationSkeletalCache->animationCache.end(), [ind](const FBXAnimationNode & _c) -> bool
					{
						return _c.uid == ind;
					});

					if (srch == animationSkeletalCache->animationCache.end())
					{
						FBXAnimationNode animNode;
						animNode.uid = ind;
						animationSkeletalCache->animationCache.push_back(animNode);
						CacheAnimationSkeletalKeys(fbxNode, cluster, animationlength, start.GetFrameCount(fbxsdk::FbxTime::eFrames30), end.GetFrameCount(fbxsdk::FbxTime::eFrames30));

						std::wstring dir = ::StringConvert::s2ws(GetEngine->GetAssetsPath() + "Cache/", GetACP());
						std::wstring dir1 = ::StringConvert::s2ws(GetEngine->GetAssetsPath() + "Cache/Models/", GetACP());
						CreateDirectory(dir.c_str(), nullptr);
						CreateDirectory(dir1.c_str(), nullptr);

						animationSkeletalCache->Save(cachePath);
					}

					ImportAnimationSkeletalKeys(fbxNode, cluster->GetLink(), sceneNode, list->GetParentSceneNode(), anim.name, anim.endFrame - anim.startFrame, anim.startFrame, anim.endFrame);
				}
			}
		}
	}
}

void FBXScene::ImportAnimationSkeletalKeys(FbxNode * fbxNode, FbxNode * pBoneNode, SceneNode * sceneNode, SceneNode * root, std::string animName, int length, int startTime, int endTime)
{
	if (fbxNode == nullptr)
		return;

	int key = 0;

	for (int i = startTime; i < endTime; ++i)
	{
		Vector3 _translation;
		Quaternion _rotation;
		Vector3 _scale;

		FBXAnimationCache _ac;

		std::vector<FBXAnimationNode>::iterator srch = find_if(animationSkeletalCache->animationCache.begin(), animationSkeletalCache->animationCache.end(), [=](const FBXAnimationNode & _c) -> bool
		{
			return _c.uid == indexMap[pBoneNode->GetUniqueID()];
		});

		if (i < srch->keyFrames.size())
			_ac = srch->keyFrames.at(i);
		else
			_ac = srch->keyFrames[srch->keyFrames.size() - 1];

		_translation = Vector3(_ac.position.x, _ac.position.y, _ac.position.z);
		_rotation = Quaternion(_ac.rotation.w, _ac.rotation.x, _ac.rotation.y, _ac.rotation.z);
		_scale = Vector3(_ac.scale.x, _ac.scale.y, _ac.scale.z);

		std::string _nm = to_string(indexMap[pBoneNode->GetUniqueID()]);

		AnimationData anim;
		anim.name = getName() + "_" + root->getName() + "_anim_" + animName;
		anim.frame = key;
		anim.skeletal = true;
		anim.position = _translation;
		anim.rotation = _rotation;
		anim.scale = _scale;
		anim.boneName = _nm;
		anim.length = length;

		//Add key frame to the list
		animationData.push_back(anim);

		key += 1;
	}
}

void FBXScene::CacheAnimationSkeletalKeys(FbxNode* fbxNode, FbxCluster* cluster, int length, int startTime, int endTime)
{
	if (fbxNode == nullptr)
		return;

	FbxNode* pBoneNode = cluster->GetLink();

	int key = 0;

	for (FbxLongLong i = startTime; i <= endTime; ++i)
	{
		AnimationData anim;

		fbxsdk::FbxTime time;
		time.SetFrame(i, fbxsdk::FbxTime::eFrames30);

		Vector3 _translation;
		Quaternion _rotation;
		Vector3 _scale;

		FbxAMatrix BoneTransform = pBoneNode->EvaluateLocalTransform(time);

		if (pBoneNode->GetParent() != nullptr && pBoneNode->GetParent()->GetSkeleton() == nullptr)
		{
			BoneTransform = pBoneNode->EvaluateGlobalTransform(time);
		}

		FbxVector4 __translation = BoneTransform.GetT();
		FbxQuaternion __rotation = BoneTransform.GetQ();
		FbxVector4 __scale = BoneTransform.GetS();

		_translation = Vector3(__translation.mData[0], __translation.mData[1], __translation.mData[2]);
		_rotation = Quaternion(__rotation.mData[3], __rotation.mData[0], __rotation.mData[1], __rotation.mData[2]);
		_scale = Vector3(__scale.mData[0], __scale.mData[1], __scale.mData[2]);

		FBXAnimationCache __ac;
		__ac.frame = key;
		__ac.skeletal = true;
		__ac.position = SVector3(_translation.x, _translation.y, _translation.z);
		__ac.rotation = SQuaternion(_rotation.x, _rotation.y, _rotation.z, _rotation.w);
		__ac.scale = SVector3(_scale.x, _scale.y, _scale.z);

		std::vector<FBXAnimationNode>::iterator srch = find_if(animationSkeletalCache->animationCache.begin(), animationSkeletalCache->animationCache.end(), [=](const FBXAnimationNode & _c) -> bool
		{
			return _c.uid == indexMap[pBoneNode->GetUniqueID()];
		});

		srch->keyFrames.push_back(__ac);

		key += 1;
	}
}

void FBXScene::AddAnimationComponent(SceneNode * node, std::string animName, int startTime, int endTime)
{
	//Add animation list component
	AnimationList * animationList = new AnimationList(node);
	animationList->AddAnimationData(animName, getName(), startTime, endTime);
	animationList->SetFBXFileName(getName());

	node->AddComponent(animationList);
}

FbxNode * FBXScene::GetByIndex(FbxNode * root, int index)
{
	FbxNode * node = nullptr;
	int __start = 0;

	GetByIndexRecursive(root, index, node, __start);

	return node;
}

void FBXScene::GetByIndexRecursive(FbxNode * root, int index, FbxNode *& outNode, int & outIndex)
{
	std::vector<FbxNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		FbxNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		if (outNode != nullptr)
			break;

		if (outIndex == index)
		{
			outNode = curNode;
			break;
		}

		++outIndex;
		////////////////////

		int childCount = curNode->GetChildCount();
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* _fbxNode = curNode->GetChild(i);
			nstack.insert(nstack.begin() + i, _fbxNode);
		}
	}

	nstack.clear();
}

void FBXScene::ImportKeyFrames(AnimationList * list, SceneNode * parent, SkeletonPtr skeleton, SceneNode * onlyThis)
{
	Animation * anim = nullptr;
	NodeAnimationTrack* pTrack = nullptr;

	for (std::vector<AnimationData>::iterator it = animationData.begin(); it != animationData.end(); ++it)
	{
		if (onlyThis != nullptr)
		{
			if (it->skeletal == false)
			{
				if (it->boneName != onlyThis->getName())
					continue;
			}
			else
			{
				continue;
			}
		}

		if (it->skeletal)
		{
			if (!skeleton->hasAnimation(it->name))
			{
				anim = skeleton->createAnimation(it->name, it->length);

				if (!sceneManager->hasAnimation(it->name))
				{
					sceneManager->createAnimation(it->name, it->length);
					sceneManager->createAnimationState(it->name);
				}
			}
			else
				anim = skeleton->getAnimation(it->name);

			if (skeleton->hasBone(it->boneName))
			{
				int boneHandle = skeleton->getBone(it->boneName)->getHandle();

				if (!anim->hasNodeTrack(boneHandle))
				{
					Bone * bone = skeleton->getBone(it->boneName);
					pTrack = anim->createNodeTrack(boneHandle, bone);
					pTrack->setTranslateMode(AnimationTrack::TranslateMode::TM_ADD);
					pTrack->setTransformSpace(AnimationTrack::TransformSpace::LOCAL);
				}
				else
				{
					pTrack = anim->getNodeTrack(boneHandle);
					pTrack->setTranslateMode(AnimationTrack::TranslateMode::TM_ADD);
					pTrack->setTransformSpace(AnimationTrack::TransformSpace::LOCAL);
				}

				TransformKeyFrame * kf = pTrack->createNodeKeyFrame(it->frame);

				kf->setTranslate(it->position);
				kf->setRotation(it->rotation);
				kf->setScale(it->scale);
			}
		}
		else
		{
			if (!sceneManager->hasAnimation(it->name))
			{
				anim = sceneManager->createAnimation(it->name, it->length);
				sceneManager->createAnimationState(it->name);
			}
			else
				anim = sceneManager->getAnimation(it->name);

			//if (!sceneManager->hasSceneNode(it->boneName))
			//	continue;

			//SceneNode * animNode = sceneManager->getSceneNodeFast(it->boneName);
			SceneNode * animNode = list->indexedPointers[it->boneName];
			
			if (animNode == nullptr)
				continue;

			if (!anim->hasNodeTrack(animNode->gcHandle))
			{
				pTrack = anim->createNodeTrack(animNode->gcHandle, animNode);
				pTrack->setTranslateMode(AnimationTrack::TranslateMode::TM_ADD);
				pTrack->setTransformSpace(AnimationTrack::TransformSpace::LOCAL);
			}
			else
			{
				pTrack = anim->getNodeTrack(animNode->gcHandle);
				pTrack->setTranslateMode(AnimationTrack::TranslateMode::TM_ADD);
				pTrack->setTransformSpace(AnimationTrack::TransformSpace::LOCAL);
			}

			TransformKeyFrame * kf = pTrack->createNodeKeyFrame(it->frame);

			kf->setTranslate(it->position);
			kf->setRotation(it->rotation);
			kf->setScale(it->scale);
		}
	}
	
	//anim->setUseBaseKeyFrame(true, 0);
}

void FBXScene::LoadCurves(SceneNode * sceneNode, int fbxIndex, AnimationList * list, AnimationList::AnimationData & anim)
{
	FbxManager * sdkManager = FBXSceneManager::getSingleton().GetSDKManager();

	if (lScene == nullptr)
	{
		fbxImporter = FbxImporter::Create(sdkManager, "");
		FbxIOSettings * ioSettings = FBXSceneManager::getSingleton().GetIOSettings();

		//bool lImportStatus = fbxImporter->Initialize(CP_UNI(mOrigin).c_str(), -1, ioSettings);
		bool lImportStatus = false;

		BufferedFbx* buf = nullptr;
		char* buffer = nullptr;

		if (GetEngine->GetUseUnpackedResources())
		{
			lImportStatus = fbxImporter->Initialize(CP_UNI(mOrigin).c_str(), -1, ioSettings);
		}
		else
		{
			if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), getName()))
			{
				int sz = 0;
				buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), getName(), sz);

				buf = new BufferedFbx(sdkManager);
				buf->buffer = buffer;
				buf->size = sz;

				lImportStatus = fbxImporter->Initialize(buf, 0, -1, ioSettings);
			}
			else if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipBuiltin(), getName()))
			{
				int sz = 0;
				buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipBuiltin(), getName(), sz);

				buf = new BufferedFbx(sdkManager);
				buf->buffer = buffer;
				buf->size = sz;

				lImportStatus = fbxImporter->Initialize(buf, 0, -1, ioSettings);
			}
		}

		lScene = FbxScene::Create(sdkManager, mName.c_str());

		fbxImporter->Import(lScene);
		fbxImporter->Destroy();

		if (!GetEngine->GetUseUnpackedResources())
		{
			if (buf != nullptr)
				delete buf;

			if (buffer != nullptr)
				delete[] buffer;
		}

		indexMap.clear();
		for (int ind = 0; ind < lScene->GetNodeCount(); ++ind)
		{
			indexMap[lScene->GetNode(ind)->GetUniqueID()] = ind;
		}
	}

	FbxNode * fbxNode = lScene->GetNode(fbxIndex);

	if (fbxNode != nullptr)
	{
		ImportAnimationCurves(fbxNode, sceneNode, list, anim);
		//ImportKeyFrames(sceneNode, SkeletonPtr(), sceneNode);
	}
}

void FBXScene::LoadSkeletal(SceneNode * sceneNode, int fbxIndex, AnimationList * list, AnimationList::AnimationData & anim)
{
	FbxManager * sdkManager = FBXSceneManager::getSingleton().GetSDKManager();

	if (lScene == nullptr)
	{
		fbxImporter = FbxImporter::Create(sdkManager, "");
		FbxIOSettings * ioSettings = FBXSceneManager::getSingleton().GetIOSettings();

		//bool lImportStatus = fbxImporter->Initialize(CP_UNI(mOrigin).c_str(), -1, ioSettings);
		bool lImportStatus = false;

		BufferedFbx* buf = nullptr;
		char* buffer = nullptr;

		if (GetEngine->GetUseUnpackedResources())
		{
			lImportStatus = fbxImporter->Initialize(CP_UNI(mOrigin).c_str(), -1, ioSettings);
		}
		else
		{
			if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), getName()))
			{
				int sz = 0;
				buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), getName(), sz);

				buf = new BufferedFbx(sdkManager);
				buf->buffer = buffer;
				buf->size = sz;

				lImportStatus = fbxImporter->Initialize(buf, 0, -1, ioSettings);
			}
			else if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipBuiltin(), getName()))
			{
				int sz = 0;
				buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipBuiltin(), getName(), sz);

				buf = new BufferedFbx(sdkManager);
				buf->buffer = buffer;
				buf->size = sz;

				lImportStatus = fbxImporter->Initialize(buf, 0, -1, ioSettings);
			}
		}

		lScene = FbxScene::Create(sdkManager, mName.c_str());

		fbxImporter->Import(lScene);
		fbxImporter->Destroy();

		if (!GetEngine->GetUseUnpackedResources())
		{
			if (buf != nullptr)
				delete buf;

			if (buffer != nullptr)
				delete[] buffer;
		}

		indexMap.clear();
		for (int ind = 0; ind < lScene->GetNodeCount(); ++ind)
		{
			indexMap[lScene->GetNode(ind)->GetUniqueID()] = ind;
		}
	}

	FbxNode * fbxNode = GetByIndex(lScene->GetRootNode(), fbxIndex);

	if (fbxNode != nullptr)
		ImportAnimationSkeletal(fbxNode, sceneNode, list, anim);
}

void FBXScene::loadImpl()
{

}

void FBXScene::unloadImpl()
{
	fbxImporter->Destroy();
}

size_t FBXScene::calculateSize() const
{
	return size_t();
}

void FBXScene::GetIndexFromRootRecursive(SceneNode * root, SceneNode * need, int & index, int & outIndex)
{
	VectorIterator it = root->getChildIterator();

	if (root != need)
	{
		++outIndex;

		while (it.hasMoreElements())
		{
			SceneNode * node = (SceneNode*)it.getNext();
			GetIndexFromRootRecursive(node, need, index, outIndex);
		}
	}
	else
	{
		index = outIndex;
	}
}

void FBXScene::GetNodeByIndexFromRootRecursive(SceneNode * root, SceneNode *& outNode, int index, int & outIndex)
{
	VectorIterator it = root->getChildIterator();

	if (outIndex != index)
	{
		++outIndex;

		while (it.hasMoreElements())
		{
			SceneNode * node = (SceneNode*)it.getNext();
			GetNodeByIndexFromRootRecursive(node, outNode, index, outIndex);
		}
	}
	else
	{
		outNode = root;
	}
}
