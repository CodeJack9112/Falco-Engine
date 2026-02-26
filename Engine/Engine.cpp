#include "stdafx.h"
#include "Engine.h"
#include "IO.h"
#include "AssetsTools.h"
#include <iostream>
#include "SoundManager.h"
#include "PhysicsManager.h"
#include "InputManager.h"
#include "UIManager.h"
#include "FBXSceneManager.h"
#include "NavigationManager.h"
#include "AnimationList.h"

#include "../boost/algorithm/string.hpp"
#include "DeferredShading/DeferredShading.h"

#include "GUI/ImguiManager.h"
#include "Screen.h"
#include "SceneSerializer.h"

#include "TerrainManager.h"

#include "../Ogre/source/OgreMain/include/Ogre.h"
#include "../Ogre/source/OgreMain/include/OgreCamera.h"
#include "../Ogre/source/OgreMain/include/OgreMesh.h"
#include "../Ogre/source/OgreMain/include/OgreEntity.h"
#include "../Ogre/source/OgreMain/include/OgreRenderQueueListener.h"
#include "../Ogre/source/OgreMain/include/OgreVector3.h"
#include "../Ogre/source/OgreMain/include/OgreQuaternion.h"
#include "../Ogre/source/OgreMain/custom/MonoScript.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include <OgreStaticGeometry.h>
#include "StringConverter.h"
#include "md5.h"
#include "ResourceMap.h"

#include <Ogre/source/RenderSystems/GL/include/OgreGLRenderSystem.h>
#include <Ogre/source/PlugIns/STBICodec/include/OgreSTBICodec.h>
#include <Ogre/source/PlugIns/ParticleFX/include/OgreParticleFXPlugin.h>
#include <Ogre/source/PlugIns/CgProgramManager/include/OgreCgPlugin.h>
#include <Ogre/source/PlugIns/OctreeSceneManager/include/OgreOctreePlugin.h>
#include <imgui_internal.h>
#include "ZipHelper.h"
#include "SkinnedMeshDeformer.h"
#include "SkeletonBone.h"

#include "GUIDGenerator.h"
#include "ProjectSettings.h"

Engine Engine::instance;
ProjectSettings* Engine::projectSettings = new ProjectSettings();

STBIPlugin* imageCodecs = nullptr;
ParticleFXPlugin* particleFX = nullptr;
CgPlugin* cgShaders = nullptr;
OctreePlugin* octree = nullptr;

Engine::Engine() : mRoot(NULL)
{
	nameGenerator = new NameGenerator("Object_");
}

Engine::~Engine(void)
{
	try
	{
		delete nameGenerator;
		delete monoRuntime;
		if (soundManager != nullptr)
			delete soundManager;
		delete physicsManager;
		delete uiManager;
		delete fbxSceneManager;
		delete navigationManager;
		delete projectSettings;

		if (particleFX != nullptr)
		{
			particleFX->shutdown();
			particleFX->uninstall();
		}

		if (cgShaders != nullptr)
		{
			cgShaders->shutdown();
			cgShaders->uninstall();
		}

		if (octree != nullptr)
		{
			octree->shutdown();
			octree->uninstall();
		}
	}
	catch (Exception &e)
	{
		MessageBoxA(0, e.getFullDescription().c_str(), "Engine::~Engine(void)", MB_ICONERROR | MB_OK);
	}
}

bool Engine::Init(Environment env, bool createWindow, string windowName)
{
	//Set environment. Editor or Player
	environment = env;

	loadProjectSettings();

	if (env == Environment::Editor)
	{
		LogManager * lm = new LogManager();
		lm->createLog("Editor.log", true, false, false);
	}
	else
	{
		LogManager * lm = new LogManager();
		lm->createLog("Engine.log", true, false, false);
	}

	inputManager = new InputManager();

	mRoot = new Root();

	GLRenderSystem * _mRenderSystem = OGRE_NEW GLRenderSystem();
	Root::getSingleton().addRenderSystem(_mRenderSystem);

	// Register codecs
	imageCodecs = new STBIPlugin();
	imageCodecs->install();
	
	particleFX = new ParticleFXPlugin();
	particleFX->install();

	cgShaders = new CgPlugin();
	cgShaders->install();
	cgShaders->initialise();

	octree = new OctreePlugin();
	octree->install();
	octree->initialise();

	RenderSystem* pSystem = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");//mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	pSystem->setConfigOption("Full Screen", "No");
	pSystem->setConfigOption("VSync", "No");
	pSystem->setConfigOption("Video Mode", "1280 x 800 @ 32-bit colour");
	mRoot->setRenderSystem(pSystem);
	mRenderSystem = pSystem;
	RenderWindow * wnd = mRoot->initialise(createWindow, windowName);

	if (createWindow)
		SetRenderWindow(wnd);

	Root::getSingletonPtr()->addFrameListener(this);

	//Create scene manager
	SceneManager *mSceneMgr = Root::getSingletonPtr()->createSceneManager("OctreeSceneManager");
	GetEngine->SetSceneManager(mSceneMgr);

	std::vector<std::function<void()>>::iterator ci;
	for (ci = AfterRootCreated.begin(); ci < AfterRootCreated.end(); ++ci)
		(*ci)();

	FontManager * fontMgr = new FontManager();

	//Empty object factory needed for empty objects
	emptyObjectFactory = new EmptyObjectFactory();
	Root::getSingletonPtr()->addMovableObjectFactory(emptyObjectFactory);

	uiCanvasFactory = new UICanvasFactory();
	Root::getSingletonPtr()->addMovableObjectFactory(uiCanvasFactory);

	uiButtonFactory = new UIButtonFactory();
	Root::getSingletonPtr()->addMovableObjectFactory(uiButtonFactory);

	uiTextFactory = new UITextFactory();
	Root::getSingletonPtr()->addMovableObjectFactory(uiTextFactory);

	uiImageFactory = new UIImageFactory();
	Root::getSingletonPtr()->addMovableObjectFactory(uiImageFactory);

	UICanvasFactory::manager = mSceneMgr;

	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
	MaterialManager::getSingleton().setDefaultAnisotropy(8);

	//Scene
	mSceneMgr->setAmbientLight(ColourValue(0.15, 0.15, 0.15, 1.0));
	
	//Initialize sound
	if (environment == Environment::Player)
	{
		soundManager = new SoundManager();
		soundManager->Init();
	}

	//Initialize physics
	physicsManager = new PhysicsManager();
	physicsManager->Init();

	uiManager = new UIManager();

	navigationManager = new NavigationManager();

	//Initialize scripting engine
	monoRuntime = new MonoRuntime(monoPath, assemblyPath);
	monoRuntime->SetSceneManager(mSceneMgr);

	sg = mSceneManager->createStaticGeometry("StaticGeometry");

	initialized = true;

	return true;
}

void Engine::SetRenderWindow(RenderWindow* window)
{
	renderWindow = window;
	SceneManager::renderWindow = renderWindow;
}

RenderSystem* Engine::GetCurrentRenderSystem()
{
	return mRenderSystem;
}

void Engine::SetSceneManager(SceneManager * mgr)
{
	mSceneManager = mgr;
}

SceneManager * Engine::GetSceneManager()
{
	return mSceneManager;
}

std::string Engine::GenerateNewName(SceneManager* manager, std::string prefix)
{
	Ogre::String name = prefix + nameGenerator->generate();

	while (manager->hasSceneNode(name))
		name = prefix + nameGenerator->generate();

	return name;
}

bool Engine::frameStarted(const FrameEvent & evt)
{
	if (updateFrame)
	{
		deltaTime = evt.timeSinceLastFrame;
		if (deltaTime < 0.0)
			deltaTime = 0.0;

		if (drawGUI)
		{
			Vector2 screenSize(Screen::GetScreenSize().x, Screen::GetScreenSize().y);
			Ogre::ImguiManager::getSingleton().newFrame(evt.timeSinceLastFrame, Ogre::Rect(0, 0, screenSize.x, screenSize.y));

			//Execute mono functions
			if (environment == Environment::Player)
			{
				bool main_window = true;
				ImGui::SetNextWindowPos(ImVec2(0, 0));
				ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y));
				ImGui::SetNextWindowBgAlpha(0.0);
				ImGui::Begin("", &main_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);

				monoRuntime->Execute("OnGUI");
				monoRuntime->Execute("BeginFrame");

				if (ImGui::GetCurrentContext()->CurrentWindow != nullptr)
					ImGui::End();
			}
			else
			{
				if (ImGUICallback != nullptr)
				{
					ImGUICallback();
				}

				//ImGui::ShowDemoWindow();
			}
		}

		//Update paged geometry
		GetTerrainManager()->UpdatePagedGeometry();

		//Update animations
		if (environment == Engine::Environment::Player)
			AnimationInternalUpdateRecursive(mSceneManager->getRootSceneNode(), evt.timeSinceLastFrame * timeScale);

		UpdateMaterials();
	}

	return true;
}

bool Engine::frameEnded(const FrameEvent& evt)
{
	if (updateFrame)
	{
		if (environment == Environment::Player)
		{
			monoRuntime->Execute("EndFrame");
		}

		//UpdateLightmapsEnd();
	}

	return true;
}

void Engine::SetAssetsPath(std::string path)
{
	assetsPath = path;
}

void Engine::SetMonoPath(std::string path)
{
	monoPath = path;
}

void Engine::SetRootPath(std::string path)
{
	rootPath = path;
}

void Engine::SetBuiltinResourcesPath(std::string path)
{
	builtinResourcesPath = path;
}

void Engine::UpdateSceneNodeIndexes()
{
	SceneNode * root = GetSceneManager()->getRootSceneNode();

	int index = -1;
	updateSceneNodeIndexes(root, index);
}

