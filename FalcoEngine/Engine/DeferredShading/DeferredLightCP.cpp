#include "../stdafx.h"
#include "DeferredLightCP.h"

//#include "Ogre.h"

#include "LightMaterialGenerator.h"
#include "../Engine.h"
#include "DeferredShading.h"
#include "../../Ogre/source/OgreMain/include/OgreParticleSystemRenderer.h"
#include "ShadowCameraSetupStableCSM.h"
#include "CSMGpuConstants.h"

using namespace Ogre;

int MIXED_SHADOWS_UPDATE_INTERVAL = 60;

DeferredLightRenderOperation::DeferredLightRenderOperation(CompositorInstance* instance, const CompositionPass* pass)
{
    mViewport = instance->getChain()->getViewport();
    
    //Get the names of the GBuffer textures
    /*const CompositionPass::InputTex& input0 = pass->getInput(0);
    mTexName0 = instance->getTextureInstanceName(input0.name, input0.mrtIndex);
    const CompositionPass::InputTex& input1 = pass->getInput(1);
    mTexName1 = instance->getTextureInstanceName(input1.name, input1.mrtIndex);*/

    // Create lights material generator
    mLightMaterialGenerator = new LightMaterialGenerator();
    
    // Create the ambient light
    mAmbientLight = new AmbientLight();
    const MaterialPtr& mat = mAmbientLight->getMaterial();
    mat->load();

	textureProjMatrix = Matrix4(
		0.5, 0, 0, 0.5,
		0, -0.5, 0, 0.5,
		0, 0, 1, 0,
		0, 0, 0, 1);

	Ogre::MaterialManager::getSingleton().addListener(this, DEFERRED_SHADOWS_SCHEME_NAME);

	// Get a reference to the template shadow caster material
	shadowCasterMaterial = Ogre::MaterialManager::getSingleton().getByName(DEFERRED_SHADOW_CASTER_NAME);

	// Make sure it's loaded
	if (!shadowCasterMaterial->isLoaded())
		shadowCasterMaterial->load();
}

DeferredLightRenderOperation::~DeferredLightRenderOperation()
{
	parent->removeRenderOperation(this);

	Ogre::MaterialManager::getSingleton().removeListener(this, DEFERRED_SHADOWS_SCHEME_NAME);

	for (LightsMap::iterator it = mLights.begin(); it != mLights.end(); ++it)
	{
		delete it->second;
	}

	mLights.clear();

	delete mAmbientLight;
	delete mLightMaterialGenerator;
}

DLight* DeferredLightRenderOperation::createDLight(Ogre::Light* light)
{
    DLight *rv = new DLight(mLightMaterialGenerator,light);
    mLights[light] = rv;
    return rv;
}

static void injectTechnique(SceneManager* sm, Technique* tech, Renderable* rend, const Ogre::LightList* lightList)
{
    for(unsigned short i=0; i<tech->getNumPasses(); ++i)
    {
        Ogre::Pass* pass = tech->getPass(i);
        if (lightList != 0) 
        {
            sm->_injectRenderWithPass(pass, rend, false, false, lightList);
        } 
        else
        {
            sm->_injectRenderWithPass(pass, rend, false);
        }
    }
}

