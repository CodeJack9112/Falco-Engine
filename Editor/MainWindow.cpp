#include "stdafx.h"
#include "MainWindow.h"

/* OGRE */
#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include <OgreParticleSystemManager.h>
#include <OgreRenderTarget.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTexture.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreHighLevelGpuProgram.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreSubEntity.h>
/* OGRE */

#include "../Player/Helpers.h"
#include "../Engine/Engine.h"
#include "../Engine/IO.h"
#include "../Engine/DynamicLines.h"
#include "../Engine/DeferredShading/DeferredShading.h"
#include "../Engine/GUI/ImguiManager.h"
#include "../Engine/EditorSettings.h"
#include "../Engine/TerrainManager.h"
#include "../Engine/CloneTools.h"
#include "../Engine/DeferredShading/DeferredLightCP.h"

#include "../Engine/AudioSource.h"
#include "../Engine/AudioListener.h"
#include "../Engine/RigidBody.h"
#include "../Engine/MeshCollider.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/HingeJoint.h"
#include "../Engine/FixedJoint.h"
#include "../Engine/CharacterJoint.h"
#include "../Engine/NavMeshAgent.h"
#include "../Engine/NavMeshObstacle.h"
#include "../Engine/AnimationList.h"
#include "../Engine/Vehicle.h"

#include "../Engine/NavigationManager.h"
#include "../Engine/SceneSerializer.h"
#include "../Engine/FBXSceneManager.h"
#include "../Engine/GUIDGenerator.h"

#include "../Engine/GUI/ImGUIWidgets.h"

#include "../boost/algorithm/string/replace.hpp"
#include "../boost/algorithm/string.hpp"
#include "../boost/process.hpp"

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_video.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_syswm.h"

#include "imgui_impl_sdl.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "EditorClasses.h"
#include "HierarchyWindow.h"
#include "AssetsWindow.h"
#include "InspectorWindow.h"
#include "SceneWindow.h"
#include "ConsoleWindow.h"
#include "LightingWindow.h"
#include "NavigationWindow.h"
#include "TerrainWindow.h"
#include "PreviewWindow.h"
#include "TreeView.h"
#include "InputHandler.h"
#include "ProjectExporter.h"
#include "DialogRagdollEditor.h"
#include "DialogAbout.h"
#include "DialogAssetExporter.h"
#include "TerrainEditor.h"
#include "Toast.h"

#include "ObjectEditor2.h"
#include "LightEditor2.h"
#include "EntityEditor.h"
#include "CameraEditor2.h"
#include "ParticleSystemEditor2.h"
#include "UIImageEditor2.h"
#include "UIButtonEditor2.h"
#include "UITextEditor2.h"
#include "UICanvasEditor2.h"
#include "ProjectSettingsEditor2.h"

#include "Undo.h"

using namespace Ogre;

//Gizmo* MainWindow::gizmo = nullptr;
Gizmo2* MainWindow::gizmo2 = nullptr;
Camera* MainWindow::mainCamera = nullptr;
Camera* MainWindow::guiCamera = nullptr;
TexturePtr MainWindow::sceneRenderTarget;
MainWindow* MainWindow::self = nullptr;
HierarchyWindow* MainWindow::hierarchyWindow = nullptr;
AssetsWindow* MainWindow::assetsWindow = nullptr;
InspectorWindow* MainWindow::inspectorWindow = nullptr;
SceneWindow* MainWindow::sceneWindow = nullptr;
ConsoleWindow* MainWindow::consoleWindow = nullptr;
LightingWindow* MainWindow::lightingWindow = nullptr;
NavigationWindow* MainWindow::navigationWindow = nullptr;
TerrainWindow* MainWindow::terrainWindow = nullptr;
PreviewWindow* MainWindow::previewWindow = nullptr;
DialogRagdollEditor * MainWindow::ragdollEditor = nullptr;
DialogAbout * MainWindow::aboutDialog = nullptr;
DialogAssetExporter* MainWindow::assetExporter = nullptr;

std::vector<TexturePtr> MainWindow::icons;
std::vector<string> MainWindow::imagesFileFormats = { "jpg", "png", "jpeg", "bmp", "psd", "tga", "gif", "pic", "ppm", "pgm", "hdr", "dds" };

bool MainWindow::loaded = false;
int MainWindow::tabSet = 3;
int MainWindow::runScreenWidth = 1280;
int MainWindow::runScreenHeight = 800;
bool MainWindow::runFullscreen = false;
std::vector<std::pair<int, int>> MainWindow::screenSizes;

string MainWindow::projectDir = "";
string MainWindow::projectName = "";
string MainWindow::projectSettingsDir = "";
string MainWindow::projectAssetsDir = "";
string MainWindow::projectTempDir = "";
string MainWindow::projectAssemblyDir = "";
string MainWindow::projectSolutionDir = "";
string MainWindow::projectLibraryDir = "";
string MainWindow::builtinResourcesPath = "";
string MainWindow::monoDir = "";

string MainWindow::openedSceneName = "";
string MainWindow::openedScenePath = "";
string MainWindow::lastScenePath = "";
ImFont * MainWindow::editorFont = nullptr;
ImFont * MainWindow::editorSmallFont = nullptr;

std::vector<std::function<void()>> MainWindow::onEndUpdateCallbacks;

bool gridVisible = false;

MainWindow::MainWindow()
{
	self = this;
	InputHandler::cameraSpeed = InputHandler::cameraSpeedNormal;

	ragdollEditor = new DialogRagdollEditor();
	aboutDialog = new DialogAbout();
	assetExporter = new DialogAssetExporter();

	hierarchyWindow = new HierarchyWindow();
	assetsWindow = new AssetsWindow();
	inspectorWindow = new InspectorWindow();
	sceneWindow = new SceneWindow();
	consoleWindow = new ConsoleWindow();
	lightingWindow = new LightingWindow();
	navigationWindow = new NavigationWindow();
	terrainWindow = new TerrainWindow();
	previewWindow = new PreviewWindow();
}

MainWindow::~MainWindow()
{
	delete hierarchyWindow;
	delete assetsWindow;
	delete inspectorWindow;
	delete ragdollEditor;
	delete aboutDialog;
	delete sceneWindow;
	delete previewWindow;
}

