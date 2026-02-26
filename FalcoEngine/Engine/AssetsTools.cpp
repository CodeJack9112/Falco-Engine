#include "stdafx.h"
#include "AssetsTools.h"

#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreMaterialSerializer.h>
#include <OgreHighLevelGpuProgram.h>
#include <OgreHighLevelGpuProgramManager.h>

#include "../boost/algorithm/string.hpp"
#include "../Engine/IO.h"

#include "StringConverter.h"
#include "Engine.h"

using namespace std;

AssetsTools::AssetsTools()
{
}

AssetsTools::~AssetsTools()
{
}

MaterialPtr AssetsTools::CreateMaterial(string name, string path, MaterialType type)
{
	if (!MaterialManager::getSingleton().resourceExists(path + name + ".material", "Assets"))
	{
		MaterialPtr mMat = MaterialManager::getSingleton().create(path + name + ".material", "Assets");

		if (type == MT_FORWARD)
			return SetupMaterialForward(mMat, name, path);

		if (type == MT_DEFERRED)
			return SetupMaterialDeferred(mMat, name, path);

		if (type == MT_EMPTY_FORWARD)
			return SetupMaterialEmptyForward(mMat, name, path);

		if (type == MT_EMPTY_DEFERRED)
			return SetupMaterialEmptyDeferred(mMat, name, path);
	}

	return MaterialPtr();
}

void AssetsTools::RepairMaterials()
{
	/*MapIterator it = MaterialManager::getSingleton().getResourceIterator();

	while (it.hasMoreElements())
	{
		MaterialPtr mat = static_pointer_cast<Material>(it.getNext());
		if (!mat->isManuallyLoaded())
			SetupMaterial(mat, "", mat->getOrigin());
	}*/
}

Ogre::MaterialPtr AssetsTools::SetupMaterialForward(Ogre::MaterialPtr material, std::string name, std::string path)
{
	material->removeAllTechniques();

	Ogre::Technique* mTech = material->createTechnique();
	mTech->setName("Forward");
	mTech->setSchemeName(MaterialManager::DEFAULT_SCHEME_NAME);
	
	Pass* ambientPass = mTech->createPass();
	ambientPass->setLightingEnabled(false);
	ambientPass->setName("diffuse");
	ambientPass->setVertexProgram("Shaders/Diffuse.glslv", "Assets");
	ambientPass->setFragmentProgram("Shaders/Diffuse.glslf", "Assets");
	ambientPass->SetPreprocessorDefine("CUTOUT", false);
	ambientPass->SetPreprocessorDefine("HAS_DIFFUSE", true);

	TextureUnitState* tex = ambientPass->createTextureUnitState();
	tex->setName("diffuseMap");

	Pass* lightingPass = mTech->createPass();
	lightingPass->setName("lighting");
	lightingPass->setLightingEnabled(false);
	lightingPass->setIteratePerLight(true, false);
	lightingPass->setSceneBlending(SceneBlendType::SBT_ADD);
	lightingPass->SetPreprocessorDefine("CUTOUT", false);
	lightingPass->SetPreprocessorDefine("RECEIVE_SHADOWS", true);
	lightingPass->SetPreprocessorDefine("HAS_DIFFUSE", true);
	lightingPass->SetPreprocessorDefine("HAS_NORMAL", false);
	lightingPass->setVertexProgram("Shaders/BumpedSpecular.glslv", "Assets");
	lightingPass->setFragmentProgram("Shaders/BumpedSpecular.glslf", "Assets");

	tex = lightingPass->createTextureUnitState();
	tex->setName("diffuseMap");
	tex = lightingPass->createTextureUnitState();
	tex->setName("normalMap");

	material->SetProgramSharedParameter("diffuseColor", Material::ParameterType::PT_VECTOR4, Material::ValueVariant(Vector4(1, 1, 1, 1)));
	material->SetProgramSharedParameter("diffuseMap", Material::ParameterType::PT_SAMPLER2D, 0);
	material->SetProgramSharedParameter("normalMap", Material::ParameterType::PT_SAMPLER2D, 1);
	material->SetProgramSharedParameter("shininess", Material::ParameterType::PT_FLOAT, 20.0f);
	material->SetProgramSharedParameter("cutoutValue", Material::ParameterType::PT_FLOAT, 0.35f);

	//----------------------------------------------------------//

	MaterialSerializer serializer;

	try
	{
		if (!name.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path + name + ".material", false, false);
			material->_notifyOrigin(path + name + ".material");
		}
		else if (!path.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path, false, false);
		}
	}
	catch (...) {}

	return material;
}

