#ifndef _DEFERRED_LIGHT_CP_H
#define _DEFERRED_LIGHT_CP_H

#include "OgreCompositorInstance.h"
#include "OgreCustomCompositionPass.h"

#include "DLight.h"
#include "MaterialGenerator.h"
#include "AmbientLight.h"
#include "OgreRenderTargetListener.h"

// Material scheme name for shadows generation
#define DEFERRED_SHADOWS_SCHEME_NAME "DeferredShading/Shadows"

// Shadow caster material name
#define DEFERRED_SHADOW_CASTER_NAME "DeferredShading/Shadows/Caster"

//The render operation that will be called each frame in the custom composition pass
//This is the class that will send the actual render calls of the spheres (point lights),
//cones (spotlights) and quads (directional lights) after the GBuffer has been constructed
class DeferredLightCompositionPass;

class DeferredLightRenderOperation : public RenderTargetListener, public Ogre::CompositorInstance::RenderSystemOperation, public Ogre::MaterialManager::Listener
{
public:
    typedef std::map<Ogre::Light*, DLight*> LightsMap;

    DeferredLightRenderOperation(Ogre::CompositorInstance* instance, const Ogre::CompositionPass* pass);
    
    /** @copydoc CompositorInstance::RenderSystemOperation::execute */
    virtual void execute(Ogre::SceneManager *sm, Ogre::RenderSystem *rs);
	virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend);

    virtual ~DeferredLightRenderOperation();

	static void UpdateStaticShadowmaps();
    LightsMap& getLightsMap() { return mLights; }

    DeferredLightCompositionPass* parent = nullptr;
    //The material generator for the light geometry
    MaterialGenerator* mLightMaterialGenerator = nullptr;

private:
	//static DeferredLightRenderOperation* self;

    /** Create a new deferred light
     */
    DLight *createDLight(Ogre::Light* light);
    
    //The texture names of the GBuffer components
    Ogre::String mTexName0;
    Ogre::String mTexName1;

    //The map of deferred light geometries already constructed
    LightsMap mLights;

    //The ambient light used to render the scene
    AmbientLight* mAmbientLight;

    //The viewport that we are rendering to
    Ogre::Viewport* mViewport;

	bool gizmoVisible = false;

	virtual void preRenderTargetUpdate(const RenderTargetEvent & evt);
	virtual void postRenderTargetUpdate(const RenderTargetEvent & evt);

	Matrix4 textureProjMatrix;
	
	MaterialPtr shadowCasterMaterial;

	void UpdateShadowmap(DLight* dLight, Camera * cam, Pass * lightingPass);
};

//The custom composition pass that is used for rendering the light geometry
//This class needs to be registered with the CompositorManager
class DeferredLightCompositionPass : public Ogre::CustomCompositionPass
{
private:
    std::vector<DeferredLightRenderOperation*> renderOps;

public:

    /** @copydoc CustomCompositionPass::createOperation */
    virtual Ogre::CompositorInstance::RenderSystemOperation* createOperation(Ogre::CompositorInstance* instance, const Ogre::CompositionPass* pass)
    {
        DeferredLightRenderOperation * renderOp = OGRE_NEW DeferredLightRenderOperation(instance, pass);
        renderOp->parent = this;
        renderOps.push_back(renderOp);

        return renderOp;
    }

    virtual ~DeferredLightCompositionPass()
    {
        for (auto it = renderOps.begin(); it != renderOps.end(); ++it)
        {
            delete *it;
        }

        renderOps.clear();
    }

    void removeRenderOperation(DeferredLightRenderOperation* op)
    {
        auto it = find(renderOps.begin(), renderOps.end(), op);
        if (it != renderOps.end())
            renderOps.erase(it);
    }

    std::vector<DeferredLightRenderOperation*>& getRenderOperations() { return renderOps; }
};

#endif