void Engine::updateSceneNodeIndexes(SceneNode* root, int& start)
{
	Node::ChildNodeIterator itr = root->getChildIterator();

	while (itr.hasMoreElements())
	{
		Node* child = itr.getNext();

		//if (child->getName() != "Gizmo" && child->getName() != "EditorGrid" && child->getName() != "EditorGrid_2" && child->getName() != "EditorCamera")
		if (!IsEditorObject((SceneNode*)child))
		{
			//if (child->getName().find("_gizmo") != string::npos)
			//	continue;

			SceneNode::ObjectIterator it = ((SceneNode*)child)->getAttachedObjectIterator();

			while (it.hasMoreElements())
			{
				++start;

				child->index = start;

				MovableObject* movObj = it.getNext();

				if (IsObjectUI(movObj))
				{
					MaterialPtr material = ((UIElement*)movObj)->GetMaterial();

					if (material != nullptr)
					{
						movObj->setRenderQueueGroupAndPriority(RENDER_QUEUE_6 - 1, start);
					}
				}
			}
		}

		updateSceneNodeIndexes((SceneNode*)child, start);
	}
}

bool Engine::IsObjectUI(MovableObject * obj)
{
	string type = obj->getMovableType();
	bool isCanvas = obj->getMovableType() == UICanvasFactory::FACTORY_TYPE_NAME;
	bool is2D = type == UIButtonFactory::FACTORY_TYPE_NAME || type == UITextFactory::FACTORY_TYPE_NAME || type == UIImageFactory::FACTORY_TYPE_NAME || isCanvas;

	return is2D;
}

bool Engine::IsObjectUIElement(MovableObject * obj)
{
	return IsObjectUI(obj) && obj->getMovableType() != UICanvasFactory::FACTORY_TYPE_NAME;
}

void Engine::UpdateGpuProgramParams(Pass * pass)
{
	if (pass->hasVertexProgram())
	{
		GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();

		if (params->_findNamedConstantDefinition("worldViewProj") != nullptr)
			params->setNamedAutoConstant("worldViewProj", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEWPROJ_MATRIX);

		if (params->_findNamedConstantDefinition("worldView") != nullptr)
			params->setNamedAutoConstant("worldView", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEW_MATRIX);

		if (params->_findNamedConstantDefinition("world") != nullptr)
			params->setNamedAutoConstant("world", GpuProgramParameters::AutoConstantType::ACT_WORLD_MATRIX);

		if (params->_findNamedConstantDefinition("view") != nullptr)
			params->setNamedAutoConstant("view", GpuProgramParameters::AutoConstantType::ACT_VIEW_MATRIX);

		if (params->_findNamedConstantDefinition("projection") != nullptr)
			params->setNamedAutoConstant("projection", GpuProgramParameters::AutoConstantType::ACT_PROJECTION_MATRIX);

		if (params->_findNamedConstantDefinition("cameraPosition") != nullptr)
			params->setNamedAutoConstant("cameraPosition", GpuProgramParameters::AutoConstantType::ACT_CAMERA_POSITION);

		if (params->_findNamedConstantDefinition("nearClipDistance") != nullptr)
			params->setNamedAutoConstant("nearClipDistance", GpuProgramParameters::AutoConstantType::ACT_NEAR_CLIP_DISTANCE);

		if (params->_findNamedConstantDefinition("farClipDistance") != nullptr)
			params->setNamedAutoConstant("farClipDistance", GpuProgramParameters::AutoConstantType::ACT_FAR_CLIP_DISTANCE);

		if (params->_findNamedConstantDefinition("ambientColor") != nullptr)
			params->setNamedAutoConstant("ambientColor", GpuProgramParameters::AutoConstantType::ACT_AMBIENT_LIGHT_COLOUR);
	}

	if (pass->hasFragmentProgram())
	{
		GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

		if (params->_findNamedConstantDefinition("worldViewProj") != nullptr)
			params->setNamedAutoConstant("worldViewProj", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEWPROJ_MATRIX);

		if (params->_findNamedConstantDefinition("worldView") != nullptr)
			params->setNamedAutoConstant("worldView", GpuProgramParameters::AutoConstantType::ACT_WORLDVIEW_MATRIX);

		if (params->_findNamedConstantDefinition("world") != nullptr)
			params->setNamedAutoConstant("world", GpuProgramParameters::AutoConstantType::ACT_WORLD_MATRIX);

		if (params->_findNamedConstantDefinition("view") != nullptr)
			params->setNamedAutoConstant("view", GpuProgramParameters::AutoConstantType::ACT_VIEW_MATRIX);

		if (params->_findNamedConstantDefinition("projection") != nullptr)
			params->setNamedAutoConstant("projection", GpuProgramParameters::AutoConstantType::ACT_PROJECTION_MATRIX);

		if (params->_findNamedConstantDefinition("cameraPosition") != nullptr)
			params->setNamedAutoConstant("cameraPosition", GpuProgramParameters::AutoConstantType::ACT_CAMERA_POSITION);

		if (params->_findNamedConstantDefinition("nearClipDistance") != nullptr)
			params->setNamedAutoConstant("nearClipDistance", GpuProgramParameters::AutoConstantType::ACT_NEAR_CLIP_DISTANCE);

		if (params->_findNamedConstantDefinition("farClipDistance") != nullptr)
			params->setNamedAutoConstant("farClipDistance", GpuProgramParameters::AutoConstantType::ACT_FAR_CLIP_DISTANCE);

		if (params->_findNamedConstantDefinition("ambientColor") != nullptr)
			params->setNamedAutoConstant("ambientColor", GpuProgramParameters::AutoConstantType::ACT_AMBIENT_LIGHT_COLOUR);
	}
}

void Engine::UpdateShadowTextureUnits(Pass* pass)
{
	if (pass->getParent()->getSchemeName() == MaterialManager::DEFAULT_SCHEME_NAME)
	{
		if (pass->hasFragmentProgram())
		{
			GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
			TextureUnitState* state = pass->getTextureUnitState("shadowMap0");
			if (state == nullptr)
			{
				state = pass->createTextureUnitState();
				state->setName("shadowMap0");
				state->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
			}

			if (params->_findNamedConstantDefinition("shadowMap0") != nullptr)
			{
				int tui = pass->getTextureUnitStateIndex(state);
				params->setNamedConstant("shadowMap0", tui);
			}

			state = pass->getTextureUnitState("shadowMap1");
			if (state == nullptr)
			{
				state = pass->createTextureUnitState();
				state->setName("shadowMap1");
				state->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
			}

			if (params->_findNamedConstantDefinition("shadowMap1") != nullptr)
			{
				int tui = pass->getTextureUnitStateIndex(state);
				params->setNamedConstant("shadowMap1", tui);
			}

			state = pass->getTextureUnitState("shadowMap2");
			if (state == nullptr)
			{
				state = pass->createTextureUnitState();
				state->setName("shadowMap2");
				state->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
			}

			if (params->_findNamedConstantDefinition("shadowMap2") != nullptr)
			{
				int tui = pass->getTextureUnitStateIndex(state);
				params->setNamedConstant("shadowMap2", tui);
			}

			state = pass->getTextureUnitState("shadowMap3");
			if (state == nullptr)
			{
				state = pass->createTextureUnitState();
				state->setName("shadowMap3");
				state->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
			}

			if (params->_findNamedConstantDefinition("shadowMap3") != nullptr)
			{
				int tui = pass->getTextureUnitStateIndex(state);
				params->setNamedConstant("shadowMap3", tui);
			}

			state = pass->getTextureUnitState("shadowMapPoint");
			if (state == nullptr)
			{
				state = pass->createTextureUnitState();
				state->setName("shadowMapPoint");
				state->setContentType(TextureUnitState::ContentType::CONTENT_SHADOW);
			}

			if (params->_findNamedConstantDefinition("shadowMapPoint") != nullptr)
			{
				int tui = pass->getTextureUnitStateIndex(state);
				params->setNamedConstant("shadowMapPoint", tui);
			}
		}
	}
}

void Engine::UpdateMaterials()
{
	MapIterator it = mSceneManager->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

	while (it.hasMoreElements())
	{
		Entity* entity = (Entity*)it.getNext();
		UpdateMaterials(entity);
	}

	it = mSceneManager->getMovableObjectIterator("StaticGeometry");

	StaticGeometry* sg = GetEngine->getStaticGeometry();
	auto reg = sg->getRegionIterator();

	while (reg.hasMoreElements())
	{
		StaticGeometry::Region* staticRegion = reg.getNext();
		UpdateMaterials(staticRegion);
	}

	//Update skybox material
	MaterialPtr skyboxMaterial = mSceneManager->getSkyBoxMaterial();
	if (skyboxMaterial != nullptr)
	{
		for (int i = 0; i < skyboxMaterial->getNumTechniques(); ++i)
		{
			Technique* tech = skyboxMaterial->getTechnique(i);

			for (int j = 0; j < tech->getNumPasses(); ++j)
			{
				Pass* pass = tech->getPass(j);

				UpdateGpuProgramParams(pass);
			}
		}
	}
}