MaterialPtr AssetsTools::SetupMaterialDeferred(MaterialPtr material, string name, string path)
{
	material->removeAllTechniques();

	Ogre::Technique* mTech = material->createTechnique();
	mTech->setName("Deferred");
	mTech->setSchemeName("Deferred");
	Pass * perlightPass = mTech->createPass();

	//----------------------------------------------------------//

	//Lighting pass
	perlightPass->setName("main");
	perlightPass->SetPreprocessorDefine("HAS_DIFFUSE", true);
	perlightPass->SetPreprocessorDefine("HAS_NORMAL", false);
	perlightPass->SetPreprocessorDefine("HAS_METALLIC", false);
	perlightPass->SetPreprocessorDefine("HAS_OCCLUSION", false);
	perlightPass->SetPreprocessorDefine("HAS_EMISSION", false);
	perlightPass->SetPreprocessorDefine("USE_IBL", false);
	perlightPass->setLightingEnabled(false);

	perlightPass->setVertexProgram("Shaders/DeferredPBR.glslv", "Assets");
	perlightPass->setFragmentProgram("Shaders/DeferredPBR.glslf", "Assets");

	material->SetProgramSharedParameter("cDiffuseColor", Material::ParameterType::PT_VECTOR3, Material::ValueVariant(Vector3(1, 1, 1)));
	material->SetProgramSharedParameter("cSpecularity", Material::ParameterType::PT_FLOAT, Material::ValueVariant(1.0f));
	material->SetProgramSharedParameter("cOcclusionFactor", Material::ParameterType::PT_FLOAT, Material::ValueVariant(1.0f));
	material->SetProgramSharedParameter("cIBLFactor", Material::ParameterType::PT_FLOAT, Material::ValueVariant(1.0f));
	material->SetProgramSharedParameter("cEmissionFactor", Material::ParameterType::PT_FLOAT, Material::ValueVariant(0.0f));
	material->SetProgramSharedParameter("cMetalness", Material::ParameterType::PT_FLOAT, Material::ValueVariant(0.5f));
	material->SetProgramSharedParameter("cReflectance", Material::ParameterType::PT_FLOAT, Material::ValueVariant(0.25f));
	//material->SetProgramSharedParameter("cFarDistance", Material::ParameterType::PT_AUTO, Material::ValueVariant(117));

	material->SetProgramSharedParameter("sDiffuseMap", Material::ParameterType::PT_INT, Material::ValueVariant(0));
	material->SetProgramSharedParameter("sNormalMap", Material::ParameterType::PT_INT, Material::ValueVariant(1));
	material->SetProgramSharedParameter("sMetallicMap", Material::ParameterType::PT_INT, Material::ValueVariant(2));
	material->SetProgramSharedParameter("sOcclusionMap", Material::ParameterType::PT_INT, Material::ValueVariant(3));
	material->SetProgramSharedParameter("sEnvMap", Material::ParameterType::PT_INT, Material::ValueVariant(4));
	material->SetProgramSharedParameter("sEmissionMap", Material::ParameterType::PT_INT, Material::ValueVariant(5));

	TextureUnitState * tex = perlightPass->createTextureUnitState();
	tex->setName("diffuseMap");
	tex = perlightPass->createTextureUnitState();
	tex->setName("normalMap");
	tex = perlightPass->createTextureUnitState();
	tex->setName("metallicMap");
	tex = perlightPass->createTextureUnitState();
	tex->setName("occlusionMap");
	tex = perlightPass->createTextureUnitState();
	tex->setName("envMap");
	tex = perlightPass->createTextureUnitState();
	tex->setName("emissionMap");

	//----------------------------------------------------------//

	MaterialSerializer serializer;

	try
	{
		if (!name.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path + name + ".material", false, false);
			material->_notifyOrigin(path + name + ".material");
		}
		else if (!path.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path, false, false);
		}
	}
	catch (...) {}

	return material;
}

Ogre::MaterialPtr AssetsTools::SetupMaterialEmptyForward(Ogre::MaterialPtr material, std::string name, std::string path)
{
	material->removeAllTechniques();

	Ogre::Technique* mTech = material->createTechnique();
	mTech->setName("Forward");
	mTech->setSchemeName(MaterialManager::DEFAULT_SCHEME_NAME);
	Pass* mainPass = mTech->createPass();

	mainPass->setName("main");

	//----------------------------------------------------------//

	MaterialSerializer serializer;

	try
	{
		if (!name.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path + name + ".material", false, false);
			material->_notifyOrigin(path + name + ".material");
		}
		else if (!path.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path, false, false);
		}
	}
	catch (...) {}

	return material;
}