void DeferredLightRenderOperation::execute(SceneManager *sm, RenderSystem *rs)
{
    Ogre::Camera* cam = mViewport->getCamera();

    mAmbientLight->updateFromCamera(cam);
    Technique* tech = mAmbientLight->getMaterial()->getBestTechnique();
    injectTechnique(sm, tech, mAmbientLight, 0);

	LightList lightList;
	/*if (updateStaticShadowmaps)
	{
		MapIterator it = sm->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);
		while (it.hasMoreElements())
			lightList.push_back((Light*)it.getNext());
	}
	else
		*/
	lightList = sm->_getLightsAffectingFrustum();
	
	for (LightList::const_iterator it = lightList.begin(); it != lightList.end(); ++it)
	{
		Light* light = *it;
		SceneNode* lightNode = light->getParentSceneNode();

		if (light->getMode() == Light::LightMode::LM_BAKED)
			continue;

		Ogre::LightList ll;
		ll.push_back(light);

		LightsMap::iterator dLightIt = mLights.find(light);
		DLight* dLight = 0;
		if (dLightIt == mLights.end())
		{
			dLight = createDLight(light);
		}
		else
		{
			dLight = dLightIt->second;
			dLight->updateFromParent();
		}
		dLight->updateFromCamera(cam);
		tech = dLight->getMaterial()->getBestTechnique();

		if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
			SceneManager::currentLightType = 0;
		if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
			SceneManager::currentLightType = 1;
		if (light->getType() == Light::LightTypes::LT_POINT)
			SceneManager::currentLightType = 2;

		Pass* pass = tech->getPass(0);

		if (pass->hasFragmentProgram())
		{
			int lightMode = 0;

			if (light->getMode() == Light::LightMode::LM_REALTIME) lightMode = 0;
			if (light->getMode() == Light::LightMode::LM_BAKED) lightMode = 1;
			if (light->getMode() == Light::LightMode::LM_MIXED) lightMode = 2;

			Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
			if (params->_findNamedConstantDefinition("lightMode"))
				params->setNamedConstant("lightMode", lightMode);
		}

		//Update shadow texture
		if (light->getMode() != Light::LightMode::LM_BAKED)
		{
			if (SceneManager::shadowsEnabled)
			{
				if (dLight->getCastShadows())
				{
					GetEngine->SetUpdateOnFrame(false);
					sm->_setCurrentShadowPassLightMode(light->getMode());
					SceneManager::RenderContext* context = sm->_pauseRendering();

					//if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
					//{
						if (light->getMode() == Light::LightMode::LM_MIXED)
						{
							bool update = false;

							if (light->frameCount == MIXED_SHADOWS_UPDATE_INTERVAL)
							{
								update = true;
								light->frameCount = 0;
							}

							if (update)
							{
								UpdateShadowmap(dLight, cam, pass);
							}

							light->frameCount++;
						}
					//}

					if (light->getMode() == Light::LightMode::LM_REALTIME)
						UpdateShadowmap(dLight, cam, pass);

					sm->_resumeRendering(context);
					GetEngine->SetUpdateOnFrame(true);

					TextureUnitState* tus = pass->getTextureUnitState("ShadowMap");

					if (tus != nullptr)
					{
						if (pass->hasFragmentProgram())
						{
							Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
							if (params->_findNamedConstantDefinition("shadowAASamples"))
								params->setNamedConstant("shadowAASamples", (float)SceneManager::shadowSamplesCount);

							if (params->_findNamedConstantDefinition("shadowCascadesCount"))
								params->setNamedConstant("shadowCascadesCount", SceneManager::shadowCascadesCount);
						}

						if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
						{
							if (light->getShadowTexture(0) != nullptr)
							{
								TextureUnitState* tus2 = pass->getTextureUnitState("ShadowMap1");
								TextureUnitState* tus3 = nullptr;
								TextureUnitState* tus4 = nullptr;

								if (SceneManager::shadowCascadesCount > 2)
									tus3 = pass->getTextureUnitState("ShadowMap2");
								if (SceneManager::shadowCascadesCount > 3)
									tus4 = pass->getTextureUnitState("ShadowMap3");

								if (tus2 == nullptr)
								{
									tus2 = pass->createTextureUnitState();
									tus2->setName("ShadowMap1");
									tus2->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
								}

								if (tus3 == nullptr && SceneManager::shadowCascadesCount > 2)
								{
									tus3 = pass->createTextureUnitState();
									tus3->setName("ShadowMap2");
									tus3->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
								}

								if (tus4 == nullptr && SceneManager::shadowCascadesCount > 3)
								{
									tus4 = pass->createTextureUnitState();
									tus4->setName("ShadowMap3");
									tus4->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
								}

								tus->setTexture(light->getShadowTexture(0));
								tus2->setTexture(light->getShadowTexture(1));
								if (SceneManager::shadowCascadesCount > 2)
									tus3->setTexture(light->getShadowTexture(2));
								else
								{
									if (tus3 != nullptr)
									{
										pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus3));
									}
								}
								if (SceneManager::shadowCascadesCount > 3)
									tus4->setTexture(light->getShadowTexture(3));
								else
								{
									if (tus4 != nullptr)
									{
										pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus4));
									}
								}
							}
						}
						else
						{
							if (light->getShadowCamera() != nullptr)
							{
								if (pass->hasFragmentProgram())
								{
									Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
									Ogre::Matrix4 textureProjectionMatrix = textureProjMatrix * light->getShadowCamera()->getProjectionMatrixWithRSDepth() * light->getShadowCamera()->getViewMatrix();
									if (params->_findNamedConstantDefinition("shadowViewProjMat"))
										params->setNamedConstant("shadowViewProjMat", textureProjectionMatrix);
								}
							}
						}

						if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
						{
							if (light->getShadowTexture(0) != nullptr)
							{
								tus->setTexture(light->getShadowTexture(0));

								TextureUnitState* tus2 = pass->getTextureUnitState("ShadowMap1");
								TextureUnitState* tus3 = pass->getTextureUnitState("ShadowMap2");
								TextureUnitState* tus4 = pass->getTextureUnitState("ShadowMap3");

								if (tus2 != nullptr) pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus2));
								if (tus3 != nullptr) pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus3));
								if (tus4 != nullptr) pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus4));
							}
						}

						if (light->getType() == Light::LightTypes::LT_POINT)
						{
							if (light->getShadowTexture(0) != nullptr)
								tus->setCubicTexture(&light->getShadowTexture(0), true);

							TextureUnitState* tus2 = pass->getTextureUnitState("ShadowMap1");
							TextureUnitState* tus3 = pass->getTextureUnitState("ShadowMap2");
							TextureUnitState* tus4 = pass->getTextureUnitState("ShadowMap3");

							if (tus2 != nullptr) pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus2));
							if (tus3 != nullptr) pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus3));
							if (tus4 != nullptr) pass->removeTextureUnitState(pass->getTextureUnitStateIndex(tus4));
						}
					}

				}
			}
		}

		injectTechnique(sm, tech, dLight, &ll);
	}
}

