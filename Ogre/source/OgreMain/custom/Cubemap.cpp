#include "Cubemap.h"
#include "TextParser.h"
#include "OgreLogManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreTextureUnitState.h"

//Cubemap

Cubemap::Cubemap(Ogre::ResourceManager* creator, const Ogre::String &name,
	Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual,
	Ogre::ManualResourceLoader *loader) : Ogre::Resource(creator, name, handle, group, isManual, loader)
{
	createParamDictionary("Cubemap");
}

Cubemap::~Cubemap()
{
	unload();
}

void Cubemap::SetTextureFrontName(Ogre::String name)
{
	textureFront = name;
	unload();
	loadImpl();
}

void Cubemap::SetTextureBackName(Ogre::String name)
{
	textureBack = name;
	unload();
	loadImpl();
}

void Cubemap::SetTextureLeftName(Ogre::String name)
{
	textureLeft = name;
	unload();
	loadImpl();
}

void Cubemap::SetTextureRightName(Ogre::String name)
{
	textureRight = name;
	unload();
	loadImpl();
}

void Cubemap::SetTextureUpName(Ogre::String name)
{
	textureUp = name;
	unload();
	loadImpl();
}

void Cubemap::SetTextureDownName(Ogre::String name)
{
	textureDown = name;
	unload();
	loadImpl();
}

TexturePtr Cubemap::GetTextureCubePtr()
{
	if (cubeTexture == nullptr)
	{
		unload();
		loadImpl();
	}

	return cubeTexture;
}

void Cubemap::loadImpl()
{
	if (cubeTexture != nullptr)
		TextureManager::getSingleton().remove(cubeTexture);

	cubeTexture = TextureManager::getSingleton().createManual(getName(),
		mGroup,
		TextureType::TEX_TYPE_CUBE_MAP,
		32,
		32,
		0,
		PixelFormat::PF_A8R8G8B8);

	if (!textureFront.empty())
	{
		if (!textureBack.empty())
		{
			if (!textureLeft.empty())
			{
				if (!textureRight.empty())
				{
					if (!textureUp.empty())
					{
						if (!textureDown.empty())
						{
							TexturePtr texturePtrFront;
							TexturePtr texturePtrBack;
							TexturePtr texturePtrLeft;
							TexturePtr texturePtrRight;
							TexturePtr texturePtrUp;
							TexturePtr texturePtrDown;

							try { texturePtrFront = TextureManager::getSingleton().load(textureFront, mGroup); }
							catch (...) {}
							try { texturePtrBack = TextureManager::getSingleton().load(textureBack, mGroup); }
							catch (...) {}
							try { texturePtrLeft = TextureManager::getSingleton().load(textureLeft, mGroup); }
							catch (...) {}
							try { texturePtrRight = TextureManager::getSingleton().load(textureRight, mGroup); }
							catch (...) {}
							try { texturePtrUp = TextureManager::getSingleton().load(textureUp, mGroup); }
							catch (...) {}
							try { texturePtrDown = TextureManager::getSingleton().load(textureDown, mGroup); }
							catch (...) {}

							if (texturePtrFront != nullptr && texturePtrBack != nullptr && texturePtrLeft != nullptr && texturePtrRight != nullptr && texturePtrUp != nullptr && texturePtrDown != nullptr)
							{
								if (cubeTexture != nullptr)
									TextureManager::getSingleton().remove(cubeTexture);

								cubeTexture = TextureManager::getSingleton().createManual(getName(),
									mGroup,
									TextureType::TEX_TYPE_CUBE_MAP,
									texturePtrFront->getWidth(),
									texturePtrFront->getHeight(),
									0,
									PixelFormat::PF_A8R8G8B8);

								Image imgRight;
								texturePtrRight->convertToImage(imgRight);
								Image imgLeft;
								texturePtrLeft->convertToImage(imgLeft);
								Image imgBack;
								texturePtrBack->convertToImage(imgBack);
								Image imgFront;
								texturePtrFront->convertToImage(imgFront);
								Image imgUp;
								texturePtrUp->convertToImage(imgUp);
								Image imgDown;
								texturePtrDown->convertToImage(imgDown);

								cubeTexture->getBuffer(0)->blitFromMemory(imgRight.getPixelBox());
								cubeTexture->getBuffer(1)->blitFromMemory(imgLeft.getPixelBox());
								cubeTexture->getBuffer(2)->blitFromMemory(imgUp.getPixelBox());
								cubeTexture->getBuffer(3)->blitFromMemory(imgDown.getPixelBox());
								cubeTexture->getBuffer(4)->blitFromMemory(imgFront.getPixelBox());
								cubeTexture->getBuffer(5)->blitFromMemory(imgBack.getPixelBox());

								imgBack.freeMemory();
								imgFront.freeMemory();
								imgLeft.freeMemory();
								imgRight.freeMemory();
								imgDown.freeMemory();
								imgUp.freeMemory();
							}

							if (texturePtrFront != nullptr)
								TextureManager::getSingleton().remove(texturePtrFront);

							if (texturePtrBack != nullptr)
								TextureManager::getSingleton().remove(texturePtrBack);

							if (texturePtrRight != nullptr)
								TextureManager::getSingleton().remove(texturePtrRight);

							if (texturePtrLeft != nullptr)
								TextureManager::getSingleton().remove(texturePtrLeft);

							if (texturePtrUp != nullptr)
								TextureManager::getSingleton().remove(texturePtrUp);

							if (texturePtrDown != nullptr)
								TextureManager::getSingleton().remove(texturePtrDown);
						}
					}
				}
			}
		}
	}

	if (texUnitState != nullptr)
		texUnitState->setCubicTexture(&cubeTexture, true);
}

