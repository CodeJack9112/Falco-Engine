#include "stdafx.h"
#include "UIManager.h"
#include <OgreRoot.h>
#include <OgreHighLevelGpuProgram.h>

UIManager::UIManager()
{
	GetEngine->GetSceneManager()->addRenderQueueListener(this);
	GetEngine->GetSceneManager()->getRenderQueue()->getQueueGroup(RENDER_QUEUE_8);
}

UIManager::~UIManager()
{
	//GetEngine->GetSceneManager()->removeRenderQueueListener(this);
}

void UIManager::renderQueueStarted(uint8 queueGroupId, const String & invocation, bool & skipThisInvocation)
{
	
}

void UIManager::renderQueueEnded(uint8 queueGroupId, const String & invocation, bool & repeatThisInvocation)
{
	if (GetEngine->GetEnvironment() == Engine::Environment::Player)
	{
		if (GetEngine->loadedScene.empty())
			return;

		if (queueGroupId == RENDER_QUEUE_8)
		{
			sort(renderQueues.begin(), renderQueues.end(), [](const UIRenderQueue* a, const UIRenderQueue* b) -> bool
			{
				return a->index < b->index;
			});

			prepareForRender();

			for (auto it = renderQueues.begin(); it != renderQueues.end(); ++it)
			{
				Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
				
				rs->_setTexture(0, true, (*it)->texture);

				//Texture settings
				Ogre::TextureUnitState::UVWAddressingMode uvwAddressMode;
				uvwAddressMode.u = Ogre::TextureUnitState::TAM_CLAMP;
				uvwAddressMode.v = Ogre::TextureUnitState::TAM_CLAMP;
				uvwAddressMode.w = Ogre::TextureUnitState::TAM_CLAMP;

				rs->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
				rs->_setTextureCoordSet(0, 0);
				rs->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
				rs->_setTextureUnitFiltering(0, Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
				rs->_setTextureAddressingMode(0, uvwAddressMode);
				rs->_setTextureLayerAnisotropy(0, 8);
				rs->_disableTextureUnitsFrom(1);

				rs->setScissorTest(true, (*it)->rect.left, (*it)->rect.top, (*it)->rect.right, (*it)->rect.bottom);
				rs->_render((*it)->renderOp);
				rs->setScissorTest(false);

				delete* it;
			}

			renderQueues.clear();
		}
	}
}

void UIManager::Cleanup()
{
	for (auto it = renderQueues.begin(); it != renderQueues.end(); ++it)
		delete *it;

	renderQueues.clear();
}

void UIManager::prepareForRender()
{
	//Ogre::LayerBlendModeEx alphaBlendMode;
	
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	//alphaBlendMode.blendType = Ogre::LBT_ALPHA;
	//alphaBlendMode.source1 = Ogre::LBS_TEXTURE;
	//alphaBlendMode.operation = Ogre::LBX_BLEND_TEXTURE_ALPHA;

	//rs->_setTextureBlendMode(0, alphaBlendMode);

	rs->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);
	
	rs->setLightingEnabled(false);
	rs->_setFog(Ogre::FOG_NONE);

	rs->_setColourBufferWriteEnabled(true, true, true, false);

	rs->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	rs->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);

	//rs->bindGpuProgram(reinterpret_cast<GpuProgram*>(UICanvas::guiShaderV.getPointer()));
	//rs->bindGpuProgram(reinterpret_cast<GpuProgram*>(UICanvas::guiShaderF.getPointer()));

	//rs->bindGpuProgramParameters(GpuProgramType::GPT_VERTEX_PROGRAM, )

	rs->_setCullingMode(CULL_NONE);
	rs->_setDepthBufferFunction(Ogre::CMPF_ALWAYS_PASS);
	rs->_setSceneBlending(SBF_SOURCE_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
}