Ogre::MaterialPtr AssetsTools::SetupMaterialEmptyDeferred(Ogre::MaterialPtr material, std::string name, std::string path)
{
	material->removeAllTechniques();

	Ogre::Technique* mTech = material->createTechnique();
	mTech->setName("Deferred");
	mTech->setSchemeName("Deferred");
	
	Pass* mainPass = mTech->createPass();
	mainPass->setName("main");
	mainPass->setLightingEnabled(false);

	//----------------------------------------------------------//

	MaterialSerializer serializer;

	try
	{
		if (!name.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path + name + ".material", false, false);
			material->_notifyOrigin(path + name + ".material");
		}
		else if (!path.empty())
		{
			serializer.exportMaterial(material, GetEngine->GetAssetsPath() + path, false, false);
		}
	}
	catch (...) {}

	return material;
}

MaterialPtr AssetsTools::CreateSkyboxMaterial(string name, string path)
{
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(name, "Assets");
	mMat->removeAllTechniques();

	Ogre::Technique* mTech = mMat->createTechnique();
	Pass * skyPass = mTech->createPass();

	//----------------------------------------------------------//

	//Skybox pass
	skyPass->setName("Skybox");
	skyPass->setLightingEnabled(false);
	skyPass->setDepthWriteEnabled(false);
	skyPass->setVertexProgram("Shaders/Skybox.glslv");
	skyPass->setFragmentProgram("Shaders/Skybox.glslf");
	
	//Textures
	TextureUnitState * tex = skyPass->createTextureUnitState();
	tex->setName("cubemap");
	tex->setTextureAddressingMode(TextureUnitState::TextureAddressingMode::TAM_CLAMP);

	//----------------------------------------------------------//

	MaterialSerializer serializer;

	try
	{
		serializer.exportMaterial(mMat, GetEngine->GetAssetsPath() + path + name + ".material", false, false);
		mMat->_notifyOrigin(path + name + ".material");
	}
	catch (...) {}

	return mMat;
}

CubemapPtr AssetsTools::CreateCubemap(string name, string path)
{
	if (!CubemapManager::getSingleton().resourceExists(path + name + ".cubemap", "Assets"))
	{
		CubemapPtr cubemap = CubemapManager::getSingleton().create(path + name + ".cubemap", "Assets");
		cubemap->_notifyOrigin(path + name + ".cubemap");

		CubemapSerializer serializer;
		serializer.exportCubemap(cubemap, GetEngine->GetAssetsPath() + path + name + ".cubemap");

		return cubemap;
	}

	return CubemapPtr();
}

void AssetsTools::CreateCSharpScript(string name, string path)
{
	std::string _name = boost::replace_all_copy(name, " ", "");

	std::string content =
		"using FalcoEngine;\n"
		"using System;\n"
		"using System.Collections;\n"
		"using System.Collections.Generic;\n"
		"\n"
		"public class " + _name + " : MonoBehaviour" + "\n"
		"{\n"
		"	void Start()\n"
		"	{\n"
		"		\n"
		"	}\n"
		"	\n"
		"	void Update(float deltaTime)\n"
		"	{\n"
		"		\n"
		"	}\n"
		"}\n";

	IO::WriteText(GetEngine->GetAssetsPath() + path + _name + ".cs", StringConvert::cp_convert(content, GetACP(), 65001));
}

HighLevelGpuProgramPtr AssetsTools::CreateVertexShader(std::string name, std::string path)
{
	string mMasterSource = "void main()\n"
"{\n"
"\n"
"}";

	HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(path + name + ".glslv", "Assets", "glsl", GPT_VERTEX_PROGRAM);
	ptrProgram->setParameter("profiles", "glsl120");
	ptrProgram->setSource(mMasterSource);

	IO::WriteText(GetEngine->GetAssetsPath() + path + name + ".glslv", mMasterSource);

	return ptrProgram;
}

HighLevelGpuProgramPtr AssetsTools::CreateFragmentShader(std::string name, std::string path)
{
	string mMasterSource = "void main()\n"
"{\n"
"\n"
"}";

	HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(path + name + ".glslf", "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
	ptrProgram->setParameter("profiles", "glsl120");
	ptrProgram->setSource(mMasterSource);

	IO::WriteText(GetEngine->GetAssetsPath() + path + name + ".glslf", mMasterSource);

	return ptrProgram;
}
