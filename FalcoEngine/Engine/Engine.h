#pragma once

#include "OgreIncludes.h"
#include <OgreFrameListener.h>
#include <OgreStaticGeometry.h>
#include "DynamicRenderable.h"
#include "Euler.h"
#include "Empty.h"

#include "UICanvas.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIImage.h"

#include "RaycastGeometry.h"
#include "AssetsTools.h"
#include "MonoRuntime.h"

//#include "Windows.h"
#include <string>
#include <vector>

#include <LibZip\zip.h>

#define ENGINE_VERSION string("1.0");

using namespace Ogre;

class SoundManager;
class PhysicsManager;
class InputManager;
class UIManager;
class FBXSceneManager;
class DeferredShadingSystem;
class NavigationManager;
class TerrainManager;
class AnimationList;
class ProjectSettings;

class Engine : public FrameListener
{
public:
	enum Environment { Editor, Player };

	Engine();
	~Engine(void);

	static Engine* GetInstance()
	{
		return &instance;
	}

	bool Init(Environment env, bool createWindow = false, string windowName = "Falco Engine game");
	RenderSystem* GetCurrentRenderSystem();
	void SetSceneManager(SceneManager * mgr);
	SceneManager * GetSceneManager();

	//Calling this function after resource initialization is neccessary to find and define shaders and materials
	void ResolveDependencies();
	void CreateCache();
	void CreateShaders(std::string fileName);

public:
	NameGenerator * nameGenerator;
	std::string GenerateNewName(SceneManager * manager, std::string prefix = "");

	virtual bool frameStarted(const FrameEvent &evt);
	virtual bool frameEnded(const FrameEvent &evt);

	void SetAssetsPath(std::string path);
	void SetMonoPath(std::string path);
	void SetRootPath(std::string path);
	void SetBuiltinResourcesPath(std::string path);
	void SetAssemblyPath(std::string path) { assemblyPath = path; }
	void SetLibraryPath(std::string path) { libraryPath = path; }
	void SetCachePath(std::string path) { cachePath = path; }

	void SetUseUnpackedResources(bool use) { useUnpackedResources = use; }
	void SetOpenedZipAssets(zip_t* zip) { zipArch1 = zip; }
	void SetOpenedZipBuiltin(zip_t* zip) { zipArch2 = zip; }

	std::string GetAssetsPath() { return assetsPath; }
	std::string GetMonoPath() { return monoPath; }
	std::string GetBuiltinResourcesPath() { return builtinResourcesPath; }
	std::string GetRootPath() { return rootPath; }
	std::string GetLibraryPath() { return libraryPath; }
	std::string GetCachePath() { return cachePath; }

	MonoRuntime * GetMonoRuntime() { return monoRuntime; }
	SoundManager * GetSoundManager() { return soundManager; }
	PhysicsManager * GetPhysicsManager() { return physicsManager; }
	InputManager * GetInputManager() { return inputManager; }
	UIManager * GetUIManager() { return uiManager; }
	FBXSceneManager * GetFBXSceneManager() { return fbxSceneManager; }
	DeferredShadingSystem * GetDeferredShadingSystem() { return deferredSystem; }
	NavigationManager * GetNavigationManager() { return navigationManager; }
	TerrainManager * GetTerrainManager() { return terrainManager; }
	zip_t* GetOpenedZipAssets() { return zipArch1; }
	zip_t* GetOpenedZipBuiltin() { return zipArch2; }
	bool GetUseUnpackedResources() { return useUnpackedResources; }

	RenderWindow * GetRenderWindow() { return renderWindow; }
	void SetRenderWindow(RenderWindow* window);
	Environment GetEnvironment() { return environment; }
	void SetMainCamera(Camera * camera);
	Camera * GetMainCamera() { return mainCamera; }
	void GetSceneNodeCount(SceneNode * root, int &start);
	void UpdateSceneNodeIndexes();
	bool IsObjectUI(MovableObject * obj);
	bool IsObjectUIElement(MovableObject * obj);
	bool IsInitialized() { return initialized; }

	void SetOnResourcesAddedCallback(std::function<void()>callback) { ResourcesAddedCallback = callback; }
	void SetOnAfterRootCreatedCallback(std::function<void()>callback) { AfterRootCreated.push_back(callback); }

	void UpdateMaterials();
	void UpdateMaterials(Entity* entity);
	void UpdateMaterials(StaticGeometry::Region* staticRegion);
	void UpdateShaders(MaterialPtr material);
	void UpdateShadersInPass(Pass* pass);
	void UpdateGpuProgramParams(Pass* pass);
	void UpdateShadowTextureUnits(Pass * pass);
	
	void ClearMaterialsInstances();
	void ClearMaterialInstance(Entity * entity);

	bool IsEditorObject(SceneNode* node);
	bool IsEditorMesh(MeshPtr mesh);
	bool IsEditorEntity(Entity* entity);
	void GetAllChildren(SceneNode * root, std::vector<SceneNode*> & outList);
	void RenderOneFrameWithoutGUI();

	StaticGeometry* getStaticGeometry() { return sg; }
	void buildStaticGeometry(std::string lightmapsPath, bool buildLightmaps = false, bool destroyEntities = true);

	float getTimeScale() { return timeScale; }
	void setTimeScale(float value) { if (value >= 0.0f) timeScale = value; else timeScale = 0.0f; }

