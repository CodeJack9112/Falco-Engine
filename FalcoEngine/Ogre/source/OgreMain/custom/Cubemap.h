#pragma once

#include "OgrePrerequisites.h"
#include "OgreResourceManager.h"
#include "OgreSerializer.h"
#include "OgreTextureManager.h"
#include "OgreScriptCompiler.h"
#include "../src/OgreScriptParser.h"
#include "../src/OgreScriptLexer.h"

using namespace Ogre;

class _OgreExport Cubemap : public Resource
{
public:
	enum UVType { UVT_SEPARATE, UVT_COMBINED };

protected:
	Ogre::String textureFront;
	Ogre::String textureBack;
	Ogre::String textureLeft;
	Ogre::String textureRight;
	Ogre::String textureUp;
	Ogre::String textureDown;

	TexturePtr cubeTexture;

	UVType uvType = UVType::UVT_SEPARATE;

	virtual void postLoadImpl(void) {}
	void loadImpl();
	void unloadImpl();
	size_t calculateSize() const;
	TextureUnitState* texUnitState = nullptr;

public:

	Cubemap(Ogre::ResourceManager *creator, const Ogre::String &name,
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		Ogre::ManualResourceLoader *loader = 0);

	//Cubemap & operator = (const Cubemap& oth);

	virtual ~Cubemap();
	
	void SetTextureFrontName(Ogre::String name);
	void SetTextureBackName(Ogre::String name);
	void SetTextureLeftName(Ogre::String name);
	void SetTextureRightName(Ogre::String name);
	void SetTextureUpName(Ogre::String name);
	void SetTextureDownName(Ogre::String name);
	void SetUvType(UVType type) { uvType = type; }

	Ogre::String GetTextureFrontName() { return textureFront; }
	Ogre::String GetTextureBackName() { return textureBack; }
	Ogre::String GetTextureLeftName() { return textureLeft; }
	Ogre::String GetTextureRightName() { return textureRight; }
	Ogre::String GetTextureUpName() { return textureUp; }
	Ogre::String GetTextureDownName() { return textureDown; }

	TexturePtr GetTextureCubePtr();
	void SetTextureUnitState(TextureUnitState* state) { texUnitState = state; }

	UVType GetUvType() { return uvType; }
};

typedef SharedPtr<Cubemap> CubemapPtr;

class _OgreExport CubemapSerializer : public Ogre::Serializer
{
public:
	CubemapSerializer();
	virtual ~CubemapSerializer();

	void exportCubemap(CubemapPtr cubemap, const Ogre::String &fileName);
	void importCubemap(Ogre::DataStreamPtr &stream, Cubemap *pDest);
};

class _OgreExport CubemapManager : public Ogre::ResourceManager, public Ogre::Singleton<CubemapManager>
{
protected:

	// must implement this from ResourceManager's interface
	Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
		const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
		const Ogre::NameValuePairList *createParams);

public:

	CubemapManager();
	virtual ~CubemapManager();

	CubemapPtr create(const String& name, const String& group,
		bool isManual = false, ManualResourceLoader* loader = 0,
		const NameValuePairList* createParams = 0);

	void parseScript(DataStreamPtr& stream, const String& groupName);

	virtual CubemapPtr load(const Ogre::String &name, const Ogre::String &group);

	static CubemapManager &getSingleton();
	static CubemapManager *getSingletonPtr();

	CubemapPtr getByName(const String& name, const String& groupName = ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
};