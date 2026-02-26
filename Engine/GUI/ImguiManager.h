#pragma once

#include <imgui.h>
#include <OgreCommon.h>

#include <OgreRenderQueueListener.h>
#include <OgreSingleton.h>
#include <OgreTexture.h>
//#include <OgreInput.h>

#include "ImguiRenderable.h"

namespace Ogre
{
    class SceneManager;
    class TextureUnitState;

    class ImguiManager : public RenderQueueListener, public Singleton<ImguiManager>
    {
    public:
        static void createSingleton();

        ImguiManager();
        ~ImguiManager();

        virtual void init(Ogre::SceneManager* mgr);

        virtual void newFrame(float deltaTime,const Ogre::Rect & windowRect);

        //inherited from RenderQueueListener
        virtual void renderQueueEnded(uint8 queueGroupId, const String& invocation,bool& repeatThisInvocation);

        //bool mouseWheelRolled(int val);
        //bool mouseMoved(int x, int y);
		//bool mousePressed(int btn);
		//bool mouseReleased(int btn);
		//bool keyPressed( const OgreBites::KeyboardEvent &arg );
		//bool keyReleased( const OgreBites::KeyboardEvent &arg );

		ImFont* addFont(const String& name, const String& group, int size);

        static ImguiManager& getSingleton(void);
        static ImguiManager* getSingletonPtr(void);

    protected:

        void createFontTexture();
        void createMaterial();

        SceneManager*				mSceneMgr;
        TextureUnitState*           mTexUnit;
        int                         mLastRenderedFrame;

        ImGUIRenderable             mRenderable;
        TexturePtr                  mFontTex;

        bool                        mFrameEnded;
    };
}
