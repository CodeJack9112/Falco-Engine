#include "../stdafx.h"
#include "DeferredShading.h"

#include "OgreConfigFile.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

#include "OgreRoot.h"
#include "OgreRenderSystem.h"

#include "OgreEntity.h"
#include "OgreSubEntity.h"
#include "OgreRoot.h"

#include "OgreCompositor.h"
#include "OgreCompositorManager.h"
#include "OgreCompositorChain.h"
#include "OgreCompositorInstance.h"

#include "OgreLogManager.h"

#include "DeferredLightCP.h"
#include "MaterialSchemeHandler.h"

#include "OgreShadowCameraSetupFocused.h"
#include "OgreShadowCameraSetup.h"
#include "ShadowCameraSetupStableCSM.h"
#include "CSMGpuConstants.h"

#include "../ProjectSettings.h"
#include "../IO.h"
#include "GpuProgramSources.h"
#include "LightMaterialGenerator.h"

DefaultShadowCameraSetup * DeferredShadingSystem::DefaultCameraSetup;
StableCSMShadowCameraSetup * DeferredShadingSystem::CSMCameraSetup;
CSMGpuConstants* DeferredShadingSystem::mGpuConstants;

using namespace Ogre;

const Ogre::uint8 DeferredShadingSystem::PRE_GBUFFER_RENDER_QUEUE = Ogre::RENDER_QUEUE_1;
const Ogre::uint8 DeferredShadingSystem::POST_GBUFFER_RENDER_QUEUE = Ogre::RENDER_QUEUE_8;

DeferredShadingSystem::DeferredShadingSystem(SceneManager *sm) : mSceneMgr(sm)
{
    sm->setShadowTechnique(SHADOWTYPE_NONE);

	ProjectSettings projectSettings;
	if (IO::FileExists(GetEngine->GetRootPath() + "Settings/Main.settings"))
	{
		projectSettings.Load(GetEngine->GetRootPath() + "Settings/Main.settings");
		SceneManager::dirShadowResolution = projectSettings.directionalShadowResolution;
		SceneManager::spotShadowResolution = projectSettings.spotShadowResolution;
		SceneManager::pointShadowResolution = projectSettings.pointShadowResolution;
		SceneManager::shadowCascadesCount = projectSettings.shadowCascadesCount;
		SceneManager::shadowSamplesCount = projectSettings.shadowSamplesCount;
		SceneManager::shadowCascadesBlending = projectSettings.shadowCascadesBlending;
		SceneManager::shadowsEnabled = projectSettings.shadowsEnabled;
	}

	DefaultCameraSetup = new DefaultShadowCameraSetup();

	float lambda = 0.93f; // lower lamdba means more uniform, higher lambda means more logarithmic
	float firstSplitDist = 50.0f;
	float farClip = 5000.0f;
	float splitPadding = 1.0f;

	mGpuConstants = new CSMGpuConstants(maxCascades);
	CSMCameraSetup = new StableCSMShadowCameraSetup(mGpuConstants);
	CSMCameraSetup->calculateSplitPoints(SceneManager::shadowCascadesCount, firstSplitDist, farClip, lambda);
	StableCSMShadowCameraSetup::SplitPointList points = CSMCameraSetup->getSplitPoints();
	CSMCameraSetup->setSplitPoints(points);
	CSMCameraSetup->setSplitPadding(splitPadding);
}