void Cubemap::unloadImpl()
{
	if (cubeTexture != nullptr)
	{
		TextureManager::getSingleton().remove(cubeTexture);
		cubeTexture.setNull();
	}
}

size_t Cubemap::calculateSize() const
{
	size_t memSize = textureFront.length() + textureBack.length() + textureLeft.length() + textureRight.length() + textureUp.length() + textureDown.length();

	memSize += sizeof(CubemapPtr) * 3;

	return memSize;
}

//Cubemap serializer

CubemapSerializer::CubemapSerializer()
{
}

CubemapSerializer::~CubemapSerializer()
{
}

void CubemapSerializer::exportCubemap(CubemapPtr cubemap, const Ogre::String &fileName)
{
	std::ofstream outFile;
	outFile.open(fileName.c_str(), std::ios::out);

	string uv = "separateUV";
	if (cubemap->GetUvType() == Cubemap::UVType::UVT_COMBINED)
		uv = "combinedUVW";

	String data = "cubemap " + cubemap->getName() + "\n";
	data += "{\n";
	data += "	texture_front " + cubemap->GetTextureFrontName() + "\n";
	data += "	texture_back " + cubemap->GetTextureBackName() + "\n";
	data += "	texture_left " + cubemap->GetTextureLeftName() + "\n";
	data += "	texture_right " + cubemap->GetTextureRightName() + "\n";
	data += "	texture_up " + cubemap->GetTextureUpName() + "\n";
	data += "	texture_down " + cubemap->GetTextureDownName() + "\n";
	data += "	uv_type " + uv + "\n";
	data += "}\n";

	outFile << data;

	outFile.close();
}

void CubemapSerializer::importCubemap(Ogre::DataStreamPtr &stream, Cubemap *pDest)
{
	String data = stream->getAsString();

	TextParser parser;
	TTextData textData = parser.ParseText(data);

	pDest->SetTextureFrontName(textData.GetByKey("texture_front").value);
	pDest->SetTextureBackName(textData.GetByKey("texture_back").value);
	pDest->SetTextureLeftName(textData.GetByKey("texture_left").value);
	pDest->SetTextureRightName(textData.GetByKey("texture_right").value);
	pDest->SetTextureUpName(textData.GetByKey("texture_up").value);
	pDest->SetTextureDownName(textData.GetByKey("texture_down").value);

	string uv = textData.GetByKey("uv_type").value;
	
	if (uv == "separateUV")
	{
		pDest->SetUvType(Cubemap::UVType::UVT_SEPARATE);
	}

	if (uv == "combinedUVW")
	{
		pDest->SetUvType(Cubemap::UVType::UVT_COMBINED);
	}
}

//Cubemap manager

template<> CubemapManager *Ogre::Singleton<CubemapManager>::msSingleton = 0;

CubemapManager *CubemapManager::getSingletonPtr()
{
	return msSingleton;
}

CubemapPtr CubemapManager::getByName(const String & name, const String & groupName)
{
	return static_pointer_cast<Cubemap>(getResourceByName(name, groupName));
}

CubemapManager &CubemapManager::getSingleton()
{
	assert(msSingleton);
	return(*msSingleton);
}

CubemapManager::CubemapManager()
{
	mResourceType = "Cubemap";
	mLoadOrder = 90.0f;

	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

CubemapManager::~CubemapManager()
{
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

CubemapPtr CubemapManager::create(const String & name, const String & group, bool isManual, ManualResourceLoader * loader, const NameValuePairList * createParams)
{
	return static_pointer_cast<Cubemap>(createResource(name, group, isManual, loader, createParams));
}

void CubemapManager::parseScript(DataStreamPtr & stream, const String & groupName)
{
	ScriptCompilerManager::getSingleton().parseScript(stream, groupName);
}

CubemapPtr CubemapManager::load(const Ogre::String &name, const Ogre::String &group)
{
	CubemapPtr cube = getByName(name, group);

	if (cube == NULL)
		cube = create(name, group);

	cube->load();

	return cube;
}

Ogre::Resource *CubemapManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
	const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
	const Ogre::NameValuePairList *createParams)
{
	Cubemap * ret = OGRE_NEW Cubemap(this, name, handle, group, isManual, loader);

	if (createParams != NULL)
		ret->_notifyOrigin(createParams->at("Path") + name + ".cubemap");

	return ret;
}