void Engine::UpdateMaterials(Entity* entity)
{
	SceneNode* sceneNode = entity->getParentSceneNode();

	if (sceneNode == nullptr)
		return;

	if (IsEditorObject(sceneNode))
		return;

	for (int i = 0; i < entity->getNumSubEntities(); ++i)
	{
		SubEntity* ent = entity->getSubEntity(i);
		MaterialPtr mMaterialPtr = ent->getMaterial();

		if (mMaterialPtr->getName().find("GrassVS_anim") != string::npos)
			continue;

		if (mMaterialPtr != nullptr && mMaterialPtr->getName() != "BaseWhite")
		{
			if (ent->getOriginalMaterial() == nullptr)
			{
				String _n = mMaterialPtr->getName() + "_" + entity->getParentSceneNode()->getName() + "_Instance_" + to_string(i);

				ent->setOriginalMaterial(mMaterialPtr);

				if (MaterialManager::getSingleton().resourceExists(_n, "Assets"))
					mMaterialPtr = MaterialManager::getSingleton().getByName(_n, "Assets");
				else if (MaterialManager::getSingleton().resourceExists(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
					mMaterialPtr = MaterialManager::getSingleton().getByName(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
				else
					mMaterialPtr = mMaterialPtr->clone(_n);

				while (mMaterialPtr->getNumTechniques() > 1)
					mMaterialPtr->removeTechnique(1);

				ent->setMaterial(mMaterialPtr);
				UpdateShaders(mMaterialPtr);
			}
		}
		else
		{
			if (ent->getOriginalMaterial() != nullptr)
			{
				MaterialPtr origMat = ent->getOriginalMaterial();

				String _n = origMat->getName() + "_" + entity->getParentSceneNode()->getName() + "_Instance_" + to_string(i);

				if (MaterialManager::getSingleton().resourceExists(_n, "Assets"))
					mMaterialPtr = MaterialManager::getSingleton().getByName(_n, "Assets");
				else if (MaterialManager::getSingleton().resourceExists(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
					mMaterialPtr = MaterialManager::getSingleton().getByName(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
				else
					mMaterialPtr = ent->getOriginalMaterial()->clone(_n);

				while (mMaterialPtr->getNumTechniques() > 1)
					mMaterialPtr->removeTechnique(1);

				ent->setMaterial(mMaterialPtr);
				UpdateShaders(mMaterialPtr);
			}
		}

		for (int i = 0; i < mMaterialPtr->getNumTechniques(); ++i)
		{
			Technique* tech = mMaterialPtr->getTechnique(i);
			
			for (int j = 0; j < tech->getNumPasses(); ++j)
			{
				Pass* pass = tech->getPass(j);

				//Setup lightmaps
				if (sceneNode->lightmapStatic && ent->getLightmapTexture() != nullptr)
				{
					if (pass->hasFragmentProgram())
					{
						GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

						TextureUnitState* state = pass->getTextureUnitState("Lightmap");
						if (state == nullptr)
						{
							state = pass->createTextureUnitState();
							state->setName("Lightmap");
						}

						if (params->_findNamedConstantDefinition("useLightMap") != nullptr)
						{
							params->setNamedConstant("useLightMap", 1);

							if (params->_findNamedConstantDefinition("lightMap") != nullptr)
							{
								state->setTexture(ent->getLightmapTexture());

								int tui = pass->getTextureUnitStateIndex(state);
								params->setNamedConstant("lightMap", tui);
							}
						}
					}
				}
				else
				{
					if (ent->getOriginalMaterial() != nullptr)
					{
						TextureUnitState* state = pass->getTextureUnitState("Lightmap");
						if (state != nullptr)
						{
							int tui = pass->getTextureUnitStateIndex(state);
							pass->removeTextureUnitState(tui);

							GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

							if (params->_findNamedConstantDefinition("useLightMap") != nullptr)
							{
								params->setNamedConstant("useLightMap", 0);
							}
						}
					}
				}

				UpdateShadowTextureUnits(pass);
				UpdateGpuProgramParams(pass);
			}
		}
	}
}

void Engine::UpdateMaterials(StaticGeometry::Region* staticRegion)
{
	auto lods = staticRegion->getLODIterator();

	while (lods.hasMoreElements())
	{
		StaticGeometry::LODBucket* lod = lods.getNext();

		auto mats = lod->getMaterialIterator();
		int i = 0;

		while (mats.hasMoreElements())
		{
			auto mat = mats.getNext();

			//MaterialPtr mMaterialPtr = mat->getMaterial();

			auto qGeom = mat->getGeometryIterator();
			int _g = 0;
			while (qGeom.hasMoreElements())
			{
				auto git = qGeom.getNext();

				MaterialPtr mMaterialPtr = git->getMaterial();

				if (mMaterialPtr != nullptr && mMaterialPtr->getName() != "BaseWhite")
				{
					if (git->getOriginalMaterial() == nullptr)
					{
						String _n = mMaterialPtr->getName() + "_StaticGeometry_Instance_" + to_string(staticRegion->getID()) + "_" + to_string(i) + "_" + to_string(_g);

						git->setOriginalMaterial(mMaterialPtr);

						if (MaterialManager::getSingleton().resourceExists(_n, "Assets"))
							mMaterialPtr = MaterialManager::getSingleton().getByName(_n, "Assets");
						else if (MaterialManager::getSingleton().resourceExists(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
							mMaterialPtr = MaterialManager::getSingleton().getByName(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
						else
							mMaterialPtr = mMaterialPtr->clone(_n);

						while (mMaterialPtr->getNumTechniques() > 1)
							mMaterialPtr->removeTechnique(1);

						git->setMaterial(mMaterialPtr);
						//git->setMaterialName(_n);

						UpdateShaders(mMaterialPtr);
					}
				}
				else
				{
					if (git->getOriginalMaterial() != nullptr)
					{
						MaterialPtr origMat = git->getOriginalMaterial();

						String _n = origMat->getName() + "_StaticGeometry_Instance_" + to_string(staticRegion->getID()) + "_" + to_string(i) + "_" + to_string(_g);

						if (MaterialManager::getSingleton().resourceExists(_n, "Assets"))
							mMaterialPtr = MaterialManager::getSingleton().getByName(_n, "Assets");
						else if (MaterialManager::getSingleton().resourceExists(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
							mMaterialPtr = MaterialManager::getSingleton().getByName(_n, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
						else
							mMaterialPtr = git->getOriginalMaterial()->clone(_n);

						while (mMaterialPtr->getNumTechniques() > 1)
							mMaterialPtr->removeTechnique(1);

						git->setMaterial(mMaterialPtr);
						//git->setMaterialName(_n);

						UpdateShaders(mMaterialPtr);
					}
				}

				for (int i = 0; i < mMaterialPtr->getNumTechniques(); ++i)
				{
					Technique* tech = mMaterialPtr->getTechnique(i);

					for (int j = 0; j < tech->getNumPasses(); ++j)
					{
						Pass* pass = tech->getPass(j);

						//Setup lightmaps
						if (git->getLightmapTexture() != nullptr)
						{
							if (pass->hasFragmentProgram())
							{
								GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

								TextureUnitState* state = pass->getTextureUnitState("Lightmap");
								if (state == nullptr)
								{
									state = pass->createTextureUnitState();
									state->setName("Lightmap");
								}

								if (params->_findNamedConstantDefinition("useLightMap") != nullptr)
								{
									params->setNamedConstant("useLightMap", 1);

									if (params->_findNamedConstantDefinition("lightMap") != nullptr)
									{
										state->setTexture(git->getLightmapTexture());

										int tui = pass->getTextureUnitStateIndex(state);
										params->setNamedConstant("lightMap", tui);
									}
								}
							}
						}
						else
						{
							if (git->getOriginalMaterial() != nullptr)
							{
								TextureUnitState* state = pass->getTextureUnitState("Lightmap");
								if (state != nullptr)
								{
									int tui = pass->getTextureUnitStateIndex(state);
									pass->removeTextureUnitState(tui);

									GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

									if (params->_findNamedConstantDefinition("useLightMap") != nullptr)
									{
										params->setNamedConstant("useLightMap", 0);
									}
								}
							}
						}

						UpdateShadowTextureUnits(pass);
						UpdateGpuProgramParams(pass);
					}
				}

				++_g;
			}

			++i;
		}
	}
}

void Engine::UpdateShaders(MaterialPtr material)
{
	for (int i = 0; i < material->getNumTechniques(); ++i)
	{
		Technique* tech = material->getTechnique(i);

		for (int i = 0; i < tech->getNumPasses(); ++i)
		{
			Pass* pass = tech->getPass(i);

			UpdateShadersInPass(pass);
		}

		if (!material->isLoaded())
			material->load();
	}
}

void Engine::UpdateShadersInPass(Pass* pass)
{
	if (pass->GetPreprocessorDefines().size() == 0)
		return;

	std::string _defines = "";
	std::string defines = "";
	int j = 0;
	int cnt = 0;

	for (auto it = pass->GetPreprocessorDefines().begin(); it != pass->GetPreprocessorDefines().end(); ++it)
	{
		if (it->second)
		{
			++cnt;
		}
	}

	if (cnt == 0)
		return;

	for (auto it = pass->GetPreprocessorDefines().begin(); it != pass->GetPreprocessorDefines().end(); ++it)
	{
		if (it->second)
		{
			_defines += it->first;
			defines += it->first;

			++j;

			if (j < cnt)
			{
				defines += ",";
				_defines += "_";
			}
		}
	}

	//Vertex program
	if (pass->hasVertexProgram())
	{
		HighLevelGpuProgramPtr vert = static_pointer_cast<HighLevelGpuProgram>(pass->getVertexProgram());

		if (!vert->getInstanced())
		{
			String name = vert->getName() + "_" + _defines;

			if (HighLevelGpuProgramManager::getSingleton().resourceExists(name, "Assets"))
			{
				vert = HighLevelGpuProgramManager::getSingleton().getByName(name, "Assets");
				//HighLevelGpuProgramManager::getSingleton().remove(name, "Assets");
			}
			else
			{
				String mMasterSource = "";

				if (!vert->getSourceFile().empty())
				{
					try
					{
						DataStreamPtr ptrMasterSource = ResourceGroupManager::getSingleton().openResource(vert->getSourceFile(), "Assets");
						mMasterSource = ptrMasterSource->getAsString();
					}
					catch (...) {}
				}
				else
				{
					string nm = assetsPath + vert->getName();
					if (IO::FileExists(nm))
					{
						string src = IO::ReadText(nm);
						mMasterSource = src;
					}
					else
						mMasterSource = vert->getSource();
				}

				vert = HighLevelGpuProgramManager::getSingleton().createProgram(name, "Assets", "glsl", GPT_VERTEX_PROGRAM);
				vert->setParameter("profiles", "glsl120");

				vert->setSource(mMasterSource);
				vert->setParameter("preprocessor_defines", defines);
				vert->setInstanced(true);
			}

			if (!vert->isLoaded())
				vert->load();

			pass->setVertexProgram(vert->getName(), false);
		}
	}

	//Fragment program
	if (pass->hasFragmentProgram())
	{
		HighLevelGpuProgramPtr frag = static_pointer_cast<HighLevelGpuProgram>(pass->getFragmentProgram());

		if (!frag->getInstanced())
		{
			String name = frag->getName() + "_" + _defines;

			if (HighLevelGpuProgramManager::getSingleton().resourceExists(name, "Assets"))
			{
				frag = HighLevelGpuProgramManager::getSingleton().getByName(name, "Assets");
				//HighLevelGpuProgramManager::getSingleton().remove(name, "Assets");
			}
			else
			{
				String mMasterSource = "";

				if (!frag->getSourceFile().empty())
				{
					try
					{
						DataStreamPtr ptrMasterSource = ResourceGroupManager::getSingleton().openResource(frag->getSourceFile(), "Assets");
						mMasterSource = ptrMasterSource->getAsString();
					}
					catch (...) {}
				}
				else
				{
					string nm = assetsPath + frag->getName();
					if (IO::FileExists(nm))
					{
						string src = IO::ReadText(nm);
						mMasterSource = src;
					}
					else
						mMasterSource = frag->getSource();
				}

				frag = HighLevelGpuProgramManager::getSingleton().createProgram(name, "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
				frag->setParameter("profiles", "glsl120");

				frag->setSource(mMasterSource);
				frag->setParameter("preprocessor_defines", defines);
				frag->setInstanced(true);
			}

			if (!frag->isLoaded())
				frag->load();

			pass->setFragmentProgram(frag->getName(), false);
		}
	}
}

void Engine::ClearMaterialsInstances()
{
	MapIterator it = mSceneManager->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

	while (it.hasMoreElements())
	{
		Entity* entity = (Entity*)it.getNext();
		ClearMaterialInstance(entity);
	}
}

void Engine::ClearMaterialInstance(Entity* entity)
{
	SceneNode* sceneNode = entity->getParentSceneNode();

	if (sceneNode == nullptr)
		return;

	if (IsEditorObject(sceneNode))
		return;

	for (int i = 0; i < entity->getNumSubEntities(); ++i)
	{
		SubEntity* ent = entity->getSubEntity(i);
		MaterialPtr mMaterialPtr = ent->getMaterial();

		if (mMaterialPtr == nullptr)
			continue;

		if (mMaterialPtr->getName().find("GrassVS_anim") != string::npos)
			continue;

		if (mMaterialPtr != nullptr && mMaterialPtr->getName() != "BaseWhite")
		{
			//ent->setMaterial(MaterialPtr());
			if (ent->getOriginalMaterial() != nullptr)
			{
				if (mMaterialPtr != MaterialManager::getSingleton().getDefaultMaterial())
				{
					try
					{
						MaterialManager::getSingleton().unload(mMaterialPtr->getHandle());
						MaterialManager::getSingleton().remove(mMaterialPtr->getHandle());
					}
					catch (...) {}
				}

				ent->setMaterial(ent->getOriginalMaterial());
				ent->setOriginalMaterial(MaterialPtr());
			}
		}
	}
}

void Engine::Destroy(SceneNode * node)
{
	instance.deleteArray.clear();

	if (node != instance.mSceneManager->getRootSceneNode())
	{
		instance.deleteArray.push_back(node);
	}

	//instance.BreakHierarchy(node);
	instance.DeleteAll(node);

	for (std::vector<SceneNode*>::iterator del = instance.deleteArray.begin(); del != instance.deleteArray.end(); ++del)
	{
		AnimationList * animList = (AnimationList*)(*del)->GetComponent(AnimationList::COMPONENT_TYPE);

		if (animList != nullptr)
		{
			for (AnimationList::AnimationDataList::iterator ait = animList->GetAnimationDataList().begin(); ait != animList->GetAnimationDataList().end(); ++ait)
			{
				std::string sourceFileName = ait->fileName;
				std::string fullName = sourceFileName + "_" + node->getName() + "_anim_" + ait->name;

				MapIterator entities = instance.mSceneManager->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

				while (entities.hasMoreElements())
				{
					Entity * ent = (Entity*)entities.getNext();

					if (ent->hasAnimationState(fullName))
					{
						AnimationState * animStateEnt = ent->getAnimationState(fullName);
						String animName = animStateEnt->getAnimationName();
						
						if (ent->hasSkeleton())
						{
							if (ent->getSkeleton()->hasAnimation(animName))
							{
								ent->getSkeleton()->removeAnimation(animName);
							}
						}
					}
				}

				if (instance.mSceneManager->hasAnimationState(fullName))
				{
					AnimationState * animState = instance.mSceneManager->getAnimationState(fullName);
					String animName = animState->getAnimationName();

					if (instance.mSceneManager->hasAnimation(animName))
					{
						instance.mSceneManager->destroyAnimation(animName);
					}
				}
			}
		}

		if ((*del)->getParent() != nullptr)
		{
			(*del)->getParent()->removeChild(*del);
			//instance.mSceneManager->getRootSceneNode()->addChild(*del);
		}

		//GetEngine->GetMonoRuntime()->DestroyInstances(*del);

		DestroyAttachedObjects(*del);

		instance.mSceneManager->destroySceneNode(*del);
	}
}

void Engine::DeleteAll(SceneNode * root)
{
	Node::ChildNodeIterator itr = root->getChildIterator();
	while (itr.hasMoreElements())
	{
		SceneNode* child = static_cast<SceneNode*>(itr.getNext());

		if (child->getName() != "Gizmo" && child->getName() != "EditorGrid" && child->getName() != "EditorGrid_2" && child->getName() != "EditorCamera" && child->getName() != "SelectionRoot_gizmo" && child->getName() != "SelectionBox_node_gizmo")
		{
			instance.deleteArray.push_back(child);
			DeleteAll(child);
		}
	}
}

bool Engine::IsEditorObject(SceneNode* node)
{
	std::vector<std::string> names = { "Gizmo",
		"EditorGrid",
		"EditorGrid_2",
		"EditorCamera",
		"SelectionRoot_gizmo",
		"SelectionBox_node_gizmo",
		"move_widget_z",
		"move_widget_x",
		"move_widget_y",
		"move_widget_xz",
		"move_widget_xy",
		"move_widget_zy",
		"rot_widget_x",
		"rot_widget_y",
		"rot_widget_z",
		"scl_widget_x",
		"scl_widget_y",
		"scl_widget_z",
		"scl_widget_xyz"
	};

	std::vector<std::string> names_2 = { "_gizmo" };
	bool _names2 = false;

	std::string nm = node->getName();
	if (nm == "")
		return false;

	for (std::vector<std::string>::iterator it = names_2.begin(); it != names_2.end(); ++it)
	{
		if (nm.find(*it) != string::npos)
		{
			_names2 = true;
			break;
		}
	}

	return std::find(names.begin(), names.end(), nm) != names.end() || _names2;
}

bool Engine::IsEditorMesh(MeshPtr mesh)
{
	std::vector<std::string> names = { "Gizmo/arrow.mesh",
		"Gizmo/plane.mesh",
		"Gizmo/circle.mesh",
		"Gizmo/scale.mesh",
		"Gizmo/cube.mesh"
	};

	std::string nm = mesh->getName();
	if (nm == "")
		return false;

	return std::find(names.begin(), names.end(), nm) != names.end();
}

bool Engine::IsEditorEntity(Entity* entity)
{
	std::vector<std::string> names = { "move_widget_z",
		"move_widget_x",
		"move_widget_y",
		"move_widget_xz",
		"move_widget_xy"
		"move_widget_zy"
		"rot_widget_x"
		"rot_widget_y"
		"rot_widget_z"
		"scl_widget_x"
		"scl_widget_y"
		"scl_widget_z"
		"scl_widget_xyz"
	};

	std::string nm = entity->getName();
	if (nm == "")
		return false;

	return std::find(names.begin(), names.end(), nm) != names.end();
}

void Engine::GetAllChildren(SceneNode* root, std::vector<SceneNode*>& outList)
{
	VectorIterator it = root->getChildIterator();
	while (it.hasMoreElements())
	{
		SceneNode* n = (SceneNode*)it.getNext();
		outList.push_back(n);

		GetAllChildren(n, outList);
	}
}

void Engine::RenderOneFrameWithoutGUI()
{
	drawGUI = false;
	Root::getSingleton().renderOneFrame();
	drawGUI = true;
}

void Engine::buildStaticGeometry(std::string lightmapsPath, bool buildLightmaps, bool destroyEntities)
{
	StaticGeometry* sg = getStaticGeometry();
	sg->reset();

	sg->setLightmapTextureSize(regionLightmapSize);

	std::vector<SceneNode*> allNodes;
	GetAllChildren(mSceneManager->getRootSceneNode(), allNodes);

	for (auto ssi = allNodes.begin(); ssi != allNodes.end(); ++ssi)
	{
		SceneNode* sn = *ssi;

		if (sn->staticBatching)
		{
			if (sn->getVisible())
			{
				if (sn->getAttachedObjects().size() > 0)
				{
					MovableObject* obj = sn->getAttachedObject(0);
					if (obj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
					{
						Entity* ent = (Entity*)obj;

						sg->addEntity(ent, sn->_getDerivedPosition(), sn->_getDerivedOrientation(), sn->_getDerivedScale());

						if (destroyEntities)
						{
							sn->detachObject(obj);
							mSceneManager->destroyMovableObject(obj);
						}
					}
				}
			}
		}
	}

	sg->build();

	if (buildLightmaps)
	{
		auto reg = sg->getRegionIterator();

		while (reg.hasMoreElements())
		{
			StaticGeometry::Region* region = reg.getNext();

			auto lit = region->getLODIterator();
			while (lit.hasMoreElements())
			{
				StaticGeometry::LODBucket* lod = lit.getNext();

				auto mtx = lod->getMaterialIterator();

				int _m = 0;
				while (mtx.hasMoreElements())
				{
					auto mtb = mtx.getNext();
					
					auto qGeom = mtb->getGeometryIterator();
					int _g = 0;
					while (qGeom.hasMoreElements())
					{
						auto git = qGeom.getNext();

						string lightmapName = "RegionLightmap_" + to_string(region->getID()) + "_" + to_string(_m) + "_" + to_string(_g);

						if (TextureManager::getSingleton().resourceExists(lightmapName, "Assets"))
							TextureManager::getSingleton().remove(lightmapName, "Assets");

						TexturePtr regionTexture = TextureManager::getSingleton().createManual(lightmapName, "Assets", TextureType::TEX_TYPE_2D, regionLightmapSize, regionLightmapSize, -1, PixelFormat::PF_R8G8B8);
						Image img;
						regionTexture->convertToImage(img);

						auto qGeomList = git->getQueuedGeometryList();

						int texSize = regionLightmapSize * regionLightmapSize;
						int fragSize = sqrt((float)texSize / (float)qGeomList.size()) * 0.8f;

						for (auto it = qGeomList.begin(); it != qGeomList.end(); ++it)
						{
							StaticGeometry::QueuedGeometry* qgeom = *it;
							TexturePtr t = qgeom->lightmapTexture;
							if (t == nullptr)
								continue;

							Image img1;
							t->convertToImage(img1);
							img1.resize(fragSize, fragSize);

							for (int i = 0; i < img1.getWidth(); ++i)
							{
								for (int j = 0; j < img1.getHeight(); ++j)
								{
									int x = (int)qgeom->lightmapOffsetX + i;
									int y = (int)qgeom->lightmapOffsetY + j;

									if (x < img.getWidth() && y < img.getHeight())
									{
										img.setColourAt(img1.getColourAt(i, j, 0), x, y, 0);
									}
								}
							}
						}

						regionTexture->loadImage(img);
						img.save(lightmapsPath + regionTexture->getName() + ".png");

						git->setLightmapTexture(regionTexture);

						++_g;
					}

					++_m;
				}
			}
		}
	}
	else
	{
		auto reg = sg->getRegionIterator();

		while (reg.hasMoreElements())
		{
			StaticGeometry::Region* region = reg.getNext();

			auto lit = region->getLODIterator();
			while (lit.hasMoreElements())
			{
				StaticGeometry::LODBucket* lod = lit.getNext();

				auto mtx = lod->getMaterialIterator();
				int _m = 0;
				while (mtx.hasMoreElements())
				{
					auto mtb = mtx.getNext();

					auto qGeom = mtb->getGeometryIterator();
					int _g = 0;
					while (qGeom.hasMoreElements())
					{
						auto git = qGeom.getNext();

						string dir = IO::RemovePart(lightmapsPath, GetEngine->GetAssetsPath());
						string lightmapName = dir + "RegionLightmap_" + to_string(region->getID()) + "_" + to_string(_m) + "_" + to_string(_g) + ".png";

						if (GetEngine->GetUseUnpackedResources())
						{
							if (IO::FileExists(GetEngine->GetAssetsPath() + lightmapName))
							{
								TexturePtr regionTexture = TextureManager::getSingleton().load(lightmapName, "Assets");
								git->setLightmapTexture(regionTexture);
							}
						}
						else
						{
							if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), lightmapName))
							{
								TexturePtr ltex;

								if (!TextureManager::getSingleton().resourceExists(lightmapName, "Assets"))
								{
									int sz = 0;
									char* buffer = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), lightmapName, sz);
									MemoryDataStream* memStream = new Ogre::MemoryDataStream(reinterpret_cast<void*>(buffer), sz);
									DataStreamPtr data_stream(memStream);

									String ext = IO::GetFileExtension(lightmapName);
									Image image;
									image.load(data_stream, ext);

									data_stream->close();
									delete[] buffer;
									memStream->close();

									ltex = TextureManager::getSingleton().create(lightmapName, "Assets", true);
									ltex->loadImage(image);
								}
								else
								{
									ltex = TextureManager::getSingleton().getByName(lightmapName, "Assets");
								}

								git->setLightmapTexture(ltex);
							}
						}

						++_g;
					}

					++_m;
				}
			}
		}
	}
}

void Engine::loadProjectSettings()
{
	if (IO::FileExists(GetEngine->GetRootPath() + "Settings/Main.settings"))
		projectSettings->Load(GetEngine->GetRootPath() + "Settings/Main.settings");
}

void Engine::saveProjectSettings()
{
	projectSettings->Save(GetEngine->GetRootPath() + "Settings/Main.settings");
}

void Engine::DestroyAttachedObjects(SceneNode * node)
{
	// Destroy all the attached objects
	SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

	while (itObject.hasMoreElements())
	{
		MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());
		if (pObject->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			instance.ClearMaterialInstance((Entity*)pObject);

		instance.mSceneManager->destroyMovableObject(pObject);
	}
}

void Engine::ClearScene(SceneManager * manager)
{
	GetEngine->ClearMaterialsInstances();
	GetEngine->getStaticGeometry()->reset();

	ColourValue ambient = ColourValue(0.15, 0.15, 0.15, 1.0);
	GetEngine->GetSceneManager()->setAmbientLight(ambient);

	instance.mSceneManager->setSkyBox(false, "");

	NavigationManager* navMgr = GetEngine->GetNavigationManager();

	navMgr->SetWalkableSlopeAngle(45);
	navMgr->SetWalkableRadius(1.6);
	navMgr->SetWalkableHeight(2.0);
	navMgr->SetWalkableClimb(3.9);
	navMgr->SetMinRegionArea(4.0);
	navMgr->SetMergeRegionArea(20.0);
	navMgr->SetMaxSimplificationError(3.0);
	navMgr->SetMaxEdgeLen(12);
	navMgr->SetCellSize(1.00);
	navMgr->SetCellHeight(0.20);

	GetEngine->SetDefaultLightmapSize(256);
	GetEngine->SetRegionLightmapSize(1024);
	GetEngine->SetLightmapBlurRadius(0.5f);

	SceneNode * root = manager->getRootSceneNode();

	GetEngine->nameGenerator->reset();
	GetEngine->GetUIManager()->Cleanup();
	GetEngine->GetMonoRuntime()->CleanUp();
	instance.GetTerrainManager()->CleanUp();

	Engine::Destroy(root);

	MapIterator __it = FBXSceneManager::getSingleton().getResourceIterator();
	while (__it.hasMoreElements())
	{
		FBXScene* res = (FBXScene*)__it.getNext().getPointer();

		res->RemoveMeshesFromCache();
	}

	MapIterator meshes = MeshManager::getSingleton().getResourceIterator();
	std::vector<MeshPtr> _meshesRemove;
	while (meshes.hasMoreElements())
	{
		MeshPtr _mesh = static_pointer_cast<Mesh>(meshes.getNext());
		if (GetEngine->IsEditorMesh(_mesh))
			continue;

		if (_mesh->getName() != "")
			_meshesRemove.push_back(_mesh);
	}

	for (auto it = _meshesRemove.begin(); it != _meshesRemove.end(); ++it)
	{
		MeshManager::getSingleton().unload((*it)->getHandle());
		MeshManager::getSingleton().remove(*it);
	}

	_meshesRemove.clear();

	GetEngine->GetPhysicsManager()->Reset();

	manager->destroyAllAnimations();
	manager->destroyAllAnimationStates();
	SkeletonManager::getSingleton().removeAll();

	instance.navigationManager->ClearNavMesh();

	instance.loadedScene = "";

	instance.SetMainCamera(nullptr);
}

void Engine::LoadResources(bool useUnpackedResources)
{
	//Init resources
	//Add resources locations
	ResourceGroupManager::getSingleton().createResourceGroup("Assets");

	if (useUnpackedResources)
	{
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(builtinResourcesPath, "FileSystem", "Assets", true, false);
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(assetsPath, "FileSystem", "Assets", true, false);
	}
	else
	{
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(builtinResourcesPath.substr(0, builtinResourcesPath.length() - 1) + ".resources", "Zip", "Assets", true);
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(assetsPath.substr(0, assetsPath.length() - 1) + ".resources", "Zip", "Assets", true);
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(cachePath, "FileSystem", "Assets", true, false);
	}

	fbxSceneManager = new FBXSceneManager(mSceneManager);

	LoadFBXFiles();

	if (ResourcesAddedCallback != NULL)
		ResourcesAddedCallback();

	//Cache files needs to be created before loading resources
	GetEngine->CreateCache();

	//Init resources
	ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

	//Create new materials and link them with shaders
	GetEngine->ResolveDependencies();

	if (GetEngine->GetEnvironment() == Engine::Environment::Editor)
		ResourceMap::removeLostResources();

	ResourceMap::identifyResources();

	Ogre::ImguiManager::createSingleton();
	Ogre::ImguiManager::getSingleton().init(mSceneManager);

	terrainManager = new TerrainManager();
	terrainManager->InitTerrainGroup();

	deferredSystem = new DeferredShadingSystem(mSceneManager);
	deferredSystem->createResources();
	deferredSystem->initialize();

	Ogre::StringStream v_src;
	v_src << "uniform mat4 cWorldViewProj;\n";
	v_src << "attribute vec4 vertex;\n";
	v_src << "attribute vec2 uv0;\n";
	v_src << "varying vec2 vUv0;\n";
	v_src << "void main(){\n";
	v_src << "gl_Position = cWorldViewProj * vertex;\n";
	v_src << "vUv0 = uv0;\n";
	v_src << "}";

	Ogre::StringStream f_src;
	f_src << "uniform vec4 color;\n";
	f_src << "uniform sampler2D tex;\n";
	f_src << "varying vec2 vUv0;\n";
	f_src << "void main(){\n";
	f_src << "gl_FragColor = texture2D(tex, vUv0) * color;\n";
	f_src << "}";

	HighLevelGpuProgramPtr ptrProgramV = HighLevelGpuProgramManager::getSingleton().createProgram("UICanvas_vertex", "Assets", "glsl", GPT_VERTEX_PROGRAM);
	ptrProgramV->setParameter("profiles", "glsl120");
	ptrProgramV->setSource(v_src.str());
	ptrProgramV->getDefaultParameters()->setNamedAutoConstant("cWorldViewProj", GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

	HighLevelGpuProgramPtr ptrProgramF = HighLevelGpuProgramManager::getSingleton().createProgram("UICanvas_fragment", "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
	ptrProgramF->setParameter("profiles", "glsl120");
	ptrProgramF->setSource(f_src.str());
	ptrProgramF->getDefaultParameters()->setNamedConstant("color", Ogre::ColourValue(1, 1, 1, 1));

	UICanvas::guiShaderV = ptrProgramV;
	UICanvas::guiShaderF = ptrProgramF;

	MaterialPtr mMat = MaterialManager::getSingleton().create("BaseWhite", "Assets");
	mMat->removeAllTechniques();

	Ogre::Technique* mTech = mMat->createTechnique();
	mTech->setSchemeName("Deferred");
	Pass* perlightPass = mTech->createPass();

	perlightPass->setVertexProgram("Shaders/DeferredPBR.glslv", "Assets");
	perlightPass->setFragmentProgram("Shaders/DeferredPBR.glslf", "Assets");
	perlightPass->setLightingEnabled(false);

	mMat->SetProgramSharedParameter("cDiffuseColor", Material::ParameterType::PT_VECTOR3, Material::ValueVariant(Vector3(1, 1, 1)));
	mMat->SetProgramSharedParameter("cSpecularity", Material::ParameterType::PT_FLOAT, Material::ValueVariant(1.0f));
	mMat->SetProgramSharedParameter("cOcclusionFactor", Material::ParameterType::PT_FLOAT, Material::ValueVariant(1.0f));
	mMat->SetProgramSharedParameter("cIBLFactor", Material::ParameterType::PT_FLOAT, Material::ValueVariant(1.0f));
	mMat->SetProgramSharedParameter("cEmissionFactor", Material::ParameterType::PT_FLOAT, Material::ValueVariant(0.0f));
	mMat->SetProgramSharedParameter("cMetalness", Material::ParameterType::PT_FLOAT, Material::ValueVariant(0.5f));
	mMat->SetProgramSharedParameter("cReflectance", Material::ParameterType::PT_FLOAT, Material::ValueVariant(0.25f));
}

void Engine::LoadFBXFiles()
{
	if (GetEngine->GetUseUnpackedResources())
	{
		IO::listFiles(assetsPath, true, nullptr, [=](std::string d, std::string f) {
			if (IO::GetFileExtension(f) == "fbx")
			{
				string path = d + f;
				path = boost::replace_all_copy(path, assetsPath, "");

				ResourceGroupManager::getSingleton().declareResource(path, "FBXScene", "Assets", { make_pair("Path", d + f) });
			}
			});

		std::string builtinPath = GetEngine->GetBuiltinResourcesPath();

		IO::listFiles(builtinPath, true, nullptr, [=](std::string d, std::string f) {
			if (IO::GetFileExtension(f) == "fbx")
			{
				string path = d + f;
				path = boost::replace_all_copy(path, builtinPath, "");

				ResourceGroupManager::getSingleton().declareResource(path, "FBXScene", "Assets", { make_pair("Path", d + f) });
			}
			});
	}
	else
	{
		std::vector<string> zipFiles = ZipHelper::getAllFilesNamesInZip(zipArch1);

		for (auto it = zipFiles.begin(); it != zipFiles.end(); ++it)
		{
			ResourceGroupManager::getSingleton().declareResource(*it, "FBXScene", "Assets", { make_pair("Path", *it) });
		}

		zipFiles = ZipHelper::getAllFilesNamesInZip(zipArch2);

		for (auto it = zipFiles.begin(); it != zipFiles.end(); ++it)
		{
			ResourceGroupManager::getSingleton().declareResource(*it, "FBXScene", "Assets", { make_pair("Path", *it) });
		}

		zipFiles.clear();
	}
}

void Engine::SetMainCamera(Camera * camera)
{
	if (environment == Environment::Player)
	{
		if (GetEngine->GetRenderWindow() != nullptr)
		{
			if (!GetEngine->GetRenderWindow()->hasViewportWithZOrder(0))
			{
				if (camera != nullptr)
				{
					camera->setAutoAspectRatio(true);
					Viewport* mViewport = GetEngine->GetRenderWindow()->addViewport(camera);
					ColourValue cColor = ColourValue(0.5f, 0.5f, 0.5f);
					mViewport->setBackgroundColour(cColor);
				}
			}
			else
			{
				Viewport* mViewport = GetEngine->GetRenderWindow()->getViewportByZOrder(0);
				if (camera != nullptr && camera->getVisible())
				{
					mViewport->setCamera(camera);
				}
				else
				{
					mViewport->setCamera(nullptr);
					mViewport->clear();
				}
			}
		}

		mainCamera = camera;
		SceneManager::mainCamera = mainCamera;

		GetEngine->GetTerrainManager()->SetPagedGeometryCamera(mainCamera);
	}
}

void Engine::GetSceneNodeCount(SceneNode * root, int &start)
{
	Node::ChildNodeIterator itr = root->getChildIterator();

	while (itr.hasMoreElements())
	{
		++start;
		Node * child = itr.getNext();
		GetSceneNodeCount((SceneNode*)child, start);
	}
}

void Engine::CreateCache()
{
	string cache_d = string(assetsPath + "Cache/");
	string cache_shader_d = string(assetsPath + "Cache/Shaders/");

	std::wstring cache_dir = ::StringConvert::s2ws(cache_d, GetACP());
	std::wstring cache_shader_dir = ::StringConvert::s2ws(cache_shader_d, GetACP());

	CreateDirectory(cache_dir.c_str(), NULL);
	CreateDirectory(cache_shader_dir.c_str(), NULL);

	//MaterialSerializer serializer;

	Ogre::FileInfoListPtr fileinfoiter = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo("Assets");

	for (unsigned int i = 0; i < (*fileinfoiter).size(); i++)
	{
		string ext = IO::GetFileExtension((*fileinfoiter)[i].filename);
		string name = (*fileinfoiter)[i].basename.substr(0, (*fileinfoiter)[i].basename.find_last_of('.'));

		//Check shaders definitions
		if (ext._Equal("glslv") || ext._Equal("glslf"))
			CreateShaders((*fileinfoiter)[i].filename);
	}

	HighLevelGpuProgramManager::getSingletonPtr()->removeAll();
}

void Engine::CreateShaders(std::string fileName)
{
	string ext = IO::GetFileExtension(fileName);
	string name = IO::GetFileName(fileName);

	std::string nameRepl = boost::replace_all_copy(fileName, "/", "_");
	std::string cachePath = assetsPath + "Cache/Shaders/" + md5(nameRepl) + ".program";

	//Check shaders definitions
	if (ext._Equal("glslv"))
	{
		if (!IO::FileExists(cachePath))
		{
			string content = "vertex_program " + fileName + " glsl\n{\n";
			content += "	source " + fileName + "\n}\n";
			IO::WriteText(cachePath, content);
		}
	}

	if (ext._Equal("glslf"))
	{
		if (!IO::FileExists(cachePath))
		{
			string content = "fragment_program " + fileName + " glsl\n{\n";
			content += "	source " + fileName + "\n}\n";
			IO::WriteText(cachePath, content);
		}
	}
}

void Engine::ResolveDependencies()
{
	if (useUnpackedResources)
	{
		//Search and load fonts
		IO::listFiles(assetsPath, true, [](std::string dir) {}, [=](std::string dir, std::string file) {
			std::string ext = IO::GetFileExtension(file);
			std::string asset = boost::replace_all_copy(dir + file, assetsPath, "");

			if (ext._Equal("ttf") || ext._Equal("otf"))
			{
				Ogre::FontPtr mFont = Ogre::FontManager::getSingleton().create(asset, "Assets");
				mFont->_notifyOrigin(assetsPath + asset);
				mFont->setType(Ogre::FT_TRUETYPE);
				mFont->setSource(asset);
				mFont->setParameter("size", "18");
				mFont->setParameter("resolution", "192");
				mFont->load();
			}
		});
	}
	else
	{
		std::vector<string> zipFiles = ZipHelper::getAllFilesNamesInZip(zipArch1);

		for (auto it = zipFiles.begin(); it != zipFiles.end(); ++it)
		{
			std::string ext = IO::GetFileExtension(*it);
			std::string asset = *it;

			if (ext._Equal("ttf") || ext._Equal("otf"))
			{
				Ogre::FontPtr mFont = Ogre::FontManager::getSingleton().create(asset, "Assets");
				mFont->_notifyOrigin(asset);
				mFont->setType(Ogre::FT_TRUETYPE);
				mFont->setSource(asset);
				mFont->setParameter("size", "18");
				mFont->setParameter("resolution", "192");
				mFont->load();
			}
		}
	}
}

void Engine::AnimationPlay(SceneNode* node, std::string animationName, bool play, float duration, unsigned int playMode, unsigned int prevStatePosition, unsigned int currentStatePosition)
{
	AnimationList* animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);
	AnimationPlay(animList, animationName, play, duration, playMode, prevStatePosition, currentStatePosition);
}

void Engine::AnimationPlay(AnimationList* animList, std::string animationName, bool play, float duration, unsigned int playMode, unsigned int prevStatePosition, unsigned int currentStatePosition)
{
	SceneNode* node = animList->GetParentSceneNode();

	if (animList == nullptr)
		return;

	if (!play)
	{
		for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
		{
			Engine::AnimationPlayInternal(node, animList, it->name, false);
		}

		animList->SetTransitionDuration(0);
		animList->SetTransitionTimeLeft(0);
		animList->SetPrevAnimationName("");
		animList->SetCurrentAnimationName("");
	}
	else
	{
		if (!AnimationIsPlaying(node, animationName))
	{
			if (duration > 0)
			{
				for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
				{
					if (it->name != animationName && it->name != animList->GetCurrentAnimationName() && it->name != animList->GetPrevAnimationName())
						Engine::AnimationPlayInternal(node, animList, it->name, false);
				}

				if (animationName != animList->GetCurrentAnimationName())
				{
					animList->SetPlayMode(static_cast<AnimationList::AnimationPlayMode>(playMode));
					animList->SetPrevStatePositionMode(static_cast<AnimationList::AnimationPositionMode>(prevStatePosition));
					animList->SetCurrentStatePositionMode(static_cast<AnimationList::AnimationPositionMode>(currentStatePosition));

					if (animList->GetCurrentAnimationName().empty())
					{
						animList->SetCurrentAnimationName(animationName);
					}
					else
					{
						if (!animList->GetPrevAnimationName().empty())
							AnimationPlayInternal(node, animList, animList->GetPrevAnimationName(), false);

						animList->SetPrevAnimationName(animList->GetCurrentAnimationName());
						animList->SetCurrentAnimationName(animationName);

						animList->SetTransitionDuration(duration);
						animList->SetTransitionTimeLeft(duration);

						if (prevStatePosition == AnimationList::AnimationPositionMode::APS_Start)
							SetAnimationTimePosition(node, animList->GetPrevAnimationName(), AnimationTimePosition::ATP_Start);
						else if (prevStatePosition == AnimationList::AnimationPositionMode::APS_End)
							SetAnimationTimePosition(node, animList->GetPrevAnimationName(), AnimationTimePosition::ATP_End);
					}
				}
			}
			else
			{
				for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
				{
					if (it->name != animationName)
						Engine::AnimationPlayInternal(node, animList, it->name, false);
				}

				animList->SetCurrentAnimationName(animationName);

				if (!animList->GetPrevAnimationName().empty())
				{
					AnimationPlayInternal(node, animList, animList->GetPrevAnimationName(), false);
					animList->SetPrevAnimationName("");
				}
			}

			AnimationPlayInternal(node, animList, animationName, play);

			if (duration > 0)
			{
				if (currentStatePosition == AnimationList::AnimationPositionMode::APS_Start)
					SetAnimationTimePosition(node, animationName, AnimationTimePosition::ATP_Start);
				else if (currentStatePosition == AnimationList::AnimationPositionMode::APS_End)
					SetAnimationTimePosition(node, animationName, AnimationTimePosition::ATP_End);
			}
		}
	}
}

void Engine::AnimationPlayInternal(SceneNode * node, AnimationList * animList, std::string animationName, bool play)
{
	Animation * curveAnim = nullptr;

	std::string sourceFileName = animList->GetAnimationData(animationName).fileName;// animList->GetFBXFileName();
	std::string fullName = sourceFileName + "_" + node->getName() + "_anim_" + animationName;

	if (!instance.mSceneManager->hasAnimation(fullName))
	{
		if (play)
		{
			SceneSerializer serializer;
			serializer.RestoreAnimations(animList, animationName);
		}
	}

	if (instance.mSceneManager->hasAnimation(fullName))
		curveAnim = instance.mSceneManager->getAnimation(fullName);

	animList->eventsCalled[animationName] = false;

	if (curveAnim != nullptr)
	{
		AnimationState * animState = instance.mSceneManager->getAnimationState(curveAnim->getName());
		animState->setEnabled(play);
		animState->setLoop(animList->GetAnimationData(animationName).loop);
		
		if (play)
		{
			if (!animState->getLoop())
				animState->setTimePosition(0);
		}
		else
		{
			animState->setTimePosition(0);
		}
	}

	SkeletalAnimationPlay(node, animationName, fullName, animList, play);
}

void Engine::SkeletalAnimationPlay(SceneNode* root, std::string animationName, std::string animfullName, AnimationList* animList, bool play)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* ent = (Entity*)curNode->getAttachedObject(0);

				if (ent->hasSkeleton())
				{
					SkeletonInstance* skeleton = ent->getSkeleton();

					if (!skeleton->hasAnimation(animfullName))
					{
						if (play)
						{
							SceneSerializer serializer;
							serializer.RestoreAnimations(animList, animationName);

							ent = (Entity*)curNode->getAttachedObject(0);
							skeleton = ent->getSkeleton();
						}
					}

					if (skeleton->hasAnimation(animfullName))
					{
						Animation* skelAnim = skeleton->getAnimation(animfullName);

						if (ent->hasAnimationState(skelAnim->getName()))
						{
							AnimationState* animState = ent->getAnimationState(skelAnim->getName());
							//skeleton->setBlendMode(SkeletonAnimationBlendMode::ANIMBLEND_CUMULATIVE);

							animState->setEnabled(play);
							animState->setLoop(animList->GetAnimationData(animationName).loop);

							if (play)
							{
								if (!animState->getLoop())
									animState->setTimePosition(0);
							}
							else
							{
								animState->setTimePosition(0);
							}
						}
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
			nstack.insert(nstack.begin() + j, nd);
		}
	}
}

void Engine::AnimationSetLoop(SceneNode * node, std::string animationName, bool loop)
{
	AnimationList * animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);

	if (animList == nullptr)
		return;

	Animation * curveAnim = nullptr;

	std::string sourceFileName = animList->GetAnimationData(animationName).fileName;// animList->GetFBXFileName();
	std::string fullName = sourceFileName + "_" + node->getName() + "_anim_" + animationName;

	if (!instance.mSceneManager->hasAnimation(fullName))
	{
		SceneSerializer serializer;
		serializer.RestoreAnimations(animList, animationName);
	}

	if (instance.mSceneManager->hasAnimation(fullName))
		curveAnim = instance.mSceneManager->getAnimation(fullName);

	if (curveAnim != nullptr)
	{
		AnimationState * animState = instance.mSceneManager->getAnimationState(curveAnim->getName());
		animState->setLoop(loop);
		animList->GetAnimationData(animationName).loop = loop;
	}

	SkeletalAnimationSetLoop(node, animationName, fullName, animList, loop);
}

void Engine::SkeletalAnimationSetLoop(SceneNode* root, std::string animationName, std::string animfullName, AnimationList* animList, bool loop)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* ent = (Entity*)curNode->getAttachedObject(0);

				if (ent->hasSkeleton())
				{
					SkeletonInstance* skeleton = ent->getSkeleton();

					if (!skeleton->hasAnimation(animfullName))
					{
						SceneSerializer serializer;
						serializer.RestoreAnimations(animList, animationName);

						ent = (Entity*)curNode->getAttachedObject(0);
						skeleton = ent->getSkeleton();
					}

					if (skeleton->hasAnimation(animfullName))
					{
						Animation* skelAnim = skeleton->getAnimation(animfullName);

						if (ent->hasAnimationState(skelAnim->getName()))
						{
							AnimationState* animState = ent->getAnimationState(skelAnim->getName());
							animState->setLoop(loop);
							animList->GetAnimationData(animationName).loop = loop;
						}
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
			nstack.insert(nstack.begin() + j, nd);
		}
	}
}

bool Engine::AnimationIsPlaying(SceneNode * node, std::string animationName)
{
	AnimationList * animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);

	if (animList == nullptr)
		return false;

	if (animList->GetTransitionDuration() > 0 && animList->GetTransitionTimeLeft() > 0/* && (animList->GetCurrentAnimationName() == animationName || animList->GetPrevAnimationName() == animationName)*/)
		return true;

	Animation * curveAnim = nullptr;

	std::string sourceFileName = animList->GetAnimationData(animationName).fileName;// animList->GetFBXFileName();
	std::string fullName = sourceFileName + "_" + node->getName() + "_anim_" + animationName;

	if (!instance.mSceneManager->hasAnimation(fullName))
	{
		//SceneSerializer serializer;
		//serializer.RestoreAnimations(animList, animationName);
	}

	if (instance.mSceneManager->hasAnimation(fullName))
		curveAnim = instance.mSceneManager->getAnimation(fullName);

	if (curveAnim != nullptr)
	{
		bool play = false;

		AnimationState * animState = instance.mSceneManager->getAnimationState(curveAnim->getName());
		if (animState->getLoop())
		{
			if (animState->getEnabled())
				play = true;
		}
		else
		{
			play = animState->getEnabled() && animState->getTimePosition() < animState->getLength();
		}

		if (play)
			return true;
	}

	bool skeletal = false;
	SkeletalAnimationIsPlaying(node, animationName, fullName, animList, skeletal);

	return skeletal;
}

void Engine::SkeletalAnimationIsPlaying(SceneNode* root, std::string animationName, std::string animfullName, AnimationList* animList, bool& play)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(root);

	play = false;

	while (nstack.size() > 0 && !play)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////

		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* ent = (Entity*)curNode->getAttachedObject(0);

				if (ent->hasSkeleton())
				{
					SkeletonInstance* skeleton = ent->getSkeleton();

					if (!skeleton->hasAnimation(animfullName))
					{
						/*SceneSerializer serializer;
						serializer.RestoreAnimations(animList, animationName);

						ent = (Entity*)curNode->getAttachedObject(0);
						skeleton = ent->getSkeleton();*/
						play = false;
						break;
					}

					if (skeleton->hasAnimation(animfullName))
					{
						Animation* skelAnim = skeleton->getAnimation(animfullName);

						if (ent->hasAnimationState(skelAnim->getName()))
						{
							AnimationState* animState = ent->getAnimationState(skelAnim->getName());
							if (animState->getLoop())
							{
								if (animState->getEnabled())
								{
									play = true;
									break;
								}
							}
							else
							{
								play = animState->getEnabled() && animState->getTimePosition() < animState->getLength();
								if (play)
									break;
							}
						}
					}
				}
			}
		}

		if (!play)
		{
			////////////////////

			int j = 0;
			auto children = curNode->getChildren();
			for (auto it = children.begin(); it != children.end(); ++it, ++j)
			{
				SceneNode* nd = (SceneNode*)(*it);
				nstack.insert(nstack.begin() + j, nd);
			}
		}
	}

	nstack.clear();
}

bool Engine::AnimationIsLoop(SceneNode * node, std::string animationName)
{
	AnimationList * animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);

	if (animList == nullptr)
		return false;

	if (animList->IsAnimationDataExists(animationName))
		return animList->GetAnimationData(animationName).loop;
	else
		return false;

	return false;
}

void Engine::AnimationSetSpeed(SceneNode * node, std::string animationName, float speed)
{
	AnimationList * animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);

	if (animList == nullptr)
		return;

	if (animList->IsAnimationDataExists(animationName))
		animList->GetAnimationData(animationName).speed = speed;
}