void MainWindow::Run()
{
	Gdiplus::GdiplusStartupInput sti;
	ULONG_PTR gpToken;
	int n = sizeof sti;
	Gdiplus::GdiplusStartup(&gpToken, &sti, NULL);

	SDL_DisplayMode displayMode;
	SDL_Init(SDL_INIT_VIDEO);

	int request = SDL_GetDesktopDisplayMode(0, &displayMode);
	window = SDL_CreateWindow(StringConvert::cp_convert("Falco Engine [OpenGL]", GetACP(), 65001).c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	
	SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	InitEngine();
	SetupViewport();
	SetupDeferredShading();

	sceneWindow->init();

	LoadLastScene();

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");

	gridVisible = settings.showGrid;
	navMeshVisible = settings.showNavMesh;
	shadowsEnabled = settings.shadowsEnabled;

	SceneManager::shadowsEnabled = shadowsEnabled;

	lines->setVisible(gridVisible);
	lines_2->setVisible(gridVisible);

	sceneWindow->show(settings.sceneWindowVisible);
	consoleWindow->show(settings.consoleWindowVisible);
	inspectorWindow->show(settings.inspectorWindowVisible);
	hierarchyWindow->show(settings.hierarchyWindowVisible);
	assetsWindow->show(settings.assetsWindowVisible);
	lightingWindow->show(settings.lightingWindowVisible);
	navigationWindow->show(settings.navigationWindowVisible);
	terrainWindow->show(settings.terrainWindowVisible);
	previewWindow->show(settings.previewWindowVisible);

	runScreenWidth = settings.runScreenWidth;
	runScreenHeight = settings.runScreenHeight;
	runFullscreen = settings.runFullscreen;

	SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");

	if (navMesh != nullptr)
		navMesh->setVisible(navMeshVisible);

	gizmo2->setSnapToGrid(settings.snapToGrid);
	gizmo2->setSnapToGridSize(settings.snapToGridSize);
	gizmo2->setCenterBase(settings.gizmoCenterBase == 0 ? Gizmo2::CenterBase::CB_CENTER : Gizmo2::CenterBase::CB_PIVOT);

	assetsWindow->addNewFiles();

	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/new.png", "Editor"));		//0
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/open.png", "Editor"));		//1
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/save.png", "Editor"));		//2
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/undo.png", "Editor"));		//3
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/redo.png", "Editor"));		//4
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/select.png", "Editor"));		//5
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/move.png", "Editor"));		//6
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/rotate.png", "Editor"));		//7
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/scale.png", "Editor"));		//8
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/local.png", "Editor"));		//9
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/world.png", "Editor"));		//10
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/magnet.png", "Editor"));		//11
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/play.png", "Editor"));		//12
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/center.png", "Editor"));		//13
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/pivot.png", "Editor"));		//14
	icons.push_back(TextureManager::getSingleton().load("Icons/Toolbar/arrow_down.png", "Editor"));	//15

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	mainCamera->setAspectRatio(static_cast<Real>(w) / static_cast<Real>(h));
	mainCamera->setAutoAspectRatio(true);

	guiCamera->setAspectRatio(static_cast<Real>(w) / static_cast<Real>(h));
	guiCamera->setAutoAspectRatio(true);

	Toast::init();

	SDL_ShowWindow(window);

	SDL_DestroyRenderer(renderer);

	ImGui_ImplSDL2_InitForOpenGL(window, nullptr);

	editorFont = ImguiManager::getSingleton().addFont("Fonts/Roboto-Medium.ttf", "Editor", 15);
	editorSmallFont = ImguiManager::getSingleton().addFont("Fonts/Roboto-Medium.ttf", "Editor", 13.0f);

	SDL_Event evt;

	loaded = true;
	bool pressedWhenHovered = false;
	bool resetMouseButtons = false;
	ImGuiIO& io = ImGui::GetIO();
	int flags = io.ConfigFlags;

	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);

	for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++)
	{
		screenSizes.push_back(make_pair(dm.dmPelsWidth, dm.dmPelsHeight));
	}

	auto end = std::unique(screenSizes.begin(), screenSizes.end());
	screenSizes.erase(end, screenSizes.end());
	sort(screenSizes.begin(), screenSizes.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool { return a.first < b.first; });

	while (!done)
	{
		// Handle events
		while (SDL_PollEvent(&evt))
		{
			// Handle window close event
			if (evt.type == SDL_QUIT)
				done = true;

			Vector2 pos = sceneWindow->getPosition();
			Vector2 sz = sceneWindow->getSize();
			Vector2 mouse = Vector2(evt.button.x, evt.button.y);
			int padding = 6;
			int paddingCorner = 20;

			float paddingTop = sceneWindow->getToolbarHeight();

			bool corner = (mouse.x > pos.x + sz.x - paddingCorner && mouse.x < pos.x + sz.x &&
				mouse.y > pos.y + sz.y - paddingCorner && mouse.y < pos.y + sz.y);

			if (!corner)
			{
				if (mouse.x > pos.x + padding && mouse.x < pos.x + sz.x - padding &&
					mouse.y > pos.y + paddingTop && mouse.y < pos.y + sz.y - padding)
				{
					if (evt.type == SDL_MOUSEBUTTONDOWN)
					{
						if (sceneWindow->isHovered())
						{
							pressedWhenHovered = true;

							Ogre::Vector2 mpos = GetScreenPos(evt.button.x, evt.button.y);

							if (evt.button.button == SDL_BUTTON_LEFT)
								InputHandler::OnLMouseDown(mpos.x, mpos.y);

							if (evt.button.button == SDL_BUTTON_RIGHT)
								InputHandler::OnRMouseDown(mpos.x, mpos.y);

							if (evt.button.button == SDL_BUTTON_MIDDLE)
								InputHandler::OnMMouseDown(mpos.x, mpos.y);
						}
					}
				}
			}

			if (evt.type == SDL_MOUSEBUTTONUP)
			{
				Ogre::Vector2 mpos = GetScreenPos(evt.button.x, evt.button.y);

				if (pressedWhenHovered)
				{
					if (evt.button.button == SDL_BUTTON_LEFT)
						InputHandler::OnLMouseUp(mpos.x, mpos.y);

					if (evt.button.button == SDL_BUTTON_RIGHT)
						InputHandler::OnRMouseUp(mpos.x, mpos.y);

					if (evt.button.button == SDL_BUTTON_MIDDLE)
						InputHandler::OnMMouseUp(mpos.x, mpos.y);

					pressedWhenHovered = false;

					if (!sceneWindow->isHovered())
						resetMouseButtons = true;
				}
			}

			if (sceneWindow->isHovered())
			{
				if (evt.type == SDL_MOUSEWHEEL)
					InputHandler::OnMouseWheel(evt.wheel.y);
			}

			if (evt.type == SDL_MOUSEBUTTONDOWN)
			{
				if (evt.button.button == SDL_BUTTON_LEFT)
					lmb = true;

				if (evt.button.button == SDL_BUTTON_RIGHT)
					rmb = true;
			}

			if (evt.type == SDL_MOUSEBUTTONUP)
			{
				if (evt.button.button == SDL_BUTTON_LEFT)
					lmb = false;

				if (evt.button.button == SDL_BUTTON_RIGHT)
					rmb = false;
			}

			if (evt.type == SDL_MOUSEMOTION)
			{
				Ogre::Vector2 mpos = GetScreenPos(evt.button.x, evt.button.y);
				InputHandler::OnMouseMove(mpos.x, mpos.y);
			}

			if (evt.type == SDL_KEYDOWN)
			{
				if (!ImGui::GetIO().WantCaptureKeyboard)
				{
					InputHandler::OnKeyDown(evt.key.keysym.scancode);

					if (evt.key.keysym.sym == SDLK_LCTRL)
						lctrl = true;

					if (evt.key.keysym.sym == SDLK_LSHIFT)
						lshift = true;

					if (!lmb && !rmb)
					{
						if (lctrl && !lshift)
						{
							if (evt.key.keysym.sym == SDLK_n)
								OnSceneNew();

							if (evt.key.keysym.sym == SDLK_s)
								OnSceneSave();

							if (evt.key.keysym.sym == SDLK_o)
								OnSceneOpen();

							if (evt.key.keysym.sym == SDLK_d)
								OnDuplicate();

							if (evt.key.keysym.sym == SDLK_z)
								OnUndo();

							if (evt.key.keysym.sym == SDLK_y)
								OnRedo();
						}

						if (lctrl && lshift)
						{
							if (evt.key.keysym.sym == SDLK_s)
								OnSceneSaveAs();

							if (evt.key.keysym.sym == SDLK_c)
								OnCopyRenderImage();

							if (evt.key.keysym.sym == SDLK_z)
								OnRedo();
						}

						if (evt.key.keysym.sym == SDLK_q)
							OnGizmoSelect();
						if (evt.key.keysym.sym == SDLK_w)
							OnGizmoMove();
						if (evt.key.keysym.sym == SDLK_e)
							OnGizmoRotate();
						if (evt.key.keysym.sym == SDLK_r)
							OnGizmoScale();
						if (evt.key.keysym.sym == SDLK_t)
						{
							if (gizmo2->getTransformSpace() == Gizmo2::TransformSpace::TS_WORLD)
								OnGizmoLocal();
							else
								OnGizmoWorld();
						}

						if (evt.key.keysym.sym == SDLK_DELETE)
						{
							AddOnEndUpdateCallback([=] () {
								DeleteSelectedNodes();
							});
						}

						if (evt.key.keysym.sym == SDLK_f)
						{
							auto vec = gizmo2->getSelectedObjects();

							if (vec.size() > 0)
							{
								SceneNode* first = vec[0];

								AxisAlignedBox box;
								if (first->getAttachedObject(0)->getMovableType() != EntityFactory::FACTORY_TYPE_NAME)
									box.setExtents(first->_getDerivedPosition(), first->_getDerivedPosition());

								for (auto it = vec.begin(); it != vec.end(); ++it)
								{
									SceneNode* node = *it;
									gizmo2->getNodesBounds(node, box);
								}

								Vector3 sz = box.getSize();
								float mx = 0;
								if (sz.x != INFINITY)
									mx = max(mx, sz.x);
								if (sz.y != INFINITY)
									mx = max(mx, sz.y);
								if (sz.z != INFINITY)
									mx = max(mx, sz.z);

								Vector3 objsCenter = box.getCenter();
								mainCamera->getParentSceneNode()->_setDerivedPosition(objsCenter + (mainCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Z * mx));
							}
						}

						if (evt.key.keysym.sym == SDLK_F5)
							OnPlay();
					}
				}
			}

			if (evt.type == SDL_KEYUP)
			{
				if (!ImGui::GetIO().WantCaptureKeyboard)
				{
					InputHandler::OnKeyUp(evt.key.keysym.scancode);

					if (evt.key.keysym.sym == SDLK_LCTRL)
						lctrl = false;

					if (evt.key.keysym.sym == SDLK_LSHIFT)
						lshift = false;
				}
			}

			if (evt.type == SDL_WINDOWEVENT)
			{
				if (evt.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					//Resize the screen
					GetEngine->GetRenderWindow()->windowMovedOrResized();
				}

				if (evt.window.event == SDL_WINDOWEVENT_RESTORED || evt.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				{
					WindowRestored();
				}
			}

			if (!rmb)
				ImGui_ImplSDL2_ProcessEvent(&evt);
		}

		InputHandler::Update();
		//MainWindow::gizmo->update();

		ImGui_ImplSDL2_NewFrame(window);

		if (resetMouseButtons)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		GetEngine->GetNavigationManager()->Update();
		Root::getSingleton().renderOneFrame();

		if (resetMouseButtons)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags &= flags;
			resetMouseButtons = false;
		}

		if (!self->openScenePath.empty())
		{
			self->OpenSceneInternal(self->openScenePath);
			self->openScenePath = "";
		}

		if (onEndUpdateCallbacks.size() > 0)
		{
			std::vector<std::function<void()>> funcs = onEndUpdateCallbacks;
			onEndUpdateCallbacks.clear();

			for (auto it = funcs.begin(); it != funcs.end(); ++it)
				(*it)();

			funcs.clear();
		}

		SDL_GL_SwapWindow(window);

		if (!InputHandler::IsLMousePressed() && !InputHandler::IsRMousePressed() && !InputHandler::IsMMousePressed() && !ImGui::GetIO().WantCaptureMouse)
			Sleep(25);
	}

	SDL_DestroyWindow(window);

	Vector3 camPos = mainCamera->getParentSceneNode()->_getDerivedPosition();
	Quaternion camRot = mainCamera->getParentSceneNode()->_getDerivedOrientation();

	settings.Load(projectSettingsDir + "Editor.settings");
	settings.sceneWindowVisible = sceneWindow->getVisible();
	settings.consoleWindowVisible = consoleWindow->getVisible();
	settings.inspectorWindowVisible = inspectorWindow->getVisible();
	settings.hierarchyWindowVisible = hierarchyWindow->getVisible();
	settings.assetsWindowVisible = assetsWindow->getVisible();
	settings.lightingWindowVisible = lightingWindow->getVisible();
	settings.navigationWindowVisible = navigationWindow->getVisible();
	settings.terrainWindowVisible = terrainWindow->getVisible();
	settings.previewWindowVisible = previewWindow->getVisible();
	settings.cameraPosition = SVector3(camPos.x, camPos.y, camPos.z);
	settings.cameraRotation = SQuaternion(camRot.x, camRot.y, camRot.z, camRot.w);
	settings.Save(projectSettingsDir + "Editor.settings");
}

void MainWindow::InitEngine()
{
	/* SETUP PATH VARS */
	//Mono path
	CString mono_p = CString(IO::ReadText(Helper::ExePath() + "Mono.conf").c_str());
	mono_p.Replace(_T("\n"), _T(""));
	string monoPath = Helper::ExePath() + CSTR2STRW(mono_p);

	monoDir = monoPath;

	//Assembly path
	string asmPath = projectDir + "Project/bin/Debug/";

	projectAssetsDir = projectDir + "Assets/";
	projectTempDir = projectDir + "Temp/";
	projectSettingsDir = projectDir + "Settings/";
	projectAssemblyDir = projectDir + "Project/bin/Debug/";
	projectSolutionDir = projectDir + "Project/";
	projectLibraryDir = projectDir + "Library/";
	builtinResourcesPath = Helper::ExePath() + "BuiltinResources/";

	/* LOAD GRAPHICS ENGINE */
	GetEngine->SetSystemCP(GetACP());
	GetEngine->SetRootPath(projectDir);
	GetEngine->SetAssetsPath(projectDir + "Assets/");
	GetEngine->SetLibraryPath(projectDir + "Library/");
	GetEngine->SetMonoPath(monoPath);
	GetEngine->SetBuiltinResourcesPath(builtinResourcesPath);
	GetEngine->SetAssemblyPath(asmPath + "MainAssembly.dll");
	GetEngine->SetCachePath(projectDir + "Assets/Cache/");
	GetEngine->SetUseUnpackedResources(true);
	GetEngine->SetImGUICallback(OnGUI);

	if (IO::FileExists(Helper::ExePath() + "cachepath.txt"))
	{
		std::string _txt = IO::ReadText(Helper::ExePath() + "cachepath.txt");
		_txt = StringConvert::cp_convert(_txt, 65001, GetACP());

		::CString d = ::CString(_txt.c_str());
		d.Replace(_T("\n"), _T(""));
		_txt = Helper::CSTR2STR(d) + "/";

		GetEngine->SetCachePath(_txt);
	}

	GetEngine->SetOnResourcesAddedCallback([=]
	{
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(projectDir + "Temp/", "FileSystem", "Temp", false, false);
		ResourceGroupManager::getSingletonPtr()->addResourceLocation(Helper::ExePath() + "Editor/", "FileSystem", "Editor", true, false);
	});

	Ogre::RenderWindow* wnd;

	GetEngine->SetOnAfterRootCreatedCallback([=, &wnd]
	{
		Ogre::NameValuePairList params;
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(window, &info);
		params["externalWindowHandle"] = Ogre::StringConverter::toString(size_t(info.info.win.window));

		wnd = Root::getSingleton().createRenderWindow("Renderer", 1280, 800, false, &params);
		wnd->setVisible(true);

		GetEngine->SetRenderWindow(wnd);
	});

	GetEngine->Init(Engine::Environment::Editor, false);

	GetEngine->GetMonoRuntime()->SetBuildCallback([=](std::string str)
	{
		if (str == "--Done--")
		{
			Toast::showMessage("Compiling finished", TI_COMPILE);
		}
	});

	GetEngine->GetMonoRuntime()->SetLogCallback([=](std::string log)
	{
		consoleWindow->log(log);
	});

	GetEngine->GetMonoRuntime()->SetSolutionPath(projectDir + "Project/");

	GetEngine->GetMonoRuntime()->CompileLibrary();

	Sleep(1000);

	GetEngine->GetMonoRuntime()->OpenLibrary();
	GetEngine->LoadResources(true);

	hierarchyWindow->init();
	assetsWindow->init();
	terrainWindow->init();
	previewWindow->init();

	IO::listFiles(Helper::ExePath() + "Editor/", true, [](std::string dir) {}, [=](std::string dir, std::string file) {
		std::string ext = IO::GetFileExtension(file);
		std::string asset = boost::replace_all_copy(dir + file, Helper::ExePath() + "Editor/", "");

		if (ext._Equal("ttf") || ext._Equal("otf"))
		{
			Ogre::FontPtr mFont = Ogre::FontManager::getSingleton().create(asset, "Editor");
			mFont->_notifyOrigin(Helper::ExePath() + "Editor/" + asset);
			mFont->setType(Ogre::FT_TRUETYPE);
			mFont->setSource(asset);
			mFont->setParameter("size", "18");
			mFont->setParameter("resolution", "192");
			mFont->load();
		}
	});
}

