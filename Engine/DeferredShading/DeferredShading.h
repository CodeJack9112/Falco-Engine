#ifndef H_DeferredShadingSystem
#define H_DeferredShadingSystem

#include "OgreCompositorInstance.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreMaterial.h"
#include "OgreRenderTargetListener.h"
#include "GeomUtils.h"

using namespace Ogre;

namespace Ogre
{
	class DefaultShadowCameraSetup;
}

class CSMGpuConstants;
class StableCSMShadowCameraSetup;

class MaterialSchemeHandler;
class DeferredLightCompositionPass;

class DeferredShadingSystem : public Ogre::RenderTargetListener
{
public:
    DeferredShadingSystem(Ogre::SceneManager *sm);
    ~DeferredShadingSystem();

    //The first render queue that does get rendered into the GBuffer
    //place objects (like skies) that should be before gbuffer before this one.
    static const Ogre::uint8 PRE_GBUFFER_RENDER_QUEUE;
    
    //The first render queue that does not get rendered into the GBuffer
    //place transparent (or other non gbuffer) objects after this one
    static const Ogre::uint8 POST_GBUFFER_RENDER_QUEUE;

	static const int maxCascades = 4;
	static CSMGpuConstants * mGpuConstants;

	static Ogre::DefaultShadowCameraSetup * DefaultCameraSetup;
	static StableCSMShadowCameraSetup * CSMCameraSetup;

    void initialize();
	void RecalculateShadowCascades();

	void addViewport(Ogre::Viewport* vp);
	void removeViewport(Ogre::Viewport* vp);
	void removeViewports();
	void setCamera(Ogre::Camera *cam) { mCamera = cam; }
	void resetLights();
	void Cleanup();
	void createResources();
	void setClearColor();
	void resetLightShaders();

	DeferredLightCompositionPass* getDeferredPass() { return deferredPass; }
    
protected:
	MaterialPtr lightAmbientMaterial;
	MaterialPtr lightGeometryMaterial;
	MaterialPtr lightShadowMaterial;
	MaterialPtr lightQuadMaterial;
	MaterialPtr lightQuadShadowMaterial;
	MaterialPtr quadMaterial;
	MaterialPtr shadowCasterMaterial;

	std::map<Viewport*, std::vector<CompositorInstance*>> mViewports;
    Ogre::SceneManager *mSceneMgr;
    Ogre::Camera *mCamera;

	MaterialSchemeHandler* forwardListener = nullptr;
	MaterialSchemeHandler* deferredListener = nullptr;
	DeferredLightCompositionPass * deferredPass = nullptr;

	void setClearColor(CompositorInstance* inst, Viewport* vp);
};

#endif