float Engine::AnimationGetSpeed(SceneNode * node, std::string animationName)
{
	AnimationList * animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);

	if (animList == nullptr)
		return 0.0;

	if (animList->IsAnimationDataExists(animationName))
		return animList->GetAnimationData(animationName).speed;
	else
		return 0.0;
}

void Engine::SetAnimationTimePosition(SceneNode* node, std::string animationName, AnimationTimePosition position)
{
	AnimationList* animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);

	if (animList == nullptr)
		return;

	Animation* curveAnim = nullptr;

	std::string sourceFileName = animList->GetAnimationData(animationName).fileName;// animList->GetFBXFileName();
	std::string fullName = sourceFileName + "_" + node->getName() + "_anim_" + animationName;

	if (!instance.mSceneManager->hasAnimation(fullName))
	{
		SceneSerializer serializer;
		serializer.RestoreAnimations(animList, animationName);
	}

	if (instance.mSceneManager->hasAnimation(fullName))
		curveAnim = instance.mSceneManager->getAnimation(fullName);

	if (curveAnim != nullptr)
	{
		AnimationState* animState = instance.mSceneManager->getAnimationState(curveAnim->getName());
		if (position == AnimationTimePosition::ATP_Start)
			animState->setTimePosition(0);
		else
			animState->setTimePosition(animState->getLength());
	}

	SetSkeletalAnimationTimePosition(node, animList, animationName, fullName, position);
}