void MainWindow::SetupViewport()
{
	/* SCENE SETUP */
	SceneNode* cameraNode = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("EditorCamera");
	//SceneNode* guiCameraNode = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("EditorUICamera");
	mainCamera = GetEngine->GetSceneManager()->createCamera("EditorCamera");
	guiCamera = GetEngine->GetSceneManager()->createCamera("EditorUICamera");
	cameraNode->attachObject(mainCamera);
	cameraNode->attachObject(guiCamera);

	ColourValue cColor = ColourValue(0.5f, 0.5f, 0.5f, 1.0f);

	//CreateRenderTexture();
	sceneRenderTarget = TextureManager::getSingleton().createManual("SceneViewRTT",
		ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D,
		1024,
		1024,
		0,
		PixelFormat::PF_FLOAT32_RGB,
		Ogre::TU_RENDERTARGET);

	// Check rtt viewport
	RenderTarget* sceneRTT = sceneRenderTarget->getBuffer()->getRenderTarget();
	sceneRTT->removeAllViewports();
	Viewport* sceneViewport = nullptr;
	sceneViewport = sceneRTT->addViewport(mainCamera);
	sceneViewport->setClearEveryFrame(true);
	sceneViewport->setOverlaysEnabled(false);
	sceneViewport->setBackgroundColour(cColor);

	GetEngine->GetRenderWindow()->removeAllViewports();
	mainViewport = GetEngine->GetRenderWindow()->addViewport(guiCamera);
	GetEngine->GetRenderWindow()->addListener(this);
	
	mainViewport->setBackgroundColour(cColor);
	mainViewport->setSkiesEnabled(false);

	//Camera
	mainCamera->setAspectRatio(static_cast<Real>(1280) / static_cast<Real>(800));
	mainCamera->setAutoAspectRatio(true);
	mainCamera->setNearClipDistance(0.5f);
	mainCamera->setFarClipDistance(5500.0f);
	mainCamera->setFOVy(Radian(Degree(60)));

	guiCamera->setAspectRatio(static_cast<Real>(1280) / static_cast<Real>(800));
	guiCamera->setAutoAspectRatio(true);
	guiCamera->setNearClipDistance(0.5f);
	guiCamera->setFarClipDistance(5500.0f);
	guiCamera->setFOVy(Radian(Degree(60)));

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");

	cameraNode->_setDerivedPosition(Vector3(settings.cameraPosition.x, settings.cameraPosition.y, settings.cameraPosition.z));
	cameraNode->_setDerivedOrientation(Quaternion(settings.cameraRotation.w, settings.cameraRotation.x, settings.cameraRotation.y, settings.cameraRotation.z));
	//cameraNode->lookAt(Vector3(0, 5, 0), Ogre::Node::TransformSpace::TS_WORLD);

	//Grid
	std::deque<Ogre::Vector3> points;
	std::deque<Ogre::Vector3> points_2;

	for (int i = -100; i < 101; i++)
	{
		points.push_back(Vector3(i, 0, -100));
		points.push_back(Vector3(i, 0, 100));
	}

	for (int j = -100; j < 101; j++)
	{
		points.push_back(Vector3(-100, 0, j));
		points.push_back(Vector3(100, 0, j));
	}

	for (int i = -25; i < 26; i++)
	{
		points_2.push_back(Vector3(i * 4, 0, -100));
		points_2.push_back(Vector3(i * 4, 0, 100));
	}

	for (int j = -25; j < 26; j++)
	{
		points_2.push_back(Vector3(-100, 0, j * 4));
		points_2.push_back(Vector3(100, 0, j * 4));
	}

	MaterialPtr mat_1 = EditorClasses::CreateColoredMaterial("MaterialGridGray", ColourValue(0.45, 0.45, 0.45, 0.45));
	MaterialPtr mat_2 = EditorClasses::CreateColoredMaterial("MaterialGridLightGray", ColourValue(0.55, 0.55, 0.55, 0.55));

	lines = new DynamicLines(mat_1, RenderOperation::OT_LINE_LIST);
	lines_2 = new DynamicLines(mat_2, RenderOperation::OT_LINE_LIST);
	lines->setCastShadows(false);
	lines_2->setCastShadows(false);

	for (int i = 0; i < points.size(); i++) {
		lines->addPoint(points[i]);
	}

	for (int i = 0; i < points_2.size(); i++) {
		lines_2->addPoint(points_2[i]);
	}

	lines->update();
	lines_2->update();

	SceneNode* linesNode = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("EditorGrid");
	SceneNode* linesNode_2 = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("EditorGrid_2");
	linesNode->attachObject(lines);
	linesNode_2->attachObject(lines_2);
	lines->setRenderQueueGroup(7);
	lines_2->setRenderQueueGroup(8);

	gizmo2 = new Gizmo2();
	gizmo2->init(GetEngine->GetSceneManager(), mainCamera);
	gizmo2->setSelectCallback(OnGizmoSelect);
	gizmo2->setManipulateStartCallback(OnGizmoManipulateStart);
	gizmo2->setManipulateCallback(OnGizmoManipulate);
	gizmo2->setManipulateEndCallback(OnGizmoManipulateEnd);
	//gizmo2->setActionCallback(OnGizmoAction);

	//Gizmo materials
	//Textures for billboards
	//Light
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().load("Icons/Gizmo/light.png", "Editor");
	EditorClasses::CreateBillboardMaterial("LightBillboard", tex);
	//Camera
	tex = Ogre::TextureManager::getSingleton().load("Icons/Gizmo/camera.png", "Editor");
	EditorClasses::CreateBillboardMaterial("CameraBillboard", tex);
	//Particle System
	tex = Ogre::TextureManager::getSingleton().load("Icons/Gizmo/particle_system.png", "Editor");
	EditorClasses::CreateBillboardMaterial("ParticleSystemBillboard", tex);
	/* SCENE SETUP */
}

void MainWindow::SetupDeferredShading()
{
	//GetEngine->GetDeferredShadingSystem()->Cleanup();
	GetEngine->GetDeferredShadingSystem()->removeViewports();
	GetEngine->GetDeferredShadingSystem()->addViewport(mainCamera->getViewport());
	GetEngine->GetDeferredShadingSystem()->addViewport(sceneRenderTarget->getBuffer()->getRenderTarget()->getViewport(0));
	GetEngine->GetDeferredShadingSystem()->addViewport(GetPreviewWindow()->getViewport());
	GetEngine->GetDeferredShadingSystem()->setCamera(mainCamera);
	//GetEngine->GetDeferredShadingSystem()->initialize();
}

void MainWindow::SerializeScene(string path)
{
	SceneSerializer ser;
	ser.Serialize(GetEngine->GetSceneManager(), path);

	assetsWindow->addNewFiles();

	Toast::showMessage("Scene saved", TI_SAVE);
}

void MainWindow::LoadSettings()
{
	ColourValue ambient = GetEngine->GetSceneManager()->getAmbientLight();
	MaterialPtr skyboxMaterial = GetEngine->GetSceneManager()->getSkyBoxMaterial();
	if (skyboxMaterial != nullptr)
		lightingWindow->setSkyBoxMaterial(skyboxMaterial->getName());
	else
		lightingWindow->setSkyBoxMaterial("None");

	lightingWindow->setAmbientLightColor(ambient);
	lightingWindow->setDefaultLightmapSize(GetEngine->GetDefaultLightmapSize());
	lightingWindow->setRegionLightmapSize(GetEngine->GetRegionLightmapSize());
	lightingWindow->setLightmapBlurRadius(GetEngine->GetLightmapBlurRadius());

	NavigationManager* navMgr = GetEngine->GetNavigationManager();

	navigationWindow->setWalkableSlopeAngle(navMgr->GetWalkableSlopeAngle());
	navigationWindow->setWalkableRadius(navMgr->GetWalkableRadius());
	navigationWindow->setWalkableHeight(navMgr->GetWalkableHeight());
	navigationWindow->setWalkableClimb(navMgr->GetWalkableClimb());
	navigationWindow->setMinRegionArea(navMgr->GetMinRegionArea());
	navigationWindow->setMergeRegionArea(navMgr->GetMergeRegionArea());
	navigationWindow->setMaxSimplificationError(navMgr->GetMaxSimplificationError());
	navigationWindow->setMaxEdgeLength(navMgr->GetMaxEdgeLen());
	navigationWindow->setCellSize(navMgr->GetCellSize());
	navigationWindow->setCellHeight(navMgr->GetCellHeight());
}

Ogre::Vector2 MainWindow::GetWindowSize()
{
	int w, h;
	SDL_GetWindowSize(self->window, &w, &h);

	return Ogre::Vector2(w, h);
}

void MainWindow::OnGUI()
{
	ImGui::PushFont(editorFont);

	bool open = true;
	ImGuiID dockspaceID = ImGui::GetID("HUB_DockSpace");

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowPos(ImVec2(0, 58));
	ImGui::SetNextWindowSize(ImVec2(GetWindowSize().x, GetWindowSize().y - 58));
	ImGui::Begin("Main", &open, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);
	ImGui::PopStyleVar(3);

	if (ImGui::DockBuilderGetNode(dockspaceID) == nullptr)
	{
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, ImVec2(GetWindowSize().x, GetWindowSize().y - 58));

		ImGuiID dock_main_id = dockspaceID;
		ImGuiID dock_scene_id;
		ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.35f, nullptr, &dock_scene_id);
		ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_scene_id, ImGuiDir_Down, 0.15f, nullptr, &dock_scene_id);
		ImGuiID dock_prop_id;
		dock_right_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Right, 0.55f, nullptr, &dock_prop_id);
		ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.4f, nullptr, &dock_right_id);
		ImGuiID dock_right_down_2_id = ImGui::DockBuilderSplitNode(dock_right_down_id, ImGuiDir_Down, 0.3f, nullptr, &dock_right_down_id);
		ImGuiID dock_prop_up_id = ImGui::DockBuilderSplitNode(dock_prop_id, ImGuiDir_Up, 0.5f, nullptr, nullptr);
		ImGuiID dock_prop_down_id = ImGui::DockBuilderSplitNode(dock_prop_id, ImGuiDir_Down, 0.5f, nullptr, nullptr);

		ImGui::DockBuilderDockWindow("Scene", dock_scene_id);
		ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
		ImGui::DockBuilderDockWindow("Lighting", dock_right_id);
		ImGui::DockBuilderDockWindow("Navigation", dock_right_id);
		ImGui::DockBuilderDockWindow("Terrain", dock_right_down_id);
		ImGui::DockBuilderDockWindow("Preview", dock_right_down_2_id);
		ImGui::DockBuilderDockWindow("Hierarchy", dock_prop_up_id);
		ImGui::DockBuilderDockWindow("Assets", dock_prop_down_id);
		ImGui::DockBuilderDockWindow("Console", dock_down_id);
		
		ImGui::DockBuilderFinish(dockspaceID);
	}
	
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoWindowMenuButton);
	ImGui::End();

	DrawMainMenu();
	DrawMainToolbar();

	hierarchyWindow->update();
	assetsWindow->update();
	inspectorWindow->update();
	lightingWindow->update();
	navigationWindow->update();
	terrainWindow->update();
	previewWindow->update();
	sceneWindow->update();
	consoleWindow->update();

	ragdollEditor->update();
	aboutDialog->update();
	assetExporter->update();

	if (tabSet > 0)
	{
		if (ImGuiWindow* window = ImGui::FindWindowByName("Inspector"))
		{
			tabSet -= 1;

			if (window->DockIsActive)
			{
				ImGuiID tab_id = ImHashStr("Inspector");
				ImGui::KeepAliveID(tab_id);
				window->DockNode->TabBar->NextSelectedTabId = tab_id;
			}
		}
	}

	Toast::update();
	ProjectExporter::UpdateProgressbar();

	ImGui::PopFont();
}

