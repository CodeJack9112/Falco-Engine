#pragma once

#include <OgreResourceManager.h>
#include "FBXScene.h"

using namespace Ogre;

namespace fbxsdk
{
	class FbxManager;
	class FbxIOSettings;
}

using namespace fbxsdk;

class FBXSceneManager : public Ogre::ResourceManager, public Ogre::Singleton<FBXSceneManager>
{
public:
	FBXSceneManager(SceneManager * sceneMgr);
	virtual ~FBXSceneManager();

private:
	SceneManager * sceneManager;
	FbxManager * sdkManager = nullptr;
	FbxIOSettings * ioSettings = nullptr;
	//FbxImporter * fbxImporter = nullptr;

protected:
	Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
		const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
		const Ogre::NameValuePairList *createParams);

public:
	FBXScenePtr create(const String& name, const String& group, bool isManual = false, ManualResourceLoader* loader = 0, const NameValuePairList* createParams = 0);
	virtual FBXScenePtr load(const Ogre::String &name, const Ogre::String &group);
	FBXScenePtr getByName(const String& name, const String& groupName = ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

	FbxManager * GetSDKManager() { return sdkManager; }
	FbxIOSettings * GetIOSettings() { return ioSettings; }
	//FbxImporter * GetFBXImporter() { return fbxImporter; }

	SceneManager * GetSceneManager() { return sceneManager; }
	static FBXSceneManager &getSingleton();
	static FBXSceneManager *getSingletonPtr();
};

// Do not add this to the project
class FBXSceneFileLoader : public Ogre::ManualResourceLoader
{
public:

	FBXSceneFileLoader() {}
	virtual ~FBXSceneFileLoader() {}

	void loadResource(Ogre::Resource *resource)
	{
		FBXScene *tf = static_cast<FBXScene*>(resource);
	}
};