void Engine::SetSkeletalAnimationTimePosition(SceneNode* root, AnimationList* animList, std::string animationName, std::string animfullName, AnimationTimePosition position)
{
	std::vector<SceneNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		if (curNode->getAttachedObjects().size() > 0)
		{
			if (curNode->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
			{
				Entity* ent = (Entity*)curNode->getAttachedObject(0);

				if (ent->hasSkeleton())
				{
					SkeletonInstance* skeleton = ent->getSkeleton();

					if (!skeleton->hasAnimation(animfullName))
					{
						SceneSerializer serializer;
						serializer.RestoreAnimations(animList, animationName);

						ent = (Entity*)curNode->getAttachedObject(0);
						skeleton = ent->getSkeleton();
					}

					if (skeleton->hasAnimation(animfullName))
					{
						Animation* skelAnim = skeleton->getAnimation(animfullName);

						if (ent->hasAnimationState(skelAnim->getName()))
						{
							AnimationState* animState = ent->getAnimationState(skelAnim->getName());
							if (position == AnimationTimePosition::ATP_Start)
								animState->setTimePosition(0);
							else
								animState->setTimePosition(animState->getLength());
						}
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
			nstack.insert(nstack.begin() + j, nd);
		}
	}
}

void Engine::AnimationInternalUpdateRecursive(SceneNode* root, float deltaTime)
{
	std::vector<SceneNode*> nstack;

	auto children = root->getChildren();
	for (auto it = children.begin(); it != children.end(); ++it)
		nstack.push_back((SceneNode*)*it);

	while (nstack.size() > 0)
	{
		SceneNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		////////////////////
		if (!curNode->getVisible())
			continue;

		AnimationList* animList = (AnimationList*)curNode->GetComponent(AnimationList::COMPONENT_TYPE);

		if (animList != nullptr)
		{
			if (animList->GetPrevAnimationName().empty())
			{
				animList->SetTransitionTimeLeft(0.0);
			}
			else
			{
				animList->SetTransitionTimeLeft(animList->GetTransitionTimeLeft() - (deltaTime * 15.0f));
				if (animList->GetTransitionTimeLeft() <= 0)
					animList->SetTransitionDuration(0);
			}

			AnimationState* _animState = nullptr;
			AnimationState* _animStateEnt = nullptr;

			AnimationList::AnimationData ait;
			if (!animList->GetCurrentAnimationName().empty())
				ait = animList->GetAnimationData(animList->GetCurrentAnimationName());

			AnimationList::AnimationData pait;
			if (!animList->GetPrevAnimationName().empty())
				pait = animList->GetAnimationData(animList->GetPrevAnimationName());

			std::string fullName = ait.fileName + "_" + curNode->getName() + "_anim_" + ait.name;
			std::string pfullName = pait.fileName + "_" + curNode->getName() + "_anim_" + pait.name;

			if (instance.mSceneManager->hasAnimationState(fullName))
			{
				AnimationState* animState = instance.mSceneManager->getAnimationState(fullName);
				AnimationState* prevAnimState = nullptr;
				if (!animList->GetPrevAnimationName().empty())
				{
					if (instance.mSceneManager->hasAnimationState(pfullName))
						prevAnimState = instance.mSceneManager->getAnimationState(pfullName);
				}

				if (animState->getEnabled())
				{
					if (animList->GetTransitionTimeLeft() > 0 && prevAnimState != nullptr)
					{
						prevAnimState->setEnabled(true);

						float weight1 = animList->GetTransitionTimeLeft() / animList->GetTransitionDuration();
						float weight2 = 1.0 - animList->GetTransitionTimeLeft() / animList->GetTransitionDuration();

						prevAnimState->setWeight(weight1);
						animState->setWeight(weight2);

						if (animList->GetPlayMode() == AnimationList::AnimationPlayMode::APL_Both)
						{
							prevAnimState->addTime((deltaTime * 15.0f) * pait.speed);
							animState->addTime((deltaTime * 15.0f) * ait.speed);
						}

						if (animList->GetPlayMode() == AnimationList::AnimationPlayMode::APL_Current)
							animState->addTime((deltaTime * 15.0f) * ait.speed);

						if (animList->GetPlayMode() == AnimationList::AnimationPlayMode::APL_Prev)
							prevAnimState->addTime((deltaTime * 15.0f) * pait.speed);
					}
					else
					{
						if (prevAnimState != nullptr)
						{
							if (prevAnimState->getEnabled())
							{
								prevAnimState->setWeight(0);
								prevAnimState->setEnabled(false);
							}
						}

						animState->setWeight(1);
						animState->addTime((deltaTime * 15.0f) * ait.speed);
					}

					_animState = animState;
				}
			}

			MapIterator entities = instance.mSceneManager->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

			while (entities.hasMoreElements())
			{
				Entity* ent = (Entity*)entities.getNext();

				if (ent->hasAnimationState(fullName))
				{
					AnimationState* animStateEnt = ent->getAnimationState(fullName);
					AnimationState* panimStateEnt = nullptr;
					if (!animList->GetPrevAnimationName().empty())
					{
						if (ent->hasAnimationState(pfullName))
							panimStateEnt = ent->getAnimationState(pfullName);
					}

					if (animStateEnt->getEnabled())
					{
						if (animList->GetTransitionTimeLeft() > 0 && panimStateEnt != nullptr)
						{
							panimStateEnt->setEnabled(true);

							float weight1 = animList->GetTransitionTimeLeft() / animList->GetTransitionDuration();
							float weight2 = 1.0 - animList->GetTransitionTimeLeft() / animList->GetTransitionDuration();

							panimStateEnt->setWeight(weight1);
							animStateEnt->setWeight(weight2);

							if (animList->GetPlayMode() == AnimationList::AnimationPlayMode::APL_Both)
							{
								panimStateEnt->addTime((deltaTime * 15.0f) * pait.speed);
								animStateEnt->addTime((deltaTime * 15.0f) * ait.speed);
							}

							if (animList->GetPlayMode() == AnimationList::AnimationPlayMode::APL_Current)
								animStateEnt->addTime((deltaTime * 15.0f) * ait.speed);

							if (animList->GetPlayMode() == AnimationList::AnimationPlayMode::APL_Prev)
								panimStateEnt->addTime((deltaTime * 15.0f) * pait.speed);
						}
						else
						{
							if (panimStateEnt != nullptr)
							{
								if (panimStateEnt->getEnabled())
								{
									panimStateEnt->setWeight(0);
									panimStateEnt->setEnabled(false);
								}
							}

							animStateEnt->setWeight(1);
							animStateEnt->addTime((deltaTime * 15.0f) * ait.speed);
						}

						_animStateEnt = animStateEnt;
					}
				}
			}

			//------End playback event------//
			AnimationState* _animStateEvent = nullptr;
			if (_animState != nullptr && _animStateEnt != nullptr)
			{
				if (_animState->getLength() > _animStateEnt->getLength())
					_animStateEvent = _animState;
				else
					_animStateEvent = _animStateEnt;
			}

			if (_animState == nullptr) _animStateEvent = _animStateEnt;
			if (_animStateEnt == nullptr) _animStateEvent = _animState;

			if (_animStateEvent != nullptr)
			{
				if (_animStateEvent->getTimePosition() == _animStateEvent->getLength())
				{
					bool call = false;
					if (_animStateEvent->getLoop())
						call = true;
					else
					{
						if (animList->eventsCalled.find(ait.name) == animList->eventsCalled.end() || animList->eventsCalled[ait.name] == false)
						{
							animList->eventsCalled[ait.name] = true;
							call = true;
						}
					}

					if (call)
					{
						MonoObject* mObj = animList->GetMonoObject();
						MonoString* mAnimName = mono_string_new(instance.monoRuntime->GetDomain(), ait.name.c_str());
						void* args[2] = { mObj, mAnimName };
						instance.monoRuntime->Execute(mObj, "CallOnEnded", args, "Animation,string");
					}
				}
			}
			//------End playback event------//
		}

		if (instance.environment == Environment::Player)
		{
			AnimationList* lst = (AnimationList*)curNode->GetComponent(AnimationList::COMPONENT_TYPE);
			SceneNode* boneRoot = curNode;

			if (lst == nullptr)
			{
				SkeletonBone* bone = (SkeletonBone*)curNode->GetComponent(SkeletonBone::COMPONENT_TYPE);

				if (bone != nullptr)
				{
					boneRoot = instance.mSceneManager->getSceneNodeFast(bone->GetRootNodeName());

					if (boneRoot != nullptr)
					{
						lst = (AnimationList*)boneRoot->GetComponent(AnimationList::COMPONENT_TYPE);
					}
				}
			}

			if (boneRoot != nullptr)
			{
				if (lst != nullptr && lst->GetEnabled())
				{
					if (boneRoot->animationEnabled == -1 || boneRoot->animationEnabled == 1)
					{
						boneRoot->animationEnabled = 0;
						SkinnedMeshDeformer::EnableAnimation(curNode, false, false);
					}

					SkinnedMeshDeformer::UpdateSkeleton(curNode, false, nullptr, false);
				}
				else
				{
					if (curNode->animationEnabled == -1 || curNode->animationEnabled == 1)
					{
						curNode->animationEnabled = 0;
						SkinnedMeshDeformer::EnableAnimation(curNode, false, false);
					}

					SkinnedMeshDeformer::UpdateSkeleton(curNode, false, nullptr, false);
				}
			}
		}

		////////////////////

		int j = 0;
		auto children = curNode->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it, ++j)
		{
			SceneNode* nd = (SceneNode*)(*it);
			nstack.insert(nstack.begin() + j, nd);
		}
	}
}