void MainWindow::DrawMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl+N")) { AddOnEndUpdateCallback([=]() { self->OnSceneNew(); }); }
			if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) { AddOnEndUpdateCallback([=]() { self->OnSceneOpen(); }); }
			if (ImGui::MenuItem("Save Scene", "Ctrl+S")) { AddOnEndUpdateCallback([=]() { self->OnSceneSave(); }); }
			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) { AddOnEndUpdateCallback([=]() { self->OnSceneSaveAs(); }); }
			ImGui::Separator();
			if (ImGui::MenuItem("New Project...", "")) { AddOnEndUpdateCallback([=]() { self->OnProjectOpen(); }); }
			if (ImGui::MenuItem("Open Project...", "")) { AddOnEndUpdateCallback([=]() { self->OnProjectOpen(); }); }
			ImGui::Separator();
			if (ImGui::MenuItem("Project Settings", "")) { AddOnEndUpdateCallback([=]() { self->OnProjectSettings(); }); }
			if (ImGui::BeginMenu("Build", ""))
			{
				if (ImGui::MenuItem("Windows x64", "")) { AddOnEndUpdateCallback([=]() { self->OnProjectBuildWin64(); }); }
				//if (ImGui::MenuItem("WebGL", "")) { self->OnProjectBuildWebGL(); }
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "")) { AddOnEndUpdateCallback([=]() { self->OnExit(); }); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) { AddOnEndUpdateCallback([=]() { self->OnUndo(); }); }
			if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) { AddOnEndUpdateCallback([=]() { self->OnRedo(); }); }
			ImGui::Separator();
			if (ImGui::MenuItem("Duplicate", "Ctrl+D")) { AddOnEndUpdateCallback([=]() { self->OnDuplicate(); }); }
			if (ImGui::MenuItem("Align With View", "Ctrl+Shift+A")) { AddOnEndUpdateCallback([=]() { self->OnAlignWithView(); }); }
			ImGui::Separator();
			if (ImGui::MenuItem("Copy Render Image", "Ctrl+Shift+C")) { AddOnEndUpdateCallback([=]() { self->OnCopyRenderImage(); }); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create Object"))
		{
			CreateObjectMenu();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add Component"))
		{
			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("Audio Source", "")) { AddOnEndUpdateCallback([=]() { self->OnAddAudioSource(); }); }
				if (ImGui::MenuItem("Audio Listener", "")) { AddOnEndUpdateCallback([=]() { self->OnAddAudioListener(); }); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Physics"))
			{
				if (ImGui::MenuItem("Rigidbody", "")) { AddOnEndUpdateCallback([=]() { self->OnAddRigidbody(); }); }
				if (ImGui::MenuItem("Vehicle", "")) { AddOnEndUpdateCallback([=]() { self->OnAddVehicle(); }); }
				ImGui::Separator();
				if (ImGui::MenuItem("Box Collider", "")) { AddOnEndUpdateCallback([=]() { self->OnAddBoxCollider(); }); }
				if (ImGui::MenuItem("Sphere Collider", "")) { AddOnEndUpdateCallback([=]() { self->OnAddSphereCollider(); }); }
				if (ImGui::MenuItem("Capsule Collider", "")) { AddOnEndUpdateCallback([=]() { self->OnAddCapsuleCollider(); }); }
				if (ImGui::MenuItem("Mesh Collider", "")) { AddOnEndUpdateCallback([=]() { self->OnAddMeshCollider(); }); }
				ImGui::Separator();
				if (ImGui::MenuItem("Fixed Joint", "")) { AddOnEndUpdateCallback([=]() { self->OnAddFixedJoint(); }); }
				if (ImGui::MenuItem("Hinge Joint", "")) { AddOnEndUpdateCallback([=]() { self->OnAddHingeJoint(); }); }
				if (ImGui::MenuItem("Character Joint", "")) { AddOnEndUpdateCallback([=]() { self->OnAddCharacterJoint(); }); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Navigation"))
			{
				if (ImGui::MenuItem("NavMesh Agent", "")) { AddOnEndUpdateCallback([=]() { self->OnAddNavMeshAgent(); }); }
				if (ImGui::MenuItem("NavMesh Obstacle", "")) { AddOnEndUpdateCallback([=]() { self->OnAddNavMeshObstacle(); }); }
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Misc"))
			{
				if (ImGui::MenuItem("Animation", "")) { AddOnEndUpdateCallback([=]() { self->OnAddAnimationList(); }); }
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Assets"))
		{
			if (ImGui::MenuItem("Export Package", "")) { AddOnEndUpdateCallback([=]() { self->OnExportPackage(); }); }
			if (ImGui::MenuItem("Import Package", "")) { AddOnEndUpdateCallback([=]() { self->OnImportPackage(); }); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");
			bool nav = navMesh != nullptr && self->navMeshVisible;

			if (ImGui::MenuItem("Show Grid", "", self->gridVisible)) { AddOnEndUpdateCallback([=]() { self->OnShowGrid(); }); }
			if (ImGui::MenuItem("Show NavMesh", "", nav)) { AddOnEndUpdateCallback([=]() { self->OnShowNavMesh(); }); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Scene", "", sceneWindow->getVisible())) { sceneWindow->show(!sceneWindow->getVisible()); }
			if (ImGui::MenuItem("Inspector", "", inspectorWindow->getVisible())) { inspectorWindow->show(!inspectorWindow->getVisible()); }
			if (ImGui::MenuItem("Hierarchy", "", hierarchyWindow->getVisible())) { hierarchyWindow->show(!hierarchyWindow->getVisible()); }
			if (ImGui::MenuItem("Assets", "", assetsWindow->getVisible())) { assetsWindow->show(!assetsWindow->getVisible()); }
			if (ImGui::MenuItem("Lighting", "", lightingWindow->getVisible())) { lightingWindow->show(!lightingWindow->getVisible()); }
			if (ImGui::MenuItem("Navigation", "", navigationWindow->getVisible())) { navigationWindow->show(!navigationWindow->getVisible()); }
			if (ImGui::MenuItem("Terrain", "", terrainWindow->getVisible())) { terrainWindow->show(!terrainWindow->getVisible()); }
			if (ImGui::MenuItem("Console", "", consoleWindow->getVisible())) { consoleWindow->show(!consoleWindow->getVisible()); }
			if (ImGui::MenuItem("Preview", "", previewWindow->getVisible())) { previewWindow->show(!previewWindow->getVisible()); }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About Falco Engine", "")) { self->OnAbout(); }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void MainWindow::CreateObjectMenu()
{
	if (ImGui::MenuItem("Empty", "")) { self->OnCreateEmpty(); }
	if (ImGui::MenuItem("Camera", "")) { self->OnCreateCamera(); }
	if (ImGui::MenuItem("Terrain", "")) { self->OnCreateTerrain(); }
	ImGui::Separator();
	if (ImGui::BeginMenu("Light"))
	{
		if (ImGui::MenuItem("Point Light", "")) { self->OnCreatePointLight(); }
		if (ImGui::MenuItem("Spot Light", "")) { self->OnCreateSpotLight(); }
		if (ImGui::MenuItem("Directional Light", "")) { self->OnCreateDirectionalLight(); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Physics"))
	{
		if (ImGui::MenuItem("Ragdoll", "")) { self->OnCreateRagdoll(); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Effects"))
	{
		if (ImGui::MenuItem("Particle System", "")) { self->OnCreateParticleSystem(); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Primitives"))
	{
		if (ImGui::MenuItem("Cube", "")) { self->OnCreateCube(); }
		if (ImGui::MenuItem("Sphere", "")) { self->OnCreateSphere(); }
		if (ImGui::MenuItem("Plane", "")) { self->OnCreatePlane(); }
		if (ImGui::MenuItem("Capsule", "")) { self->OnCreateCapsule(); }
		if (ImGui::MenuItem("Cylinder", "")) { self->OnCreateCylinder(); }
		if (ImGui::MenuItem("Pyramid", "")) { self->OnCreatePyramid(); }
		if (ImGui::MenuItem("Cone", "")) { self->OnCreateCone(); }
		if (ImGui::MenuItem("Tube", "")) { self->OnCreateTube(); }
		if (ImGui::MenuItem("Torus", "")) { self->OnCreateTorus(); }
		if (ImGui::MenuItem("Teapot", "")) { self->OnCreateTeapot(); }
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("UI"))
	{
		if (ImGui::MenuItem("Canvas", "")) { self->OnCreateCanvas(); }
		ImGui::Separator();
		if (ImGui::MenuItem("Image", "")) { self->OnCreateImage(); }
		if (ImGui::MenuItem("Text", "")) { self->OnCreateText(); }
		if (ImGui::MenuItem("Button", "")) { self->OnCreateButton(); }
		ImGui::EndMenu();
	}
}

void MainWindow::addChangedAsset(std::string path)
{
	if (std::find(changedAssets.begin(), changedAssets.end(), path) == changedAssets.end())
		changedAssets.push_back(path);
}

static void HelpMarker(const char* desc)
{
	if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f)
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void MainWindow::DrawMainToolbar()
{
	if (!loaded)
		return;

	bool open = true;
	Ogre::Vector2 ws = GetWindowSize();
	ImVec2 pos = ImVec2(0, 20);
	ImVec2 size = ImVec2(ws.x, 38);

	bool snap = gizmo2->getSnapToGrid();
	bool gizmoSelect = gizmo2->getGizmoType() == Gizmo2::GizmoType::GT_SELECT;
	bool gizmoMove = gizmo2->getGizmoType() == Gizmo2::GizmoType::GT_MOVE;
	bool gizmoRotate = gizmo2->getGizmoType() == Gizmo2::GizmoType::GT_ROTATE;
	bool gizmoScale = gizmo2->getGizmoType() == Gizmo2::GizmoType::GT_SCALE;
	bool gizmoLocal = gizmo2->getTransformSpace() == Gizmo2::TransformSpace::TS_LOCAL;
	bool gizmoWorld = gizmo2->getTransformSpace() == Gizmo2::TransformSpace::TS_WORLD;
	bool gizmoCenter = gizmo2->getCenterBase() == Gizmo2::CenterBase::CB_CENTER;
	bool gizmoPivot = gizmo2->getCenterBase() == Gizmo2::CenterBase::CB_PIVOT;

	ImVec4 col3 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col4 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col5 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col6 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col7 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col8 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col9 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col10 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col11 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col12 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	ImVec4 col13 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

	if (snap) col3 = ImVec4(1, 1, 1, 1);
	if (gizmoSelect) col4 = ImVec4(1, 1, 1, 1);
	if (gizmoMove) col5 = ImVec4(1, 1, 1, 1);
	if (gizmoRotate) col6 = ImVec4(1, 1, 1, 1);
	if (gizmoScale) col7 = ImVec4(1, 1, 1, 1);
	if (gizmoLocal) col8 = ImVec4(1, 1, 1, 1);
	if (gizmoWorld) col9 = ImVec4(1, 1, 1, 1);
	if (gizmoCenter) col10 = ImVec4(1, 1, 1, 1);
	if (gizmoPivot) col11 = ImVec4(1, 1, 1, 1);

	if (Undo::IsUndoAvailable()) col12 = ImVec4(1, 1, 1, 1);
	if (Undo::IsRedoAvailable()) col13 = ImVec4(1, 1, 1, 1);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	
	ImGui::Begin("Tools", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking);
	
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	
	if (ImGui::ImageButton((void*)icons[0]->getHandle(), ImVec2(16, 16))) { AddOnEndUpdateCallback([=]() { self->OnSceneNew(); }); } // New
	ImGui::SameLine(); HelpMarker("New scene");

	if (ImGui::ImageButton((void*)icons[1]->getHandle(), ImVec2(16, 16))) { AddOnEndUpdateCallback([=]() { self->OnSceneOpen(); }); } // Open
	ImGui::SameLine(); HelpMarker("Open scene");
	if (ImGui::ImageButton((void*)icons[2]->getHandle(), ImVec2(16, 16))) { AddOnEndUpdateCallback([=]() { self->OnSceneSave(); }); } // Save
	ImGui::SameLine(); HelpMarker("Save scene");
	
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::ImageButton((void*)icons[3]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col12)) { AddOnEndUpdateCallback([=]() { self->OnUndo(); }); } // Undo
	ImGui::SameLine(); HelpMarker("Undo");
	if (ImGui::ImageButton((void*)icons[4]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col13)) { AddOnEndUpdateCallback([=]() { self->OnRedo(); }); } // Redo
	ImGui::SameLine(); HelpMarker("Redo");

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::ImageButton((void*)icons[5]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col4)) { self->OnGizmoSelect(); } // Select
	ImGui::SameLine(); HelpMarker("Select");
	if (ImGui::ImageButton((void*)icons[6]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col5)) { self->OnGizmoMove(); } // Move
	ImGui::SameLine(); HelpMarker("Move");
	if (ImGui::ImageButton((void*)icons[7]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col6)) { self->OnGizmoRotate(); } // Rotate
	ImGui::SameLine(); HelpMarker("Rotate");
	if (ImGui::ImageButton((void*)icons[8]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col7)) { self->OnGizmoScale(); } // Scale
	ImGui::SameLine(); HelpMarker("Scale");

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::ImageButton((void*)icons[13]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col10)) { self->OnGizmoCenter(); } // Center
	ImGui::SameLine(); HelpMarker("Center");
	if (ImGui::ImageButton((void*)icons[14]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col11)) { self->OnGizmoPivot(); } // Pivot
	ImGui::SameLine(); HelpMarker("Pivot");

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::ImageButton((void*)icons[9]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col8)) { self->OnGizmoLocal(); } // Local
	ImGui::SameLine(); HelpMarker("Local");
	if (ImGui::ImageButton((void*)icons[10]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col9)) { self->OnGizmoWorld(); } // World
	ImGui::SameLine(); HelpMarker("World");

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::ImageButton((void*)icons[11]->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col3)) { self->OnGizmoSnapToGrid(); } // Snap to grid
	ImGui::SameLine(); HelpMarker("Snap to grid");
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7);
	if (ImGui::ImageButtonWithID((void*)icons[15]->getHandle(), ImVec2(8, 16), ImGui::GetCurrentWindow()->GetIDNoKeepAlive("magnetValues")))
	{
		ImGui::OpenPopup("magnet_values_popup");
	}

	if (ImGui::BeginPopup("magnet_values_popup"))
	{
		bool selected = false;

		if (ImGui::Selectable("0.0625", gizmo2->getSnapToGridSize() == 0.0625f))
		{
			gizmo2->setSnapToGridSize(0.0625f);
			selected = true;
		}
		if (ImGui::Selectable("0.125", gizmo2->getSnapToGridSize() == 0.125f))
		{
			gizmo2->setSnapToGridSize(0.125f);
			selected = true;
		}
		if (ImGui::Selectable("0.25", gizmo2->getSnapToGridSize() == 0.25f))
		{
			gizmo2->setSnapToGridSize(0.25f);
			selected = true;
		}
		if (ImGui::Selectable("0.5", gizmo2->getSnapToGridSize() == 0.5f))
		{
			gizmo2->setSnapToGridSize(0.5f);
			selected = true;
		}
		if (ImGui::Selectable("1.0", gizmo2->getSnapToGridSize() == 1.0f))
		{
			gizmo2->setSnapToGridSize(1.0f);
			selected = true;
		}
		if (ImGui::Selectable("2.0", gizmo2->getSnapToGridSize() == 2.0f))
		{
			gizmo2->setSnapToGridSize(2.0f);
			selected = true;
		}

		if (selected)
		{
			EditorSettings settings;
			settings.Load(projectSettingsDir + "Editor.settings");
			settings.snapToGridSize = gizmo2->getSnapToGridSize();
			settings.Save(projectSettingsDir + "Editor.settings");
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::ImageButton((void*)icons[12]->getHandle(), ImVec2(16, 16))) { self->OnPlay(); } // Play
	ImGui::SameLine(); HelpMarker("Play");
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7);
	if (ImGui::ImageButtonWithID((void*)icons[15]->getHandle(), ImVec2(8, 16), ImGui::GetCurrentWindow()->GetIDNoKeepAlive("screenSizes")))
	{
		ImGui::OpenPopup("screen_resolutions_popup");
	}

	if (ImGui::BeginPopup("screen_resolutions_popup"))
	{
		if (ImGui::MenuItem("Fullscreen", "", runFullscreen))
		{
			runFullscreen = !runFullscreen;
			EditorSettings settings;
			settings.Load(projectSettingsDir + "Editor.settings");
			settings.runFullscreen = runFullscreen;
			settings.Save(projectSettingsDir + "Editor.settings");
		}

		ImGui::Separator();

		for (auto it = screenSizes.begin(); it != screenSizes.end(); ++it)
		{
			std::string res = to_string(it->first) + "x" + to_string(it->second);

			if (ImGui::MenuItem(res.c_str(), "", runScreenWidth == it->first && runScreenHeight == it->second))
			{
				runScreenWidth = it->first;
				runScreenHeight = it->second;

				EditorSettings settings;
				settings.Load(projectSettingsDir + "Editor.settings");
				settings.runScreenWidth = it->first;
				settings.runScreenHeight = it->second;
				settings.Save(projectSettingsDir + "Editor.settings");
			}
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	ImGui::PopStyleVar();

	ImGui::End();

	ImGui::PopStyleVar();
}

HWND MainWindow::GetWindowHandle()
{
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(self->window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;

	return hwnd;
}

void MainWindow::ProcessSceneNode(SceneNode* root, bool addRoot, bool addNodes, bool processBillboards)
{
	ProcessSceneNodeInternal(root, addRoot, addNodes, processBillboards);
}

void MainWindow::ProcessSceneNodeInternal(SceneNode* root, bool addRoot, bool addNodes, bool processBillboards)
{
	if (addRoot)
	{
		if (addNodes)
			hierarchyWindow->addNode(root, root->getParent()->getName(), false);
		if (root->getAttachedObjects().size() > 0)
		{
			if (processBillboards)
				ProcessBillboard(root, root->getAttachedObject(0));
		}
	}

	Node::ChildNodeIterator itr = root->getChildIterator();

	while (itr.hasMoreElements())
	{
		Node* child = itr.getNext();

		if (!GetEngine->IsEditorObject((SceneNode*)child))
		{
			if (addNodes)
				hierarchyWindow->addNode((SceneNode*)child, child->getParent()->getName());

			if (processBillboards)
			{
				SceneNode::ObjectIterator it = ((SceneNode*)child)->getAttachedObjectIterator();

				while (it.hasMoreElements())
				{
					MovableObject* movObj = it.getNext();
					ProcessBillboard((SceneNode*)child, movObj);
				}
			}

			ProcessSceneNodeInternal((SceneNode*)child, false, addNodes, processBillboards);
		}
	}
}

bool MainWindow::CheckObjectsType(std::string typeName, std::vector<SceneNode*>& nodes)
{
	bool result = true;

	for (std::vector<SceneNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if ((*it)->getAttachedObjects().size() > 0)
		{
			MovableObject* obj = (*it)->getAttachedObject(0);

			if (obj->getMovableType() != typeName)
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

void MainWindow::preRenderTargetUpdate(const RenderTargetEvent& evt)
{
	GetEngine->GetSceneManager()->getRootSceneNode()->setVisible(false, true, false, false);
}

void MainWindow::postRenderTargetUpdate(const RenderTargetEvent& evt)
{
	GetEngine->GetSceneManager()->getRootSceneNode()->setVisible(true, true, false, false);
	lines->setVisible(gridVisible);
	lines_2->setVisible(gridVisible);
}

void MainWindow::CreateRenderTexture()
{
	if (sceneWindow->getSize().x > 0 && sceneWindow->getSize().y > 0)
	{
		if (sceneRenderTarget != nullptr)
			TextureManager::getSingleton().remove(sceneRenderTarget);

		ColourValue cColor = ColourValue(0.5f, 0.5f, 0.5f, 1.0f);

		sceneRenderTarget = TextureManager::getSingleton().createManual("SceneViewRTT",
			ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			sceneWindow->getSize().x,
			sceneWindow->getSize().y,
			0,
			PixelFormat::PF_FLOAT32_RGB,
			Ogre::TU_RENDERTARGET);

		// Check rtt viewport
		RenderTarget* sceneRTT = sceneRenderTarget->getBuffer()->getRenderTarget();
		sceneRTT->removeAllViewports();
		Viewport* sceneViewport = nullptr;
		sceneViewport = sceneRTT->addViewport(mainCamera);
		sceneViewport->setClearEveryFrame(true);
		sceneViewport->setOverlaysEnabled(false);
		sceneViewport->setBackgroundColour(cColor);
	}
}

void MainWindow::UpdateViewport()
{
	self->CreateRenderTexture();
	self->SetupDeferredShading();
}

void MainWindow::DeleteSelectedNodes()
{
	ObjectEditor2::resetBufferObjects();

	TreeView* treeView = hierarchyWindow->getTreeView();
	auto deleteNodes = treeView->getSelectedNodes();

	if (deleteNodes.size() > 0)
	{
		for (auto it = deleteNodes.begin(); it != deleteNodes.end(); ++it)
		{
			SceneNode* node = GetEngine->GetSceneManager()->getSceneNodeFast(*it);
			if (node != nullptr && !GetEngine->IsEditorObject(node))
			{
				TreeNode* _node = treeView->getNodeByName(*it, treeView->getRootNode());
				treeView->deleteNode(_node);

				Engine::Destroy(node);
			}
		}

		gizmo2->clearWireframes();
		gizmo2->clearSelection();
		gizmo2->callSelectCallback();
		gizmo2->updatePosition();
		GetEngine->UpdateSceneNodeIndexes();

		DeferredLightRenderOperation::UpdateStaticShadowmaps();
		GetEngine->GetNavigationManager()->setNavMeshIsDirty();

		deleteNodes.clear();
	}
}

void MainWindow::WindowRestored()
{
	if (GetEngine->IsInitialized())
	{
		assetsWindow->reloadFiles();

		if (changedAssets.size() > 0)
			consoleWindow->clear();

		bool reloadScene = false;

		//Assets hot reloading
		for (auto it = changedAssets.begin(); it != changedAssets.end(); ++it)
		{
			string fn = *it;
			string ext = IO::GetFileExtension(fn);

			if (ext == "glslv" || ext == "glslf")
			{
				if (!HighLevelGpuProgramManager::getSingleton().resourceExists(fn, "Assets"))
					continue;

				HighLevelGpuProgramPtr prog = HighLevelGpuProgramManager::getSingleton().getByName(fn, "Assets");

				string _f = prog->getSourceFile();

				if (_f.empty())
					_f = prog->getName();

				string _base = projectAssetsDir;
				if (!IO::FileExists(projectAssetsDir + _f))
					_base = builtinResourcesPath;

				string newSource = "";

				if (IO::FileExists(_base + _f))
					newSource = IO::ReadText(_base + _f);

				if (boost::replace_all_copy(prog->getSource(), "\n", "") != boost::replace_all_copy(newSource, "\n", ""))
				{
					prog->setSourceFile(_f);
					prog->setSource(newSource, false);

					prog->load();
					prog->reload();
					prog->reload();

					ShaderLog(prog);

					MapIterator materials = MaterialManager::getSingleton().getResourceIterator();
					while (materials.hasMoreElements())
					{
						MaterialPtr mat = static_pointer_cast<Material>(materials.getNext());

						for (int i = 0; i < mat->getNumTechniques(); ++i)
						{
							for (int j = 0; j < mat->getTechnique(i)->getNumPasses(); ++j)
							{
								Pass* pass = mat->getTechnique(i)->getPass(j);

								//Reassign shaders
								if (pass->hasVertexProgram())
								{
									if (pass->getVertexProgramName() == prog->getName())
									{
										pass->setVertexProgram("", false);
										pass->setVertexProgram(prog->getName(), false);
									}
								}

								if (pass->hasFragmentProgram())
								{
									if (pass->getFragmentProgramName() == prog->getName())
									{
										pass->setFragmentProgram("", false);
										pass->setFragmentProgram(prog->getName(), false);
									}
								}

								//Lookup shader variations
								if (pass->GetPreprocessorDefines().size() > 0)
								{
									std::string _defines = "";
									int jj = 0;
									int cnt = 0;

									for (auto it1 = pass->GetPreprocessorDefines().begin(); it1 != pass->GetPreprocessorDefines().end(); ++it1)
									{
										if (it1->second)
										{
											++cnt;
										}
									}

									if (cnt > 0)
									{
										for (auto it1 = pass->GetPreprocessorDefines().begin(); it1 != pass->GetPreprocessorDefines().end(); ++it1)
										{
											if (it1->second)
											{
												_defines += it1->first;

												++jj;

												if (jj < cnt)
												{
													_defines += "_";
												}
											}
										}

										string _name = prog->getName() + "_" + _defines;
										if (HighLevelGpuProgramManager::getSingleton().resourceExists(_name, "Assets"))
										{
											HighLevelGpuProgramPtr _prog = HighLevelGpuProgramManager::getSingleton().getByName(_name, "Assets");

											_prog->setSourceFile(_f);
											_prog->setSource(newSource, false);

											_prog->load();
											_prog->reload();
											_prog->reload();
										}
									}
								}
							}
						}

					}
				}
			}

			if (ext == "fbx")
			{
				FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(fn, "Assets");

				if (!FBXSceneManager::getSingleton().resourceExists(fn, "Assets"))
					continue;

				fbx->clearCache();

				reloadScene = true;
			}

			if (find(imagesFileFormats.begin(), imagesFileFormats.end(), ext) != imagesFileFormats.end())
			{
				TexturePtr tex = TextureManager::getSingleton().load(fn, "Assets");

				if (tex == nullptr)
					continue;

				AddOnEndUpdateCallback([=]()
				{
					previewWindow->clearPreview();

					TextureManager::getSingleton().remove(tex);

					string _fpath = MainWindow::GetProjectAssetsDir() + fn;
					std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
					if (ifs.is_open())
					{
						Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
						Ogre::Image img;
						img.load(data_stream, ext);
						Ogre::TextureManager::getSingleton().loadImage(fn, "Assets", img);
						ifs.close();
					}

					AssetsWindow::reassignTexture(fn, fn);
				});
			}
		}

		if (changedAssets.size() > 0)
		{
			changedAssets.clear();
			inspectorWindow->updateCurrentEditor();
		}

		if (scriptsChanged)
		{
			scriptsChanged = false;
			CompileScripts();
		}

		if (reloadScene)
		{
			AddOnEndUpdateCallback([=]()
			{
				previewWindow->clearPreview();
				TerrainEditor::SetTerrainMode(false);
				hierarchyWindow->getTreeView()->clear();
				gizmo2->selectObject(nullptr, false);

				string lastScene = GetEngine->loadedScene;
				string tempPath = projectTempDir + GUIDGenerator::genGuid() + ".dump";

				SceneSerializer* ser = new SceneSerializer();
				ser->Serialize(GetEngine->GetSceneManager(), tempPath);
				ser->Deserialize(GetEngine->GetSceneManager(), tempPath);
				delete ser;

				IO::FileDelete(tempPath);

				GetEngine->loadedScene = lastScene;
				GetEngine->GetNavigationManager()->SetLoadedScene(lastScene);

				SceneNode* root = GetEngine->GetSceneManager()->getRootSceneNode();
				ProcessSceneNode(root);

				GetEngine->GetTerrainManager()->SetPagedGeometryCamera(mainCamera);

				SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");

				if (navMesh != nullptr)
					navMesh->setVisible(navMeshVisible);
			});
		}
		else
		{
			hierarchyWindow->refreshHierarchy();
		}
	}
}

void MainWindow::CompileScripts()
{
	MonoRuntime* mono = GetEngine->GetMonoRuntime();

	GetEngine->GetMonoRuntime()->RebuildClassList();
	consoleWindow->clear();
	mono->CloseLibrary(false);

	//frm->GetStatusBar()->SetPaneText(frm->GetStatusBar()->CommandToIndex(ID_INDICATOR_BUILD_STATUS), CString("Compiling scripts..."));
	//theApp.compiled = false;
	mono->CompileLibrary(MonoRuntime::CompileConfiguration::Debug, true);
	//frm->GetStatusBar()->SetPaneText(frm->GetStatusBar()->CommandToIndex(ID_INDICATOR_BUILD_STATUS), CString(""));

	consoleWindow->log(mono->GetLastLog());

	mono->OpenLibrary();
	mono->CreateObjects();

	mono->MonoScriptSetFieldsAllNodes(GetEngine->GetSceneManager()->getRootSceneNode());
}

void MainWindow::ProcessBillboard(SceneNode* child, MovableObject* movObj)
{
	if (movObj->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
	{
		//Billboard image of point light
		BillboardSet* billboardSet = EditorClasses::CreateBillboard(GetEngine->GetSceneManager(), child, child->getName() + "_gizmo", "LightBillboard");
	}
	if (movObj->getMovableType() == "Camera")
	{
		//Billboard image of camera
		BillboardSet* billboardSet = EditorClasses::CreateBillboard(GetEngine->GetSceneManager(), child, child->getName() + "_gizmo", "CameraBillboard");
	}
	if (movObj->getMovableType() == ParticleSystemFactory::FACTORY_TYPE_NAME)
	{
		//Billboard image of camera
		BillboardSet* billboardSet = EditorClasses::CreateBillboard(GetEngine->GetSceneManager(), child, child->getName() + "_gizmo", "ParticleSystemBillboard");
	}
}

void MainWindow::OnGizmoSelect(Gizmo2::SelectedObjects nodes, void* userData)
{
	if (userData == nullptr)
	{
		std::vector<string> names;

		for (auto it = nodes.begin(); it != nodes.end(); ++it)
			names.push_back((*it)->getName());

		TreeView* hierarchyTree = hierarchyWindow->getTreeView();
		TreeView* assetsTree = assetsWindow->getTreeView();

		bool selectAfter = false;
		hierarchyTree->selectNodes(names, true, &selectAfter);
		assetsTree->selectNodes({  });

		if (nodes.size() == 1)
		{
			TreeNode* node = hierarchyTree->getNodeByName(names[0], hierarchyTree->getRootNode());
			hierarchyTree->focusOnNode(node);
		}
	}

	if (nodes.size() > 0)
	{
		ObjectEditor2* editor = nullptr;

		if (CheckObjectsType(EmptyObjectFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new ObjectEditor2();
		}
		if (CheckObjectsType(EntityFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new EntityEditor();
		}
		if (CheckObjectsType(LightFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new LightEditor2();
		}
		if (CheckObjectsType("Camera", nodes))
		{
			editor = new CameraEditor2();
		}
		if (CheckObjectsType(ParticleSystemFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new ParticleSystemEditor2();
		}
		if (CheckObjectsType(UICanvasFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new UICanvasEditor2();
		}
		if (CheckObjectsType(UIButtonFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new UIButtonEditor2();
		}
		if (CheckObjectsType(UITextFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new UITextEditor2();
		}
		if (CheckObjectsType(UIImageFactory::FACTORY_TYPE_NAME, nodes))
		{
			editor = new UIImageEditor2();
		}

		if (editor == nullptr)
		{
			editor = new ObjectEditor2();
		}

		editor->init(nodes);

		inspectorWindow->setEditor(editor);
	}
	else
	{
		inspectorWindow->setEditor(nullptr);
	}
}

void MainWindow::OnGizmoAction(Gizmo2::SelectedObjects nodes, Gizmo2::GizmoAction action)
{
}

void MainWindow::OnGizmoManipulate(Gizmo2::SelectedObjects nodes)
{
	inspectorWindow->updateObjectEditorTransform();
}

void MainWindow::OnGizmoManipulateStart(Gizmo2::SelectedObjects nodes)
{
	std::vector<std::pair<SceneNode*, Affine3>> * nodeTransforms = new std::vector<std::pair<SceneNode*, Affine3>>();

	for (auto it = nodes.begin(); it != nodes.end(); ++it)
	{
		nodeTransforms->push_back(make_pair(*it, (*it)->_getFullTransform()));
	}

	Undo::Push(
		Undo::Command([=](Undo::Command * command)
		{
			void* data = command->GetData();
			std::vector<std::pair<SceneNode*, Affine3>> * _data = reinterpret_cast<std::vector<std::pair<SceneNode*, Affine3>>*>(data);

			for (auto it = _data->begin(); it != _data->end(); ++it)
			{
				SceneNode* node = it->first;
				Vector3 pos;
				Vector3 scale;
				Quaternion rot;
				it->second.decomposition(pos, scale, rot);

				node->_setDerivedPosition(pos);
				node->_setDerivedOrientation(rot);
				node->setScale(scale);
			}

			AddOnEndUpdateCallback([=]() {
				gizmo2->updatePosition();
				gizmo2->callSelectCallback();
			});

		}, reinterpret_cast<void*>(nodeTransforms)),
		Undo::Command([=](Undo::Command* command)
		{

		}, nullptr)
	);
}

void MainWindow::OnGizmoManipulateEnd(Gizmo2::SelectedObjects nodes)
{
	std::vector<std::pair<SceneNode*, Affine3>>* nodeTransforms = new std::vector<std::pair<SceneNode*, Affine3>>();

	for (auto it = nodes.begin(); it != nodes.end(); ++it)
	{
		nodeTransforms->push_back(make_pair(*it, (*it)->_getFullTransform()));
	}

	Undo::SetCurrentActionRedo(
		Undo::Command([=](Undo::Command* command)
		{
			void* data = command->GetData();
			std::vector<std::pair<SceneNode*, Affine3>> * _data = reinterpret_cast<std::vector<std::pair<SceneNode*, Affine3>>*>(data);

			for (auto it = _data->begin(); it != _data->end(); ++it)
			{
				SceneNode* node = it->first;
				Vector3 pos;
				Vector3 scale;
				Quaternion rot;
				it->second.decomposition(pos, scale, rot);

				node->_setDerivedPosition(pos);
				node->_setDerivedOrientation(rot);
				node->setScale(scale);
			}

			AddOnEndUpdateCallback([=]() {
				gizmo2->updatePosition();
				gizmo2->callSelectCallback();
			});

		}, reinterpret_cast<void*>(nodeTransforms))
	);
}

Ogre::Vector2 MainWindow::GetScreenPos(float x, float y)
{
	float xx = sceneWindow->getPosition().x;
	float yy = sceneWindow->getPosition().y;

	return Ogre::Vector2(x - xx, y - yy);
}

void MainWindow::ShaderLog(HighLevelGpuProgramPtr prog)
{
	string logText = prog->getCompileLog();

	if (!logText.empty())
	{
		std::vector<std::string> results;
		boost::split(results, logText, [](char c) {return c == '\n'; });

		for (auto it = results.begin(); it != results.end(); ++it)
		{
			string _log = *it;

			if (!_log.empty() && _log != "\n")
			{
				if (_log.find("warning") != string::npos)
					consoleWindow->log(prog->getName() + ": " + _log, LMT_WARNING);
				else if (_log.find("error") != string::npos)
					consoleWindow->log(prog->getName() + ": " + _log, LMT_ERROR);
				else
					consoleWindow->log(prog->getName() + ": " + _log, LMT_INFO);
			}
		}
	}
}

void MainWindow::OnSceneNew()
{
	lastScenePath = "";
	openedSceneName = "";
	openedScenePath = "";

	string title = "Falco Engine [OpenGL]";
	SDL_SetWindowTitle(window, title.c_str());

	gizmo2->selectObject(nullptr, false);
	hierarchyWindow->getTreeView()->clear();

	previewWindow->clearPreview();

	//Clear current scene
	Engine::ClearScene(GetEngine->GetSceneManager());

	GetEngine->GetTerrainManager()->InitPagedGeometry();
	GetEngine->GetTerrainManager()->SetPagedGeometryCamera(mainCamera);

	LoadSettings();
}

void MainWindow::OnSceneOpen()
{
	CFileDialog fileDialog(TRUE, _T(".scene"), NULL, OFN_HIDEREADONLY, _T("Falco Engine scene (*.scene)|*.scene"));
	int result = fileDialog.DoModal();
	if (result == IDOK)
	{
		string fn = CSTR2STRW(fileDialog.GetPathName());
		fn = IO::ReplaceBackSlashes(fn);

		if (fn.find(projectAssetsDir) == string::npos)
		{
			MessageBoxA(0, "Select location inside project assets folder!", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		OpenScene(fn);
	}
}

void MainWindow::OnSceneSave()
{
	if (lastScenePath.empty())
	{
		CFileDialog fileDialog(FALSE, _T(".scene"), NULL, OFN_HIDEREADONLY, _T("Falco Engine scene (*.scene)|*.scene"));
		int result = fileDialog.DoModal();
		if (result == IDOK)
		{
			string fn = CSTR2STRW(fileDialog.GetPathName());
			fn = IO::ReplaceBackSlashes(fn);

			if (fn.find(projectAssetsDir) == string::npos)
			{
				MessageBoxA(0, "Select location inside project assets folder!", "Error", MB_OK | MB_ICONERROR);
				return;
			}

			lastScenePath = fn;
			openedSceneName = CSTR2STRW(fileDialog.GetFileTitle());
			openedScenePath = lastScenePath;

			GetEngine->loadedScene = openedScenePath;
			GetEngine->GetNavigationManager()->SetLoadedScene(openedScenePath);

			string title = "Falco Engine - " + openedSceneName + " [OpenGL]";
			SDL_SetWindowTitle(window, title.c_str());

			SerializeScene(lastScenePath);
			SaveLastScene();
		}
	}
	else
	{
		SerializeScene(lastScenePath);
		SaveLastScene();
	}
}

void MainWindow::OnSceneSaveAs()
{
	CFileDialog fileDialog(FALSE, _T(".scene"), NULL, OFN_HIDEREADONLY, _T("Falco Engine scene (*.scene)|*.scene"));
	int result = fileDialog.DoModal();
	if (result == IDOK)
	{
		string fn = CSTR2STRW(fileDialog.GetPathName());
		fn = IO::ReplaceBackSlashes(fn);

		if (fn.find(projectAssetsDir) == string::npos)
		{
			MessageBoxA(0, "Select location inside project assets folder!", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		lastScenePath = fn;
		openedSceneName = CSTR2STRW(fileDialog.GetFileTitle());
		openedScenePath = lastScenePath;

		GetEngine->loadedScene = openedScenePath;
		GetEngine->GetNavigationManager()->SetLoadedScene(openedScenePath);

		string title = "Falco Engine - " + openedSceneName + " [OpenGL]";
		SDL_SetWindowTitle(window, title.c_str());

		SerializeScene(lastScenePath);
		SaveLastScene();
	}
}

void MainWindow::SaveLastScene()
{
	string path = IO::RemovePart(lastScenePath, projectAssetsDir);

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	settings.LastOpenedScene = path;
	settings.Save(projectSettingsDir + "Editor.settings");
}

void MainWindow::LoadLastScene()
{
	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	if (!settings.LastOpenedScene.empty())
	{
		if (IO::FileExists(projectAssetsDir + settings.LastOpenedScene))
			OpenSceneInternal(projectAssetsDir + settings.LastOpenedScene);
		else
			LoadSettings();
	}
	else
		LoadSettings();
}

void MainWindow::OpenScene(string path)
{
	openScenePath = path;
}

void MainWindow::OpenSceneInternal(string path)
{
	previewWindow->clearPreview();
	ObjectEditor2::resetBufferObjects();

	TerrainEditor::SetTerrainMode(false);
	hierarchyWindow->getTreeView()->clear();

	string name = IO::GetFileNameWithExt(path);

	openedSceneName = name.c_str();
	openedScenePath = path;
	lastScenePath = path;

	string title = "Falco Engine - " + name + " [OpenGL]";

	SDL_SetWindowTitle(window, title.c_str());

	gizmo2->selectObject(nullptr, false);

	//Deserialize scene
	SceneSerializer* ser = new SceneSerializer();
	ser->Deserialize(GetEngine->GetSceneManager(), path);
	delete ser;

	/* LOAD SCENE */
	LoadSettings();

	//Setup hierarhy
	SceneNode* root = GetEngine->GetSceneManager()->getRootSceneNode();
	ProcessSceneNode(root);

	GetEngine->GetTerrainManager()->SetPagedGeometryCamera(mainCamera);

	//Settings
	//EditorSettings settings;
	//settings.Load(projectSettingsDir + "Editor.settings");

	SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");

	if (navMesh != nullptr)
		navMesh->setVisible(navMeshVisible);

	SaveLastScene();

	AddOnEndUpdateCallback([=]() {
		DeferredLightRenderOperation::UpdateStaticShadowmaps();
	});
}

void MainWindow::OnProjectOpen()
{
	string path = Helper::ExeName();

	::ShellExecute(NULL, _T("open"), CString(path.c_str()), NULL, NULL, SW_RESTORE);

	done = true;
}

void MainWindow::OnProjectSettings()
{
	ProjectSettingsEditor2* editor = new ProjectSettingsEditor2();
	editor->init(projectSettingsDir + "Main.settings");
	inspectorWindow->setEditor(editor);
}

void MainWindow::OnProjectBuildWin64()
{
	CFolderPickerDialog folderPickerDialog(_T(""), OFN_FILEMUSTEXIST | OFN_ENABLESIZING, NULL, sizeof(OPENFILENAME));

	consoleWindow->clear();

	if (folderPickerDialog.DoModal() == IDOK)
	{
		string folderPath = CSTR2STRW(folderPickerDialog.GetFolderPath());
		folderPath = IO::ReplaceBackSlashes(folderPath);
		folderPath = folderPath + "/";

		if (IO::DirExists(folderPath))
		{
			AddOnEndUpdateCallback([=]() {
				ProjectExporter::ExportWinX64(projectName, folderPath);
			});
		}
	}
}

void MainWindow::OnProjectBuildWebGL()
{
	CFolderPickerDialog folderPickerDialog(_T(""), OFN_FILEMUSTEXIST | OFN_ENABLESIZING, NULL, sizeof(OPENFILENAME));

	consoleWindow->clear();

	if (folderPickerDialog.DoModal() == IDOK)
	{
		string folderPath = CSTR2STRW(folderPickerDialog.GetFolderPath());
		folderPath = IO::ReplaceBackSlashes(folderPath);
		folderPath = folderPath + "/";

		if (IO::DirExists(folderPath))
		{
			AddOnEndUpdateCallback([=]() {
				ProjectExporter::ExportWebGL(projectName, folderPath);
			});
		}
	}
}

void MainWindow::OnExit()
{
	done = true;
}

void MainWindow::OnUndo()
{
	Undo::DoUndo();
}

void MainWindow::OnRedo()
{
	Undo::DoRedo();
}

void MainWindow::OnDuplicate()
{
	TreeView* treeView = hierarchyWindow->getTreeView();
	auto selectedNodes = treeView->getSelectedNodes();
	std::vector<SceneNode*> newObjects;

	if (selectedNodes.size() > 0)
	{
		for (auto it = selectedNodes.begin(); it != selectedNodes.end(); ++it)
		{
			if (GetEngine->GetSceneManager()->hasSceneNode(*it))
			{
				SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(*it);
				SceneNode* newNode = CloneTools::CloneSceneNode(node);

				if (newNode != nullptr)
				{
					//ProcessSceneNode(newNode, true);
					newObjects.push_back(newNode);
				}
			}
		}

		AddOnEndUpdateCallback([=]() {
			for (auto it = newObjects.begin(); it != newObjects.end(); ++it)
				ProcessSceneNode(*it, true);

			gizmo2->selectObjects(newObjects);
			(const_cast<std::vector<SceneNode*>*>(&newObjects))->clear();
		});
	}
}

void MainWindow::OnAlignWithView()
{
	if (gizmo2->getSelectedObjects().size() == 0)
		return;

	SceneNode* node = (SceneNode*)gizmo2->getSelectedObjects()[0];

	if (node != NULL)
	{
		Vector3 pos = mainCamera->getParentSceneNode()->_getDerivedPosition();
		Quaternion rot = mainCamera->getParentSceneNode()->_getDerivedOrientation();

		node->_setDerivedPosition(pos);
		node->_setDerivedOrientation(rot);

		//gizmo2->selectObjects(gizmo2->getSelectedObjects());

		inspectorWindow->updateCurrentEditor();
	}
}

bool BitmapToClipboard(HBITMAP hBM, HWND hWnd)
{
	if (!::OpenClipboard(hWnd))
		return false;
	::EmptyClipboard();

	BITMAP bm;
	::GetObject(hBM, sizeof(bm), &bm);

	BITMAPINFOHEADER bi;
	::ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = bm.bmBitsPixel;
	bi.biCompression = BI_RGB;
	if (bi.biBitCount <= 1)	// make sure bits per pixel is valid
		bi.biBitCount = 1;
	else if (bi.biBitCount <= 4)
		bi.biBitCount = 4;
	else if (bi.biBitCount <= 8)
		bi.biBitCount = 8;
	else // if greater than 8-bit, force to 24-bit
		bi.biBitCount = 24;

	// Get size of color table.
	SIZE_T dwColTableLen = (bi.biBitCount <= 8) ? (1 << bi.biBitCount) * sizeof(RGBQUAD) : 0;

	// Create a device context with palette
	HDC hDC = ::GetDC(NULL);
	HPALETTE hPal = static_cast<HPALETTE>(::GetStockObject(DEFAULT_PALETTE));
	HPALETTE hOldPal = ::SelectPalette(hDC, hPal, FALSE);
	::RealizePalette(hDC);

	// Use GetDIBits to calculate the image size.
	::GetDIBits(hDC, hBM, 0, static_cast<UINT>(bi.biHeight), NULL,
		reinterpret_cast<LPBITMAPINFO>(&bi), DIB_RGB_COLORS);
	// If the driver did not fill in the biSizeImage field, then compute it.
	// Each scan line of the image is aligned on a DWORD (32bit) boundary.
	if (0 == bi.biSizeImage)
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) * bi.biHeight;

	// Allocate memory
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + dwColTableLen + bi.biSizeImage);
	if (hDIB)
	{
		union tagHdr_u
		{
			LPVOID             p;
			LPBYTE             pByte;
			LPBITMAPINFOHEADER pHdr;
			LPBITMAPINFO       pInfo;
		} Hdr;

		Hdr.p = ::GlobalLock(hDIB);
		// Copy the header
		::CopyMemory(Hdr.p, &bi, sizeof(BITMAPINFOHEADER));
		// Convert/copy the image bits and create the color table
		int nConv = ::GetDIBits(hDC, hBM, 0, static_cast<UINT>(bi.biHeight),
			Hdr.pByte + sizeof(BITMAPINFOHEADER) + dwColTableLen,
			Hdr.pInfo, DIB_RGB_COLORS);
		::GlobalUnlock(hDIB);
		if (!nConv)
		{
			::GlobalFree(hDIB);
			hDIB = NULL;
		}
	}
	if (hDIB)
		::SetClipboardData(CF_DIB, hDIB);
	::CloseClipboard();
	::SelectPalette(hDC, hOldPal, FALSE);
	::ReleaseDC(NULL, hDC);
	return NULL != hDIB;
}

HBITMAP TextureToBitmap(Image img)
{
	Gdiplus::Bitmap bitmap(img.getWidth(), img.getHeight(), PixelFormat24bppRGB);

	for (int i = 0; i < img.getWidth(); ++i)
	{
		for (int j = 0; j < img.getHeight(); ++j)
		{
			ColourValue color = img.getColourAt(i, j, 0); 

// Falco :Bug #91
			int nR = color.r * 255.0f;
			int nG = color.g * 255.0f;
			int nB = color.b * 255.0f;

			if (nR > 255) { nR = 255; }
			if (nG > 255) { nG = 255; }
			if (nB > 255) { nB = 255; }

			bitmap.SetPixel(i, j, Gdiplus::Color(nR, nG, nB));
		}
	}

	HBITMAP bmp;
	bitmap.GetHBITMAP(Gdiplus::Color::Transparent, &bmp);
	img.freeMemory();

	return bmp;
}

void MainWindow::OnCopyRenderImage()
{
	Image img;
	sceneRenderTarget->convertToImage(img);
	
	HBITMAP bmp = TextureToBitmap(img);
	
	BitmapToClipboard(bmp, GetWindowHandle());
	DeleteObject(bmp);
	img.freeMemory();

	Toast::showMessage("Render image is copied into the clipboard", ToastIcon::TI_IMAGE);
}

void MainWindow::OnCreateEmpty()
{
	SceneNode* node = EditorClasses::CreateEmptyObject(GetEngine->GetSceneManager());

	hierarchyWindow->addNode(node);
	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateCamera()
{
	SceneNode* node = EditorClasses::CreateCamera(GetEngine->GetSceneManager());

	hierarchyWindow->addNode(node);
	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateTerrain()
{
	CFileDialog fileDialog(FALSE, _T(".terrain"), NULL, OFN_HIDEREADONLY, _T("Falco Engine terrain (*.terrain)|*.terrain"));
	int result = fileDialog.DoModal();
	if (result == IDOK)
	{
		std::string filename = CSTR2STRW(fileDialog.GetPathName());

		string fn = IO::ReplaceBackSlashes(filename);

		if (fn.find(projectAssetsDir) == string::npos)
		{
			MessageBoxA(0, "Select location inside project assets folder!", "Error", MB_OK | MB_ICONERROR);
			return;
		}

		GetEngine->GetTerrainManager()->CreateTerrain(0, 0, filename);
	}
}

void MainWindow::OnCreatePointLight()
{
	SceneNode* node = EditorClasses::CreateLight(GetEngine->GetSceneManager(), Light::LightTypes::LT_POINT);

	hierarchyWindow->addNode(node);
	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateSpotLight()
{
	SceneNode* node = EditorClasses::CreateLight(GetEngine->GetSceneManager(), Light::LightTypes::LT_SPOTLIGHT);

	hierarchyWindow->addNode(node);
	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateDirectionalLight()
{
	SceneNode* node = EditorClasses::CreateLight(GetEngine->GetSceneManager(), Light::LightTypes::LT_DIRECTIONAL);

	hierarchyWindow->addNode(node);
	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateRagdoll()
{
	ragdollEditor->show();
}

void MainWindow::OnCreateParticleSystem()
{
	SceneNode* node = EditorClasses::CreateParticleSystem(GetEngine->GetSceneManager());

	hierarchyWindow->addNode(node);
	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateCube()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Cube.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedOrientation(Quaternion::IDENTITY);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateSphere()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Sphere.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedOrientation(Quaternion::IDENTITY);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreatePlane()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Plane.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateCapsule()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Capsule.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateCylinder()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Cylinder.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateCone()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Cone.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreatePyramid()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Pyramid.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateTorus()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Torus.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateTube()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Tube.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateTeapot()
{
	SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), "Primitives/Teapot.fbx", Vector2(), 30);
	Vector3 pos = node->_getDerivedPosition();

	SceneNode* obj = (SceneNode*)node->getChild(0);
	node->removeChild(obj);
	Engine::Destroy(node);
	GetEngine->GetSceneManager()->getRootSceneNode()->addChild(obj);
	node = obj;
	node->setScale(1, 1, 1);
	node->_setDerivedPosition(pos);

	MaterialPtr baseWhite = MaterialManager::getSingleton().getByName("BaseWhite", "Assets");
	((Entity*)node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(baseWhite);

	if (node != nullptr)
	{
		ProcessSceneNode(node, true);

		AddOnEndUpdateCallback([=]() {
			gizmo2->selectObject(node, false);
		});
	}
}

void MainWindow::OnCreateCanvas()
{
	SceneManager* mgr = GetEngine->GetSceneManager();
	UICanvas* canvas = UICanvasFactory::uiCanvas[mgr];
	SceneNode* node = EditorClasses::CreateUICanvas(mgr);

	if (canvas == nullptr)
		hierarchyWindow->addNode(node);

	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateImage()
{
	SceneNode* node = EditorClasses::CreateUIImage(GetEngine->GetSceneManager());

	if (node->getParentSceneNode()->getName() != "Ogre/SceneRoot")
		hierarchyWindow->addNode(node, node->getParentSceneNode()->getName());
	else
		hierarchyWindow->addNode(node);

	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateText()
{
	SceneNode* node = EditorClasses::CreateUIText(GetEngine->GetSceneManager());

	if (node->getParentSceneNode()->getName() != "Ogre/SceneRoot")
		hierarchyWindow->addNode(node, node->getParentSceneNode()->getName());
	else
		hierarchyWindow->addNode(node);

	gizmo2->selectObject(node, false);
}

void MainWindow::OnCreateButton()
{
	SceneNode* node = EditorClasses::CreateUIButton(GetEngine->GetSceneManager());

	if (node->getParentSceneNode()->getName() != "Ogre/SceneRoot")
		hierarchyWindow->addNode(node, node->getParentSceneNode()->getName());
	else
		hierarchyWindow->addNode(node);

	gizmo2->selectObject(node, false);
}

void MainWindow::OnAddAudioSource()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			AudioSource* audio = new AudioSource(node);
			node->components.push_back(audio);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddAudioListener()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			AudioListener* audio = new AudioListener(node);
			node->components.push_back(audio);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddRigidbody()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			RigidBody* audio = new RigidBody(node);
			node->components.push_back(audio);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddBoxCollider()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			BoxCollider* comp = new BoxCollider(node);
			node->components.push_back(comp);

			if (node->getAttachedObjects().size() > 0)
			{
				MovableObject* obj = node->getAttachedObject(0);
				if (obj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					AxisAlignedBox aab = obj->getBoundingBox();

					Vector3 size = aab.getHalfSize();
					Vector3 center = aab.getCenter();

					comp->SetBoxSize(size);
					comp->SetOffset(center);
				}
			}
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddSphereCollider()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			SphereCollider* comp = new SphereCollider(node);
			node->components.push_back(comp);

			if (node->getAttachedObjects().size() > 0)
			{
				MovableObject* obj = node->getAttachedObject(0);
				if (obj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					AxisAlignedBox aab = obj->getBoundingBox();
					float radius = obj->getBoundingRadius() * 0.5f;

					Vector3 center = aab.getCenter();

					comp->SetRadius(radius);
					comp->SetOffset(center);
				}
			}
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddCapsuleCollider()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			CapsuleCollider* comp = new CapsuleCollider(node);
			node->components.push_back(comp);

			if (node->getAttachedObjects().size() > 0)
			{
				MovableObject* obj = node->getAttachedObject(0);
				if (obj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					AxisAlignedBox aab = obj->getBoundingBox();
					float radius = obj->getBoundingRadius() * 0.5f;

					Vector3 size = aab.getSize();
					Vector3 center = aab.getCenter();

					comp->SetRadius(radius);
					comp->SetHeight(size.y);
					comp->SetOffset(center);
				}
			}
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddMeshCollider()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			MeshCollider* audio = new MeshCollider(node);
			node->components.push_back(audio);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddFixedJoint()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			FixedJoint* fixedJoint = new FixedJoint(node);
			node->components.push_back(fixedJoint);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddHingeJoint()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			HingeJoint* hingeJoint = new HingeJoint(node);
			node->components.push_back(hingeJoint);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddCharacterJoint()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			CharacterJoint* audio = new CharacterJoint(node);
			node->components.push_back(audio);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddNavMeshAgent()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			NavMeshAgent* audio = new NavMeshAgent(node);
			node->components.push_back(audio);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddNavMeshObstacle()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			NavMeshObstacle* comp = new NavMeshObstacle(node);
			node->components.push_back(comp);

			if (node->getAttachedObjects().size() > 0)
			{
				MovableObject* obj = node->getAttachedObject(0);
				if (obj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					AxisAlignedBox aab = obj->getBoundingBox();

					Vector3 size = aab.getHalfSize();
					Vector3 center = aab.getCenter();

					comp->setSize(size);
					comp->setOffset(center);
				}
			}

			comp->SceneLoaded();
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);

		GetEngine->GetNavigationManager()->setNavMeshIsDirty();
	}
}

void MainWindow::OnAddAnimationList()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* _node = *it;

			std::vector<SceneNode*> children;
			GetEngine->GetAllChildren(_node, children);

			for (auto _it = children.begin(); _it != children.end(); ++_it)
			{
				SceneNode* node = *_it;

				if (node->getAttachedObjects().size() > 0)
				{
					MovableObject* obj = node->getAttachedObject(0);

					if (obj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
					{
						Entity* ent = (Entity*)obj;

						if (ent->getMesh() != nullptr)
						{
							if (ent->getMesh()->isFbx)
							{
								AnimationList* animList = new AnimationList(_node);
								animList->SetFBXFileName(ent->getMesh()->getOrigin());
								animList->AddAnimationData("Default", ent->getMesh()->getOrigin(), 0, 25);

								_node->components.push_back(animList);
								break;
							}
						}
					}
				}
			}

			children.clear();
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnAddVehicle()
{
	auto sceneNodes = gizmo2->getSelectedObjects();

	if (sceneNodes.size() > 0)
	{
		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			SceneNode* node = *it;

			Vehicle* component = new Vehicle(node);
			node->components.push_back(component);
		}

		inspectorWindow->updateCurrentEditor();
		gizmo2->showWireframe(true);
	}
}

void MainWindow::OnExportPackage()
{
	assetExporter->show(DialogAssetExporter::Mode::Export);
}

void MainWindow::OnImportPackage()
{
	assetExporter->show(DialogAssetExporter::Mode::Import);
}

void MainWindow::OnShowGrid()
{
	if (gridVisible) {
		lines->setVisible(false);
		lines_2->setVisible(false);
		gridVisible = false;
	}
	else {
		lines->setVisible(true);
		lines_2->setVisible(true);
		gridVisible = true;
	}

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	settings.showGrid = gridVisible;
	settings.Save(projectSettingsDir + "Editor.settings");
}

void MainWindow::OnShowNavMesh()
{
	SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");

	if (navMesh != nullptr)
	{
		navMeshVisible = !navMeshVisible;
		navMesh->setVisible(navMeshVisible);

		if (navMeshVisible)
			GetEngine->GetNavigationManager()->setNavMeshIsDirty();

		EditorSettings settings;
		settings.Load(projectSettingsDir + "Editor.settings");
		settings.showNavMesh = navMeshVisible;
		settings.Save(projectSettingsDir + "Editor.settings");
	}
}

void MainWindow::OnEnableShadows()
{
	shadowsEnabled = !shadowsEnabled;

	SceneManager::shadowsEnabled = shadowsEnabled;

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	settings.shadowsEnabled = shadowsEnabled;
	settings.Save(projectSettingsDir + "Editor.settings");

	AddOnEndUpdateCallback([=]() {
		DeferredLightRenderOperation::UpdateStaticShadowmaps();
	});
}

void MainWindow::OnAbout()
{
	aboutDialog->show();
}

void MainWindow::OnGizmoSelect()
{
	gizmo2->setGizmoType(Gizmo2::GizmoType::GT_SELECT);
}

void MainWindow::OnGizmoMove()
{
	gizmo2->setGizmoType(Gizmo2::GizmoType::GT_MOVE);
}

void MainWindow::OnGizmoRotate()
{
	gizmo2->setGizmoType(Gizmo2::GizmoType::GT_ROTATE);
}

void MainWindow::OnGizmoScale()
{
	gizmo2->setGizmoType(Gizmo2::GizmoType::GT_SCALE);
}

void MainWindow::OnGizmoLocal()
{
	gizmo2->setTransformSpace(Gizmo2::TransformSpace::TS_LOCAL);
}

void MainWindow::OnGizmoWorld()
{
	gizmo2->setTransformSpace(Gizmo2::TransformSpace::TS_WORLD);
}

void MainWindow::OnGizmoSnapToGrid()
{
	gizmo2->setSnapToGrid(!gizmo2->getSnapToGrid());

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	settings.snapToGrid = gizmo2->getSnapToGrid();
	settings.Save(projectSettingsDir + "Editor.settings");
}

void MainWindow::OnGizmoCenter()
{
	gizmo2->setCenterBase(Gizmo2::CenterBase::CB_CENTER);

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	settings.gizmoCenterBase = 0;
	settings.Save(projectSettingsDir + "Editor.settings");
}

void MainWindow::OnGizmoPivot()
{
	gizmo2->setCenterBase(Gizmo2::CenterBase::CB_PIVOT);

	EditorSettings settings;
	settings.Load(projectSettingsDir + "Editor.settings");
	settings.gizmoCenterBase = 1;
	settings.Save(projectSettingsDir + "Editor.settings");
}

void MainWindow::OnPlay()
{
	AddOnEndUpdateCallback([=]() {
		if (consoleWindow->getClearOnPlay())
		{
			consoleWindow->clear();
		}

		string cmd = "\"" + projectName + ".exe\" \"" + projectDir + "\" \"" + projectAssemblyDir + "\"" + "\"" + openedScenePath + "\" useUnpackedResources " +
			to_string(runScreenWidth) + " " + to_string(runScreenHeight) + " " + (runFullscreen ? "true" : "false");

		cmd = Helper::ExePath() + "Player.exe " + cmd;

		using namespace boost::process;

		ipstream pipe_stream;
		child c(cmd, std_out > pipe_stream);

		std::string line;

		consoleWindow->setScrollToBottom(true);

		while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
		{
			consoleWindow->log(CP_UNI(line));
			Root::getSingleton().renderOneFrame();
		}

		c.wait();

		consoleWindow->setScrollToBottom(false);
	});
}