void DeferredShadingSystem::createResources()
{
	//-------Shaders begin-----------//
	//Ambient
	HighLevelGpuProgramPtr ambientVsProg = HighLevelGpuProgramManager::getSingleton().createProgram("DeferredShading/post/Ambient_vs", "Assets", "glsl", GPT_VERTEX_PROGRAM);
	ambientVsProg->setParameter("profiles", "glsl120");
	ambientVsProg->setSource(GpuProgramSources::ambientVs);
	ambientVsProg->getDefaultParameters()->setNamedAutoConstant("flip", GpuProgramParameters::AutoConstantType::ACT_RENDER_TARGET_FLIPPING);
	ambientVsProg->getDefaultParameters()->setNamedConstant("farCorner", Vector3(1.0f, 1.0f, 1.0f));

	HighLevelGpuProgramPtr ambientPsProg = HighLevelGpuProgramManager::getSingleton().createProgram("DeferredShading/post/Ambient_ps", "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
	ambientPsProg->setParameter("profiles", "glsl120");
	ambientPsProg->setSource(GpuProgramSources::ambientPs);
	ambientPsProg->getDefaultParameters()->setNamedAutoConstant("ambientColor", GpuProgramParameters::AutoConstantType::ACT_AMBIENT_LIGHT_COLOUR);
	ambientPsProg->getDefaultParameters()->setNamedAutoConstant("proj", GpuProgramParameters::AutoConstantType::ACT_PROJECTION_MATRIX);
	ambientPsProg->getDefaultParameters()->setNamedAutoConstant("farClipDistance", GpuProgramParameters::AutoConstantType::ACT_FAR_CLIP_DISTANCE);
	ambientPsProg->getDefaultParameters()->setNamedConstant("Tex0", 0);
	ambientPsProg->getDefaultParameters()->setNamedConstant("Tex1", 1);
	ambientPsProg->getDefaultParameters()->setNamedConstant("Tex2", 7);
	ambientPsProg->getDefaultParameters()->setNamedConstant("Tex3", 5);
	ambientPsProg->getDefaultParameters()->setNamedConstant("Tex4", 6);

	//Light
	HighLevelGpuProgramPtr lightVsProg = HighLevelGpuProgramManager::getSingleton().createProgram("DeferredShading/post/Standard_vs", "Assets", "glsl", GPT_VERTEX_PROGRAM);
	lightVsProg->setParameter("profiles", "glsl120");
	lightVsProg->setSource(GpuProgramSources::lightVs);
	lightVsProg->getDefaultParameters()->setNamedAutoConstant("worldViewProj", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEWPROJ_MATRIX);

	HighLevelGpuProgramPtr lightPsProg = HighLevelGpuProgramManager::getSingleton().createProgram("DeferredShading/post/Standard_ps", "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
	lightPsProg->setParameter("profiles", "glsl120");
	lightPsProg->setSource(GpuProgramSources::lightFs);

	//Shadow caster
	HighLevelGpuProgramPtr shadowCasterVsProg = HighLevelGpuProgramManager::getSingleton().createProgram("DeferredShading/Shadows/CasterVP", "Assets", "glsl", GPT_VERTEX_PROGRAM);
	shadowCasterVsProg->setParameter("profiles", "glsl120");
	shadowCasterVsProg->setSource(GpuProgramSources::shadowCasterVs);
	shadowCasterVsProg->getDefaultParameters()->setNamedAutoConstant("texelOffsets", GpuProgramParameters::AutoConstantType::ACT_TEXEL_OFFSETS);
	shadowCasterVsProg->getDefaultParameters()->setNamedAutoConstant("cWorldViewProj", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEWPROJ_MATRIX);
	shadowCasterVsProg->getDefaultParameters()->setNamedAutoConstant("cWorldView", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEW_MATRIX);

	HighLevelGpuProgramPtr shadowCasterPsProg = HighLevelGpuProgramManager::getSingleton().createProgram("DeferredShading/Shadows/CasterFP", "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
	shadowCasterPsProg->setParameter("profiles", "glsl120");
	shadowCasterPsProg->setSource(GpuProgramSources::shadowCasterFs);
	shadowCasterPsProg->getDefaultParameters()->setNamedAutoConstant("cFarDistance", GpuProgramParameters::AutoConstantType::ACT_FAR_CLIP_DISTANCE);
	shadowCasterPsProg->getDefaultParameters()->setNamedConstant("diffuseTexture", 0);
	//-------Shaders end-----------//

	//-------Materials begin---------//

	//Ambient light
	lightAmbientMaterial = MaterialManager::getSingleton().create("DeferredShading/AmbientLight", "Assets");
	lightAmbientMaterial->removeAllTechniques();
	Technique* tech = lightAmbientMaterial->createTechnique();
	Pass* pass = tech->createPass();
	pass->setLightingEnabled(false);
	pass->setDepthWriteEnabled(true);
	pass->setDepthCheckEnabled(true);
	pass->setVertexProgram("DeferredShading/post/Ambient_vs", false);
	pass->setFragmentProgram("DeferredShading/post/Ambient_ps", false);

	for (int i = 0; i < 8; ++i)
	{
		TextureUnitState* tex = pass->createTextureUnitState();
		tex->setContentType(TextureUnitState::ContentType::CONTENT_COMPOSITOR);
		tex->setCompositorReference("DeferredShading/GBuffer", "mrt_output", i);
		tex->setTextureAddressingMode(TextureUnitState::TextureAddressingMode::TAM_CLAMP);
		tex->setTextureFiltering(TextureFilterOptions::TFO_NONE);
	}

	//Light geometry
	lightGeometryMaterial = MaterialManager::getSingleton().create("DeferredShading/LightMaterial/Geometry", "Assets");
	lightGeometryMaterial->removeAllTechniques();
	tech = lightGeometryMaterial->createTechnique();
	tech->setName("DeferredTechnique");
	pass = tech->createPass();
	pass->setName("DeferredPass");
	pass->setSceneBlending(SceneBlendType::SBT_ADD);
	pass->setLightingEnabled(false);
	pass->setDepthWriteEnabled(false);
	pass->setDepthCheckEnabled(true);

	for (int i = 0; i < 8; ++i)
	{
		TextureUnitState* tex = pass->createTextureUnitState();
		tex->setName("GBuffer" + to_string(i + 1));
		tex->setContentType(TextureUnitState::ContentType::CONTENT_COMPOSITOR);
		tex->setCompositorReference("DeferredShading/GBuffer", "mrt_output", i);
		tex->setTextureAddressingMode(TextureUnitState::TextureAddressingMode::TAM_CLAMP);
		tex->setTextureFiltering(TextureFilterOptions::TFO_NONE);
	}

	//Light geometry shadow
	lightShadowMaterial = lightGeometryMaterial->clone("DeferredShading/LightMaterial/GeometryShadow");
	TextureUnitState* tex = lightShadowMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
	tex->setName("ShadowMap");
	tex->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);

	//Light quad
	lightQuadMaterial = lightGeometryMaterial->clone("DeferredShading/LightMaterial/Quad");
	lightQuadMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	//Light quad shadow
	lightQuadShadowMaterial = lightShadowMaterial->clone("DeferredShading/LightMaterial/QuadShadow");
	lightQuadShadowMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	//Simple quad
	quadMaterial = MaterialManager::getSingleton().create("DeferredShading/Post/SimpleQuad", "Assets");
	quadMaterial->removeAllTechniques();
	tech = quadMaterial->createTechnique();
	pass = tech->createPass();
	pass->setLightingEnabled(false);
	pass->setDepthWriteEnabled(false);
	pass->setDepthCheckEnabled(false);
	tex = pass->createTextureUnitState();
	tex->setTextureName("white.bmp");
	tex->setTextureFiltering(TextureFilterOptions::TFO_NONE);

	shadowCasterMaterial = MaterialManager::getSingleton().create("DeferredShading/Shadows/Caster", "Assets");
	shadowCasterMaterial->removeAllTechniques();
	tech = shadowCasterMaterial->createTechnique();
	pass = tech->createPass();
	pass->setLightingEnabled(false);
	pass->setVertexProgram("DeferredShading/Shadows/CasterVP", false);
	pass->setFragmentProgram("DeferredShading/Shadows/CasterFP", false);

	//-------Materials end---------//

	//------Compositors begin-----//
	//GBuffer
	CompositorPtr gbuffer = CompositorManager::getSingleton().create("DeferredShading/GBuffer", "Assets");
	CompositionTechnique* ctech = gbuffer->createTechnique();
	CompositionTechnique::TextureDefinition * texdef = ctech->createTextureDefinition("mrt_output");
	texdef->width = 0;
	texdef->height = 0;
	texdef->formatList = { PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA, PixelFormat::PF_FLOAT16_RGBA };
	texdef->scope = CompositionTechnique::TextureScope::TS_CHAIN;
	CompositionTargetPass * tpass = ctech->createTargetPass();
	tpass->setOutputName("mrt_output");
	tpass->setInputMode(CompositionTargetPass::InputMode::IM_NONE);
	
	CompositionPass * cpass = tpass->createPass();
	cpass->setType(CompositionPass::PassType::PT_CLEAR);
	tpass->setShadowsEnabled(false);
	tpass->setMaterialScheme("Deferred");

	CompositionPass * cpass2 = tpass->createPass();
	cpass2->setType(CompositionPass::PassType::PT_RENDERSCENE);
	cpass->setFirstRenderQueue(RENDER_QUEUE_1);
	cpass->setLastRenderQueue(RENDER_QUEUE_8 - 1);

	//ShowLit
	CompositorPtr showLit = CompositorManager::getSingleton().create("DeferredShading/ShowLit", "Assets");
	ctech = showLit->createTechnique();
	tpass = ctech->getOutputTargetPass();
	tpass->setInputMode(CompositionTargetPass::InputMode::IM_NONE);
	tpass->setShadowsEnabled(false);

	cpass = tpass->createPass();
	cpass->setType(CompositionPass::PassType::PT_CLEAR);
	cpass->setClearColour(ColourValue(0.4f, 0.4f, 0.4f, 1.0f));

	cpass2 = tpass->createPass();
	cpass2->setType(CompositionPass::PassType::PT_RENDERSCENE);
	cpass2->setMaterialScheme("Default");
	cpass2->setFirstRenderQueue(1);
	cpass2->setLastRenderQueue(9);

	CompositionPass* cpass3 = tpass->createPass();
	cpass3->setType(CompositionPass::PassType::PT_RENDERCUSTOM);
	cpass3->setCustomType("DeferredLight");
	/*cpass3->setInput(0, "mrt_output", 0);
	cpass3->setInput(1, "mrt_output", 1);
	cpass3->setInput(2, "mrt_output", 2);
	cpass3->setInput(3, "mrt_output", 3);
	cpass3->setInput(4, "mrt_output", 4);
	cpass3->setInput(5, "mrt_output", 5);
	cpass3->setInput(6, "mrt_output", 6);
	cpass3->setInput(7, "mrt_output", 7);*/

	CompositionPass* cpass4 = tpass->createPass();
	cpass4->setType(CompositionPass::PassType::PT_RENDERSCENE);
	cpass4->setMaterialScheme("Default");
	cpass4->setFirstRenderQueue(RENDER_QUEUE_1);
	cpass4->setLastRenderQueue(RENDER_QUEUE_9);

	/*CompositionPass* cpass5 = tpass->createPass();
	cpass5->setType(CompositionPass::PassType::PT_RENDERSCENE);
	cpass5->setMaterialScheme("Default");
	cpass5->setFirstRenderQueue(RENDER_QUEUE_8);*/
	//------Compositors end-------//
}

void DeferredShadingSystem::setClearColor()
{
	for (auto it = mViewports.begin(); it != mViewports.end(); ++it)
	{
		Viewport* vp = it->first;

		if (CompositorManager::getSingleton().hasCompositorChain(vp))
		{
			CompositorChain* chain = CompositorManager::getSingleton().getCompositorChain(vp);
			auto inst = chain->getCompositorInstances();

			for (auto it = inst.begin(); it != inst.end(); ++it)
			{
				setClearColor(*it, vp);
			}
		}
	}
}

void DeferredShadingSystem::resetLightShaders()
{
	DeferredLightCompositionPass* dfPass = getDeferredPass();
	auto ops = dfPass->getRenderOperations();

	for (auto it = ops.begin(); it != ops.end(); ++it)
	{
		DeferredLightRenderOperation* op = *it;

		auto& lights = op->getLightsMap();

		for (auto ll = lights.begin(); ll != lights.end(); ++ll)
		{
			MaterialPtr mat = ll->second->getMaterial();
			Technique* tech = mat->getTechnique(0);
			Pass* pass = tech->getPass(0);
			pass->setFragmentProgram("", true);
		}
	}

	for (auto it = LightMaterialGenerator::programsF.begin(); it != LightMaterialGenerator::programsF.end(); ++it)
	{
		HighLevelGpuProgramManager::getSingleton().remove(it->second->getHandle());
	}

	LightMaterialGenerator::programsF.clear();
}

void DeferredShadingSystem::setClearColor(CompositorInstance* inst, Viewport* vp)
{
	Camera* cam = vp->getCamera();

	CompositionTechnique* tech = inst->getTechnique();
	CompositionTargetPass* tpass = tech->getOutputTargetPass();
	if (tpass->getNumPasses() > 0)
	{
		CompositionPass* pass = tpass->getPass(0);
		pass->setClearColour(cam->getClearColor());
	}
}

void DeferredShadingSystem::initialize()
{
	CompositorManager& compMan = CompositorManager::getSingleton();
	compMan.initialise();

	forwardListener = new MaterialSchemeHandler();
	deferredListener = new MaterialSchemeHandler();

	MaterialManager::getSingleton().addListener(forwardListener, "Default");
	MaterialManager::getSingleton().addListener(deferredListener, "Deferred");

	deferredPass = new DeferredLightCompositionPass();

	compMan.registerCustomCompositionPass("DeferredLight", deferredPass);
}

void DeferredShadingSystem::RecalculateShadowCascades()
{
	float lambda = 0.93f;
	float firstSplitDist = 50.0f;
	float farClip = 5000.0f;

	CSMCameraSetup->calculateSplitPoints(SceneManager::shadowCascadesCount, firstSplitDist, farClip, lambda);
	StableCSMShadowCameraSetup::SplitPointList points = CSMCameraSetup->getSplitPoints();
	CSMCameraSetup->setSplitPoints(points);
}

void DeferredShadingSystem::addViewport(Ogre::Viewport* vp)
{
	CompositorManager& compMan = CompositorManager::getSingleton();

	CompositorInstance* inst1 = compMan.addCompositor(vp, "DeferredShading/GBuffer");
	CompositorInstance* inst2 = compMan.addCompositor(vp, "DeferredShading/ShowLit");

	inst1->setEnabled(true);
	inst2->setEnabled(true);

	mViewports[vp].push_back(inst1);
	mViewports[vp].push_back(inst2);

	setClearColor(inst2, vp);
}

void DeferredShadingSystem::removeViewport(Ogre::Viewport* vp)
{
	if (CompositorManager::getSingleton().hasCompositorChain(vp))
	{
		CompositorChain* chain = CompositorManager::getSingleton().getCompositorChain(vp);
		auto inst = chain->getCompositorInstances();

		for (auto it = inst.begin(); it != inst.end(); ++it)
			chain->_removeInstance(*it);

		CompositorManager::getSingleton().removeCompositorChain(vp);
	}

	mViewports[vp].clear();
	mViewports.erase(vp);
}

void DeferredShadingSystem::removeViewports()
{
	for (auto it = mViewports.begin(); it != mViewports.end(); ++it)
	{
		Viewport* vp = it->first;

		if (CompositorManager::getSingleton().hasCompositorChain(vp))
		{
			CompositorChain* chain = CompositorManager::getSingleton().getCompositorChain(vp);
			auto inst = chain->getCompositorInstances();

			for (auto it = inst.begin(); it != inst.end(); ++it)
				chain->_removeInstance(*it);

			CompositorManager::getSingleton().removeCompositorChain(vp);
		}

		mViewports[vp].clear();
	}

	mViewports.clear();
}

DeferredShadingSystem::~DeferredShadingSystem()
{
	Cleanup();
}

void DeferredShadingSystem::Cleanup()
{
	if (CompositorManager::getSingletonPtr() == nullptr)
		return;

	CompositorManager& compMan = CompositorManager::getSingleton();

	removeViewports();

	MaterialManager::getSingleton().removeListener(forwardListener, "Default");
	MaterialManager::getSingleton().removeListener(deferredListener, "Deferred");

	if (compMan.getHasCompositionPass("DeferredLight"))
		compMan.unRegisterCustomCompositionPass("DeferredLight");

	delete deferredPass;

	mViewports.clear();
}

void DeferredShadingSystem::resetLights()
{
	DeferredLightCompositionPass* dfPass = getDeferredPass();
	auto & ops = dfPass->getRenderOperations();

	for (auto it = ops.begin(); it != ops.end(); ++it)
	{
		DeferredLightRenderOperation* op = *it;
		
		auto & lights = op->getLightsMap();

		for (auto it = lights.begin(); it != lights.end(); ++it)
		{
			delete it->second;
		}

		lights.clear();
	}
}