void DeferredLightRenderOperation::preRenderTargetUpdate(const RenderTargetEvent & evt)
{
	GetEngine->GetSceneManager()->setSkyBoxEnabled(false);
}

void DeferredLightRenderOperation::postRenderTargetUpdate(const RenderTargetEvent & evt)
{
	GetEngine->GetSceneManager()->setSkyBoxEnabled(true);
}

Ogre::Technique * DeferredLightRenderOperation::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String & schemeName, Ogre::Material * originalMaterial, unsigned short lodIndex, const Ogre::Renderable * rend)
{
	if (schemeName == DEFERRED_SHADOWS_SCHEME_NAME)
	{
		Technique* _tech = originalMaterial->getTechnique(schemeName);

		if (_tech != nullptr)
			return _tech;

		// Get a reference to the original technique
		Ogre::MaterialManager& ogreMaterialManager = Ogre::MaterialManager::getSingleton();
		ogreMaterialManager.setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
		Ogre::Technique* originalTechnique = originalMaterial->getBestTechnique();

		// Store a reference to the shadow caster technique used by the default scheme
		Ogre::Technique* shadowCasterTechnique = shadowCasterMaterial->getTechnique(0);

		// Now, get back to the deferred shadow scheme
		ogreMaterialManager.setActiveScheme(DEFERRED_SHADOWS_SCHEME_NAME);

		// Create a new empty gbuffer technique on the existing material.
		Technique* shadowTechnique = originalMaterial->createTechnique();
		shadowTechnique->setName(DEFERRED_SHADOWS_SCHEME_NAME);
		shadowTechnique->setSchemeName(DEFERRED_SHADOWS_SCHEME_NAME);
		shadowTechnique->removeAllPasses();

		Pass * shadowPass = shadowTechnique->createPass();

		// Set our shadow caster programs
		Ogre::Pass* shadowCasterDefaultPass = shadowCasterTechnique->getPass(0);

		// Start with vertex program
		if (shadowCasterDefaultPass->hasVertexProgram())
		{
			shadowPass->setVertexProgram(shadowCasterDefaultPass->getVertexProgramName(), false);

			const Ogre::GpuProgramPtr& shadowPassVertexProgram = shadowPass->getVertexProgram();
			if (!shadowPassVertexProgram->isLoaded())
			{
				shadowPassVertexProgram->load();
			}

			// Copy params
			shadowPass->setVertexProgramParameters(shadowCasterDefaultPass->getVertexProgramParameters());
		}

		// Continue with fragment program
		if (shadowCasterDefaultPass->hasFragmentProgram())
		{
			shadowPass->setFragmentProgram(shadowCasterDefaultPass->getFragmentProgramName(), false);

			const Ogre::GpuProgramPtr& shadowPassFragmentProgram = shadowPass->getFragmentProgram();
			if (!shadowPassFragmentProgram->isLoaded())
			{
				shadowPassFragmentProgram->load();
			}

			// Copy params
			shadowPass->setFragmentProgramParameters(shadowCasterDefaultPass->getFragmentProgramParameters());
		}

		if (originalTechnique->getNumPasses() > 0)
		{
			Pass* originalPass = originalTechnique->getPass(0);

			if (originalPass->getNumTextureUnitStates() > 0)
			{
				if (shadowPass->getNumTextureUnitStates() == 0)
				{
					shadowPass->createTextureUnitState();
					*shadowPass->getTextureUnitState(0) = *originalPass->getTextureUnitState(0);
				}
			}
		}

		return shadowTechnique;
	}

	return originalMaterial->getTechnique(0);
}

