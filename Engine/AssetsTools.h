#pragma once

#include <OgrePrerequisites.h>
#include "../Ogre/source/OgreMain/custom/Cubemap.h"

class AssetsTools
{
private:
	static Ogre::MaterialPtr SetupMaterialForward(Ogre::MaterialPtr material, std::string name, std::string path);
	static Ogre::MaterialPtr SetupMaterialDeferred(Ogre::MaterialPtr material, std::string name, std::string path);
	static Ogre::MaterialPtr SetupMaterialEmptyForward(Ogre::MaterialPtr material, std::string name, std::string path);
	static Ogre::MaterialPtr SetupMaterialEmptyDeferred(Ogre::MaterialPtr material, std::string name, std::string path);
	
public:
	enum MaterialType { MT_FORWARD, MT_DEFERRED, MT_EMPTY_FORWARD, MT_EMPTY_DEFERRED };

	AssetsTools();
	~AssetsTools();

	static void RepairMaterials();
	static Ogre::MaterialPtr CreateMaterial(std::string name, std::string path, MaterialType type = MaterialType::MT_DEFERRED);
	static Ogre::MaterialPtr CreateSkyboxMaterial(std::string name, std::string path);
	static CubemapPtr CreateCubemap(std::string name, std::string path);
	static void CreateCSharpScript(std::string name, std::string path);
	static HighLevelGpuProgramPtr CreateVertexShader(std::string name, std::string path);
	static HighLevelGpuProgramPtr CreateFragmentShader(std::string name, std::string path);
};

