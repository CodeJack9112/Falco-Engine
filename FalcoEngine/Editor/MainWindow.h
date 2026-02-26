#pragma once

#include <OgreVector2.h>
#include <OgreRenderTargetListener.h>

#include "../Ogre/source/build/Dependencies/include/SDL2/SDL.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_video.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_syswm.h"
#include "../Ogre/source/build/Dependencies/include/SDL2/SDL_opengl.h"

//#include "../Engine/Gizmo.h"
#include "../Engine/Gizmo2.h"

class DynamicLines;
class HierarchyWindow;
class AssetsWindow;
class InspectorWindow;
class SceneWindow;
class ConsoleWindow;
class LightingWindow;
class NavigationWindow;
class TerrainWindow;
class PreviewWindow;
class DialogRagdollEditor;
class DialogAbout;
class DialogAssetExporter;

class MainWindow : public RenderTargetListener
{
public:
	MainWindow();
	~MainWindow();

	void Run();

	static string GetProjectDir() { return projectDir; }
	static string GetProjectAssetsDir() { return projectAssetsDir; }
	static string GetProjectSolutionDir() { return projectSolutionDir; }
	static string GetProjectMonoDir() { return monoDir; }
	static string GetProjectLibraryDir() { return projectLibraryDir; }
	static string GetProjectSettingsDir() { return projectSettingsDir; }
	static string GetProjectTempDir() { return projectTempDir; }
	static string GetBuiltinResourcesDir() { return builtinResourcesPath; }
	static string GetLoadedScenePath() { return lastScenePath; }
	static void SetProjectDir(string dir) { projectDir = dir; }

	static string GetProjectName() { return projectName; }
	static void SetProjectName(string name) { projectName = name; }
	static HWND GetWindowHandle();

	//static Gizmo* gizmo;
	static Gizmo2* gizmo2;
	static Ogre::Camera* mainCamera;
	static Camera* guiCamera;
	static Ogre::TexturePtr sceneRenderTarget;

	void ProcessSceneNode(SceneNode* root, bool addRoot = false, bool addNodes = true, bool processBillboards = true);
	static void ShaderLog(HighLevelGpuProgramPtr prog);

public:
	static ImFont* editorFont;
	static ImFont* editorSmallFont;

	static Ogre::Vector2 GetWindowSize();
	static std::vector<string> GetImagesFileFormats() { return imagesFileFormats; }

	static InspectorWindow* GetInspectorWindow() { return inspectorWindow; }
	static AssetsWindow* GetAssetsWindow() { return assetsWindow; }
	static HierarchyWindow* GetHierarchyWindow() { return hierarchyWindow; }
	static ConsoleWindow* GetConsoleWindow() { return consoleWindow; }
	static LightingWindow* GetLightingWindow() { return lightingWindow; }
	static NavigationWindow* GetNavigationWindow() { return navigationWindow; }
	static TerrainWindow* GetTerrainWindow() { return terrainWindow; }
	static PreviewWindow* GetPreviewWindow() { return previewWindow; }
	static SceneWindow* GetSceneWindow() { return sceneWindow; }
	static void UpdateViewport();

	static void CreateObjectMenu();
	static MainWindow* Get() { return self; }

	void setScriptsChanged(bool val) { scriptsChanged = val; }
	bool getScriptsChanged() { return scriptsChanged; }
	void addChangedAsset(std::string path);
	std::vector<std::string>& getChangedAssets() { return changedAssets; }

	static void AddOnEndUpdateCallback(std::function<void()> callback) { onEndUpdateCallbacks.push_back(callback); }
	
private:
	/* CHILD WINDOWS */
	static HierarchyWindow * hierarchyWindow;
	static AssetsWindow * assetsWindow;
	static InspectorWindow * inspectorWindow;
	static SceneWindow* sceneWindow;
	static ConsoleWindow* consoleWindow;
	static NavigationWindow* navigationWindow;
	static LightingWindow* lightingWindow;
	static TerrainWindow* terrainWindow;
	static PreviewWindow* previewWindow;
	static DialogRagdollEditor* ragdollEditor;
	static DialogAbout* aboutDialog;
	static DialogAssetExporter* assetExporter;

	bool done = false;
	bool openScene = false;
	bool newScene = false;

	static std::vector<std::pair<int, int>> screenSizes;
	static int runScreenWidth;
	static int runScreenHeight;
	static bool runFullscreen;

public:
	bool navMeshVisible = true;
	bool gridVisible = true;
	bool shadowsEnabled = true;

	static std::vector<TexturePtr> icons;

	void CompileScripts();
	void WindowRestored();

private:
	bool lctrl = false;
	bool lshift = false;
	bool lmb = false;
	bool rmb = false;