void DeferredLightRenderOperation::UpdateStaticShadowmaps()
{
	auto it = GetEngine->GetSceneManager()->getMovableObjectIterator(LightFactory::FACTORY_TYPE_NAME);

	while (it.hasMoreElements())
	{
		Light* light = (Light*)it.getNext();
		light->frameCount = MIXED_SHADOWS_UPDATE_INTERVAL;
	}
}

void DeferredLightRenderOperation::UpdateShadowmap(DLight* dLight, Camera* cam, Pass* lightingPass)
{
	Light* light = dLight->getParentLight();

	SceneNode* lightNode = light->getParentSceneNode();
	SceneManager* sm = GetEngine->GetSceneManager();

	MonoDomain* domain = SceneManager::monoDomain;

	SceneManager::monoDomain = nullptr;
	if (light->getShadowCamera() == nullptr)
	{
		if (sm->hasCamera("ShadowCamera_" + lightNode->getName()))
			sm->destroyCamera("ShadowCamera_" + lightNode->getName());
		
		Camera* sc = sm->createCamera("ShadowCamera_" + lightNode->getName());
		light->setShadowCamera(sc);
	}

	Camera* shadowCamera = light->getShadowCamera();

	shadowCamera->setFOVy(Degree(90));
	shadowCamera->setAspectRatio(1);
	shadowCamera->setFixedYawAxis(false);
	shadowCamera->setNearClipDistance(cam->getNearClipDistance());
	shadowCamera->setFarClipDistance(cam->getFarClipDistance());

	SceneManager::monoDomain = domain;

	if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
	{
		for (int i = 0; i < SceneManager::shadowCascadesCount; ++i)
		{
			//shadowTextureDirectional[i] = Ogre::TextureManager::getSingleton().getByName("ShadowTextureMapDirectional_" + lightNode->getName() + "_" + to_string(i));

			if (light->getShadowTexture(i) == nullptr)
			{
				if (TextureManager::getSingleton().resourceExists("ShadowTexture_" + lightNode->getName() + "_" + to_string(i), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME))
				{
					TextureManager::getSingleton().remove("ShadowTexture_" + lightNode->getName() + "_" + to_string(i), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				}

				light->setShadowTexture(i, Ogre::TextureManager::getSingleton().createManual("ShadowTexture_" + lightNode->getName() + "_" + to_string(i),
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::TEX_TYPE_2D,
					SceneManager::dirShadowResolution,
					SceneManager::dirShadowResolution,
					0,
					PixelFormat::PF_FLOAT16_R,
					Ogre::TU_RENDERTARGET));
			}

			shadowCamera->setAspectRatio((Real)light->getShadowTexture(i)->getWidth() / (Real)light->getShadowTexture(i)->getHeight());

			// Check rtt viewport
			Ogre::RenderTarget* shadowRTT = light->getShadowTexture(i)->getBuffer()->getRenderTarget();
			//shadowRTT->removeAllViewports();
			Ogre::Viewport* shadowViewport = nullptr;
			shadowRTT->setAutoUpdated(false);

			if (shadowRTT->getNumViewports() > 0)
			{
				shadowViewport = shadowRTT->getViewport(0);
			}
			else
			{
				shadowViewport = shadowRTT->addViewport(shadowCamera);
				shadowViewport->setClearEveryFrame(true);
				shadowViewport->setOverlaysEnabled(false);
				shadowViewport->setSkiesEnabled(false);
				shadowViewport->setBackgroundColour(ColourValue::White);
				shadowViewport->setMaterialScheme(DEFERRED_SHADOWS_SCHEME_NAME);
				shadowViewport->setAutoUpdated(false);
				shadowViewport->setShadowsEnabled(false);
			}

			shadowCamera->setLodCamera(cam);

			// Be sure that the viewport is bind to the correct camera
			assert(shadowViewport != 0);

			shadowCamera->setDirection(light->getDerivedDirection());

			DeferredShadingSystem::CSMCameraSetup->mGpuConstants->currentPass = lightingPass;
			DeferredShadingSystem::CSMCameraSetup->getShadowCamera(sm, cam, mViewport, light, shadowCamera, i);

			//shadowRTT->setDepthBufferPool(2);
			shadowRTT->addListener(this);

			//shadowRTT->update();
			shadowRTT->_beginUpdate();
			shadowRTT->_updateViewport(shadowViewport, false);
			shadowRTT->_endUpdate();

			//shadowRTT->removeAllViewports();
			shadowRTT->removeAllListeners();
			//shadowRTT->detachDepthBuffer();

			//if (i == 0)
			//shadowRTT->writeContentsToFile("../rend2d" + to_string(i) + ".png");
			shadowRTT = nullptr;
			shadowViewport = nullptr;
		}
	}

	if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
	{
		if (light->getShadowTexture(0) == nullptr)
		{
			if (TextureManager::getSingleton().resourceExists("ShadowTexture_" + lightNode->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME))
			{
				TextureManager::getSingleton().remove("ShadowTexture_" + lightNode->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			}

			light->setShadowTexture(0, Ogre::TextureManager::getSingleton().createManual("ShadowTexture_" + lightNode->getName(),
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				Ogre::TEX_TYPE_2D,
				SceneManager::spotShadowResolution,
				SceneManager::spotShadowResolution,
				0,
				PixelFormat::PF_FLOAT16_R,
				Ogre::TU_RENDERTARGET));
		}

		shadowCamera->setAspectRatio((Real)light->getShadowTexture(0)->getWidth() / (Real)light->getShadowTexture(0)->getHeight());

		// Check rtt viewport
		Ogre::RenderTarget* shadowRTT = light->getShadowTexture(0)->getBuffer()->getRenderTarget();
		//shadowRTT->removeAllViewports();
		Ogre::Viewport* shadowViewport = nullptr;
		shadowRTT->setAutoUpdated(false);

		if (shadowRTT->getNumViewports() > 0)
		{
			shadowViewport = shadowRTT->getViewport(0);
		}
		else
		{
			shadowViewport = shadowRTT->addViewport(shadowCamera);
			shadowViewport->setClearEveryFrame(true);
			shadowViewport->setOverlaysEnabled(false);
			shadowViewport->setSkiesEnabled(false);
			shadowViewport->setBackgroundColour(ColourValue::White);
			shadowViewport->setMaterialScheme(DEFERRED_SHADOWS_SCHEME_NAME);
			shadowViewport->setAutoUpdated(false);
			shadowViewport->setShadowsEnabled(false);
		}

		shadowCamera->setLodCamera(cam);

		// Be sure that the viewport is bind to the correct camera
		assert(shadowViewport != 0);

		shadowCamera->setDirection(light->getDerivedDirection());

		shadowCamera->setPosition(light->getDerivedPosition());
		DeferredShadingSystem::DefaultCameraSetup->getShadowCamera(sm, cam, mViewport, light, shadowCamera, 0);

		//shadowRTT->setDepthBufferPool(2);
		shadowRTT->addListener(this);

		//shadowViewport->update();
		//shadowRTT->update();
		shadowRTT->_beginUpdate();
		shadowRTT->_updateViewport(0, false);
		shadowRTT->_endUpdate();

		//shadowRTT->removeAllViewports();
		shadowRTT->removeAllListeners();
		//shadowRTT->detachDepthBuffer();

		//shadowRTT->writeContentsToFile("../rend2d.png");
		shadowRTT = nullptr;
		shadowViewport = nullptr;
	}

	if (light->getType() == Ogre::Light::LT_POINT)
	{
		shadowCamera->setPosition(lightNode->_getDerivedPosition());

		if (light->getShadowTexture(0) == nullptr)
		{
			if (TextureManager::getSingleton().resourceExists("ShadowTexture_" + lightNode->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME))
			{
				TextureManager::getSingleton().remove("ShadowTexture_" + lightNode->getName(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			}

			light->setShadowTexture(0, TextureManager::getSingleton().createManual("ShadowTexture_" + lightNode->getName(),
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				TextureType::TEX_TYPE_CUBE_MAP,
				SceneManager::pointShadowResolution,
				SceneManager::pointShadowResolution,
				0,
				PixelFormat::PF_FLOAT16_R,
				TextureUsage::TU_RENDERTARGET));
		}

		// Assign our camera to all 6 render targets of the texture (1 for each direction)
		for (uint i = 0; i < 6; i++)
		{
			shadowCamera->setOrientation(Quaternion::IDENTITY);
			shadowCamera->setFarClipDistance(light->getAttenuationRange());

			if (i == 0) shadowCamera->yaw(Ogre::Degree(90)); //back
			if (i == 1) shadowCamera->yaw(Ogre::Degree(-90)); //left
			if (i == 2) { shadowCamera->pitch(Ogre::Degree(-90)); shadowCamera->yaw(Ogre::Degree(180)); } //up
			if (i == 3) { shadowCamera->pitch(Ogre::Degree(90)); shadowCamera->yaw(Ogre::Degree(180)); } //down
			if (i == 4) shadowCamera->yaw(Ogre::Degree(180));

			Ogre::RenderTarget* shadowRTT = light->getShadowTexture(0)->getBuffer(i)->getRenderTarget();
			//shadowRTT->removeAllViewports();
			Ogre::Viewport* shadowViewport = nullptr;
			shadowRTT->setAutoUpdated(false);

			if (shadowRTT->getNumViewports() > 0)
			{
				shadowViewport = shadowRTT->getViewport(0);
			}
			else
			{
				shadowViewport = shadowRTT->addViewport(shadowCamera);
				shadowViewport->setClearEveryFrame(true);
				shadowViewport->setOverlaysEnabled(false);
				shadowViewport->setSkiesEnabled(false);
				shadowViewport->setBackgroundColour(ColourValue::White);
				shadowViewport->setMaterialScheme(DEFERRED_SHADOWS_SCHEME_NAME);
				shadowViewport->setAutoUpdated(false);
				shadowViewport->setShadowsEnabled(false);
			}

			shadowCamera->setLodCamera(cam);

			//shadowRTT->setDepthBufferPool(2);
			shadowRTT->addListener(this);

			//shadowViewport->update();
			//shadowRTT->update();
			shadowRTT->_beginUpdate();
			shadowRTT->_updateViewport(0, false);
			shadowRTT->_endUpdate();

			//shadowRTT->removeAllViewports();
			shadowRTT->removeAllListeners();
			//shadowRTT->detachDepthBuffer();

			//shadowRTT->writeContentsToFile("../rend" + to_string(i) + ".png");
			shadowRTT = nullptr;
			shadowViewport = nullptr;
		}
	}
}
