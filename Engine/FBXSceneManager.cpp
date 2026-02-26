#include "stdafx.h"

#include "FBXSceneManager.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

template<> FBXSceneManager *Ogre::Singleton<FBXSceneManager>::msSingleton = 0;

FBXSceneManager::FBXSceneManager(SceneManager * sceneMgr)
{
	sceneManager = sceneMgr;

	mResourceType = "FBXScene";

	// low, because it will likely reference other resources
	mLoadOrder = 91.0f;

	// this is how we register the ResourceManager with OGRE
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);

	//Create FBX SDK manager
	sdkManager = FbxManager::Create();

	// Create an IOSettings object.
	ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ioSettings);
}

FBXSceneManager::~FBXSceneManager()
{
	// and this is how we unregister it
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

Ogre::Resource * FBXSceneManager::createImpl(const Ogre::String & name, Ogre::ResourceHandle handle, const Ogre::String & group, bool isManual, Ogre::ManualResourceLoader * loader, const Ogre::NameValuePairList * createParams)
{
	FBXScene * ret = new FBXScene(this, name, handle, group, isManual, loader);

	if (createParams != NULL)
		ret->_notifyOrigin(createParams->at("Path"));

	return ret;
}

FBXScenePtr FBXSceneManager::create(const String & name, const String & group, bool isManual, ManualResourceLoader * loader, const NameValuePairList * createParams)
{
	FBXScenePtr fbx = static_pointer_cast<FBXScene>(createResource(name, group, isManual, loader, createParams));
	
	return fbx;
}

FBXScenePtr FBXSceneManager::load(const Ogre::String & name, const Ogre::String & group)
{
	FBXScenePtr fbx = getByName(name);

	if (fbx.isNull())
		fbx = create(name, group);

	fbx->load();
	return fbx;
}

FBXScenePtr FBXSceneManager::getByName(const String & name, const String & groupName)
{
	FBXScenePtr fbx = static_pointer_cast<FBXScene>(getResourceByName(name, groupName));

	return fbx;
}

FBXSceneManager & FBXSceneManager::getSingleton()
{
	assert(msSingleton);
	return(*msSingleton);
}

FBXSceneManager * FBXSceneManager::getSingletonPtr()
{
	return msSingleton;
}