	static ProjectSettings * getProjectSettings() { return projectSettings; }
	static void loadProjectSettings();
	static void saveProjectSettings();

private:
	enum AnimationTimePosition
	{
		ATP_Start,
		ATP_End
	};

	zip_t* zipArch1;
	zip_t* zipArch2;

	static ProjectSettings * projectSettings;

	static Engine instance;
	bool initialized = false;
	bool useUnpackedResources = true;
	Environment environment;
	Root* mRoot;
	SceneManager * mSceneManager;
	RenderSystem* mRenderSystem;
	DeferredShadingSystem * deferredSystem;

	EmptyObjectFactory * emptyObjectFactory;
	UICanvasFactory * uiCanvasFactory;
	UIButtonFactory * uiButtonFactory;
	UITextFactory * uiTextFactory;
	UIImageFactory * uiImageFactory;

	MonoRuntime * monoRuntime = nullptr;
	SoundManager * soundManager = nullptr;
	PhysicsManager * physicsManager = nullptr;
	InputManager * inputManager = nullptr;
	UIManager * uiManager = nullptr;
	FBXSceneManager * fbxSceneManager = nullptr;
	NavigationManager * navigationManager = nullptr;
	TerrainManager * terrainManager = nullptr;
	StaticGeometry* sg;

	RenderWindow * renderWindow;
	Camera * mainCamera = nullptr;
	float deltaTime = 0.0f;

	std::string assetsPath;
	std::string rootPath;
	std::string monoPath;
	std::string assemblyPath;
	std::string builtinResourcesPath;
	std::string libraryPath;
	std::string cachePath;

	bool updateFrame = true;
	bool drawGUI = true;

	static void DestroyAttachedObjects(SceneNode * node);
	void updateSceneNodeIndexes(SceneNode * root, int & start);

	std::function<void()>ResourcesAddedCallback = NULL;
	std::vector<std::function<void()>>AfterRootCreated;
	void(*ImGUICallback)() = nullptr;

	std::vector<SceneNode*> deleteArray;
	void DeleteAll(SceneNode * root);

	int defaultLightmapSize = 256;
	int regionLightmapSize = 1024;
	float lightmapBlurRadius = 0.5f;

	int systemCP = 0;
	float timeScale = 1.0f;

	static void SkeletalAnimationSetLoop(SceneNode * root, std::string animationName, std::string animfullName, AnimationList * animList, bool loop);
	static void SkeletalAnimationIsPlaying(SceneNode * root, std::string animationName, std::string animfullName, AnimationList * animList, bool & play);
	static void AnimationPlayInternal(SceneNode * node, AnimationList * animList, std::string animationName, bool play);
	static void SkeletalAnimationPlay(SceneNode* root, std::string animationName, std::string animfullName, AnimationList* animList, bool play);

	static void AnimationInternalUpdateRecursive(SceneNode * root, float deltaTime);
	static void SetSkeletalAnimationTimePosition(SceneNode* root, AnimationList* animList, std::string animationName, std::string animfullName, AnimationTimePosition position);

public:
	static void Destroy(SceneNode * node);
	static void ClearScene(SceneManager * manager);
	static void AnimationPlay(SceneNode * node, std::string animationName, bool play, float duration = 0.0f, unsigned int playMode = 2, unsigned int prevStatePosition = 2, unsigned int currentStatePosition = 2);
	static void AnimationPlay(AnimationList * animList, std::string animationName, bool play, float duration = 0.0f, unsigned int playMode = 2, unsigned int prevStatePosition = 2, unsigned int currentStatePosition = 2);
	static void AnimationSetLoop(SceneNode * node, std::string animationName, bool loop);
	static bool AnimationIsPlaying(SceneNode * node, std::string animationName);
	static bool AnimationIsLoop(SceneNode * node, std::string animationName);
	static void AnimationSetSpeed(SceneNode * node, std::string animationName, float speed);
	static float AnimationGetSpeed(SceneNode * node, std::string animationName);
	static void SetAnimationTimePosition(SceneNode* node, std::string animationName, AnimationTimePosition position);

	void LoadResources(bool useUnpackedResources = true);
	void LoadFBXFiles();
	float GetDeltaTime() { return deltaTime; }
	void SetUpdateOnFrame(bool update) { updateFrame = update; }
	bool GetUpdateOnFrame() { return updateFrame; }
	void SetDrawGUI(bool update) { drawGUI = update; }
	bool GetDrawGUI() { return drawGUI; }

	void SetDefaultLightmapSize(int size) { defaultLightmapSize = size; }
	int GetDefaultLightmapSize() { return defaultLightmapSize; }
	void SetLightmapBlurRadius(float radius) { lightmapBlurRadius = radius; }
	float GetLightmapBlurRadius() { return lightmapBlurRadius; }
	void SetRegionLightmapSize(int size) { regionLightmapSize = size; }
	int GetRegionLightmapSize() { return regionLightmapSize; }

	void SetSystemCP(int cp) { systemCP = cp; }
	int GetSystemCP() { return systemCP; }

	void SetImGUICallback(void(*callback)()) { ImGUICallback = callback; }

	std::string loadedScene = "";
	bool shouldExit = false;
};

#define GetEngine Engine::GetInstance()