	std::vector<std::string> changedAssets;
	bool scriptsChanged = false;

	void DeleteSelectedNodes();

	void ProcessBillboard(SceneNode* child, MovableObject* movObj);
	void ProcessSceneNodeInternal(SceneNode* root, bool addRoot = false, bool addNodes = true, bool processBillboards = true);
	static bool CheckObjectsType(std::string typeName, std::vector<SceneNode*>& nodes);

	virtual void preRenderTargetUpdate(const RenderTargetEvent& evt);
	virtual void postRenderTargetUpdate(const RenderTargetEvent& evt);

private:
	static string projectDir;
	static string projectName;
	static string projectSettingsDir;
	static string projectAssetsDir;
	static string projectTempDir;
	static string projectAssemblyDir;
	static string projectSolutionDir;
	static string projectLibraryDir;
	static string monoDir;
	static string builtinResourcesPath;

	static string openedSceneName;
	static string openedScenePath;
	static string lastScenePath;

	static MainWindow * self;
	static bool loaded;
	static int tabSet;

	SDL_Window* window = nullptr;
	Viewport* mainViewport = nullptr;

	DynamicLines* lines = nullptr;
	DynamicLines* lines_2 = nullptr;
	
	static std::vector<string> imagesFileFormats;

	std::string openScenePath = "";

	void InitEngine();
	void SetupViewport();
public:
	void SetupDeferredShading();
private:
	void CreateRenderTexture();
	void OpenSceneInternal(string path);
	void LoadLastScene();
	void SaveLastScene();
	void SerializeScene(string path);
	void LoadSettings();

public:
	void OpenScene(string path);

private:
	static void OnGUI();
	static void DrawMainMenu();
	static void DrawMainToolbar();

	static void OnGizmoSelect(Gizmo2::SelectedObjects nodes, void* userData);
	static void OnGizmoAction(Gizmo2::SelectedObjects nodes, Gizmo2::GizmoAction action);
	static void OnGizmoManipulate(Gizmo2::SelectedObjects nodes);
	static void OnGizmoManipulateStart(Gizmo2::SelectedObjects nodes);
	static void OnGizmoManipulateEnd(Gizmo2::SelectedObjects nodes);

	Ogre::Vector2 GetScreenPos(float x, float y);

	static std::vector<std::function<void()>> onEndUpdateCallbacks;

private:
	//File
	void OnSceneNew();
	void OnSceneOpen();
	void OnSceneSave();
	void OnSceneSaveAs();
	void OnProjectOpen();
	void OnProjectSettings();
	void OnProjectBuildWin64();
	void OnProjectBuildWebGL();
	void OnExit();

	//Edit
	void OnUndo();
	void OnRedo();
	void OnDuplicate();
	void OnAlignWithView();
	void OnCopyRenderImage();

	//Create object
	void OnCreateEmpty();
	void OnCreateCamera();
public:
	void OnCreateTerrain();

	//View
	void OnShowGrid();
	void OnShowNavMesh();
	void OnEnableShadows();

private:
	void OnCreatePointLight();
	void OnCreateSpotLight();
	void OnCreateDirectionalLight();
	void OnCreateRagdoll();
	void OnCreateParticleSystem();
	void OnCreateCube();
	void OnCreateSphere();
	void OnCreatePlane();
	void OnCreateCapsule();
	void OnCreateCylinder();
	void OnCreateTeapot();
	void OnCreateCone();
	void OnCreatePyramid();
	void OnCreateTorus();
	void OnCreateTube();
	void OnCreateCanvas();
	void OnCreateImage();
	void OnCreateText();
	void OnCreateButton();

	//Add component
	void OnAddAudioSource();
	void OnAddAudioListener();
	void OnAddRigidbody();
	void OnAddBoxCollider();
	void OnAddSphereCollider();
	void OnAddCapsuleCollider();
	void OnAddMeshCollider();
	void OnAddFixedJoint();
	void OnAddHingeJoint();
	void OnAddCharacterJoint();
	void OnAddNavMeshAgent();
	void OnAddNavMeshObstacle();
	void OnAddAnimationList();
	void OnAddVehicle();

	//Assets
	void OnExportPackage();
	void OnImportPackage();

	//Help
	void OnAbout();

	//Gizmo
	void OnGizmoSelect();
	void OnGizmoMove();
	void OnGizmoRotate();
	void OnGizmoScale();
	void OnGizmoLocal();
	void OnGizmoWorld();
	void OnGizmoSnapToGrid();
	void OnGizmoCenter();
	void OnGizmoPivot();

	//Play
	void OnPlay();
};