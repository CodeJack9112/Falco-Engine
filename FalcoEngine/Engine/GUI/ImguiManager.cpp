#include <imgui.h>
#include "ImguiRenderable.h"
#include "ImguiManager.h"

#include <OgreMaterialManager.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>
#include <OgreString.h>
#include <OgreStringConverter.h>
#include <OgreViewport.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreHighLevelGpuProgram.h>
#include <OgreUnifiedHighLevelGpuProgram.h>
#include <OgreRoot.h>
#include <OgreTechnique.h>
#include <OgreTextureUnitState.h>
#include <OgreViewport.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTarget.h>
#include <Ogre/source/Components/Overlay/include/OgreFontManager.h>
#include <Ogre/source/Components/Overlay/include/OgreFont.h>

#include "../Engine.h"

using namespace Ogre;

template<> ImguiManager* Singleton<ImguiManager>::msSingleton = 0;

void ImguiManager::createSingleton()
{
    if(!msSingleton)
    {
        msSingleton = new ImguiManager();
    }
}

ImguiManager* ImguiManager::getSingletonPtr(void)
{
    createSingleton();
    return msSingleton;
}

ImguiManager& ImguiManager::getSingleton(void)
{  
    createSingleton();
    return ( *msSingleton );  
}

ImguiManager::ImguiManager()
:mSceneMgr(0)
,mLastRenderedFrame(-1)
{
    ImGui::CreateContext();
}

ImguiManager::~ImguiManager()
{
    ImGui::DestroyContext();
    mSceneMgr->removeRenderQueueListener(this);
}

void ImguiManager::init(Ogre::SceneManager * mgr)
{
    mSceneMgr  = mgr;

    mSceneMgr->addRenderQueueListener(this);
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::GetStyle().WindowRounding = 0;
    ImGui::GetStyle().ColorButtonPosition = ImGuiDir_Left;
    ImGui::GetStyle().IndentSpacing = 15;
    ImGui::GetStyle().FrameRounding = 4;
    ImGui::GetStyle().GrabRounding = 3;

    ImVec4* colors = ImGui::GetStyle().Colors;
    /*colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.32f, 0.32f, 0.32f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.43f, 0.43f, 0.43f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.51f, 0.51f, 0.51f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.26f, 0.26f, 0.26f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.84f, 0.84f, 0.84f, 0.70f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.72f, 0.72f, 0.72f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);*/

    colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.11f, 0.11f, 0.11f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.27f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.23f);
    colors[ImGuiCol_Button] = ImVec4(0.42f, 0.42f, 0.42f, 0.47f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.19f, 0.19f, 0.19f, 0.94f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.31f, 0.31f, 0.31f, 0.94f);

    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.32f, 0.32f, 0.32f, 0.40f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 0.31f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.26f, 0.26f, 0.26f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.80f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.84f, 0.84f, 0.84f, 0.70f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.72f, 0.72f, 0.72f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

    createFontTexture();
    createMaterial();
}

//-----------------------------------------------------------------------------------
void ImguiManager::renderQueueEnded(uint8 queueGroupId, const String& invocation,bool& repeatThisInvocation)
{
    if (!GetEngine->GetDrawGUI())
        return;

    if((queueGroupId != Ogre::RENDER_QUEUE_OVERLAY) || (invocation == "SHADOWS"))
    {
        return;
    }
    
    Ogre::RenderSystem* renderSys = Ogre::Root::getSingletonPtr()->getRenderSystem();
    Ogre::Viewport* vp = renderSys->_getViewport();
    
    if (!vp || (!vp->getTarget()->isPrimary()) || mFrameEnded)
    {
        return;
    }
    
    mFrameEnded = true;
    ImGuiIO& io = ImGui::GetIO();
    
    // Construct projection matrix, taking texel offset corrections in account (important for DirectX9)
    // See also:
    //     - OGRE-API specific hint: http://www.ogre3d.org/forums/viewtopic.php?f=5&p=536881#p536881
    //     - IMGUI Dx9 demo solution: https://github.com/ocornut/imgui/blob/master/examples/directx9_example/imgui_impl_dx9.cpp#L127-L138
    const float texelOffsetX = renderSys->getHorizontalTexelOffset();
    const float texelOffsetY = renderSys->getVerticalTexelOffset();
    const float L = texelOffsetX;
    const float R = io.DisplaySize.x + texelOffsetX;
    const float T = texelOffsetY;
    const float B = io.DisplaySize.y + texelOffsetY;
    
    mRenderable.mXform = Matrix4(   2.0f/(R-L),    0.0f,         0.0f,       (L+R)/(L-R),
                                    0.0f,         -2.0f/(B-T),   0.0f,       (T+B)/(B-T),
                                    0.0f,          0.0f,        -1.0f,       0.0f,
                                    0.0f,          0.0f,         0.0f,       1.0f);
    
    // Instruct ImGui to Render() and process the resulting CmdList-s
    /// Adopted from https://bitbucket.org/ChaosCreator/imgui-ogre2.1-binding
    /// ... Commentary on OGRE forums: http://www.ogre3d.org/forums/viewtopic.php?f=5&t=89081#p531059
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    int vpWidth  = vp->getActualWidth();
    int vpHeight = vp->getActualHeight();
    for (int i = 0; i < draw_data->CmdListsCount; ++i)
    {
        const ImDrawList* draw_list = draw_data->CmdLists[i];
        unsigned int startIdx = 0;

        for (int j = 0; j < draw_list->CmdBuffer.Size; ++j)
        {
            // Create a renderable and fill it's buffers
            const ImDrawCmd *drawCmd = &draw_list->CmdBuffer[j];
            mRenderable.updateVertexData(draw_list->VtxBuffer.Data, &draw_list->IdxBuffer.Data[startIdx], draw_list->VtxBuffer.Size, drawCmd->ElemCount);

            // Set scissoring
            int scLeft   = static_cast<int>(drawCmd->ClipRect.x); // Obtain bounds
            int scTop    = static_cast<int>(drawCmd->ClipRect.y);
            int scRight  = static_cast<int>(drawCmd->ClipRect.z);
            int scBottom = static_cast<int>(drawCmd->ClipRect.w);

            scLeft   = scLeft   < 0 ? 0 : (scLeft  > vpWidth ? vpWidth : scLeft); // Clamp bounds to viewport dimensions
            scRight  = scRight  < 0 ? 0 : (scRight > vpWidth ? vpWidth : scRight);
            scTop    = scTop    < 0 ? 0 : (scTop    > vpHeight ? vpHeight : scTop);
            scBottom = scBottom < 0 ? 0 : (scBottom > vpHeight ? vpHeight : scBottom);

            Pass * pass = mRenderable.mMaterial->getBestTechnique()->getPass(0);
            TextureUnitState * st = pass->getTextureUnitState(0);
            if (drawCmd->TextureId != 0)
            {
                Ogre::ResourceHandle handle = (Ogre::ResourceHandle)drawCmd->TextureId;
                Ogre::TexturePtr tex = Ogre::static_pointer_cast<Ogre::Texture>(
                    Ogre::TextureManager::getSingleton().getByHandle(handle));
                if (tex)
                {
                    st->setTexture(tex);
                    st->setTextureFiltering(Ogre::TFO_TRILINEAR);
                }
            }
            else
            {
                st->setTexture(mFontTex);
                st->setTextureFiltering(Ogre::TFO_NONE);
            }
            renderSys->setScissorTest(true, scLeft, scTop, scRight, scBottom);

            // Render!
            mSceneMgr->_injectRenderWithPass(pass,
                                             &mRenderable, false);

            // Update counts
            startIdx += drawCmd->ElemCount;
        }
    }
    renderSys->setScissorTest(false);
}
//-----------------------------------------------------------------------------------
void ImguiManager::createMaterial()
{
    mRenderable.mMaterial = MaterialManager::getSingleton().create(
        "imgui/material", ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    Pass* mPass = mRenderable.mMaterial->getTechnique(0)->getPass(0);

    mPass->setCullingMode(CULL_NONE);
    mPass->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
    mPass->setLightingEnabled(false);
    mPass->setVertexColourTracking(TVC_DIFFUSE);
    mPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    mPass->setSeparateSceneBlendingOperation(Ogre::SBO_ADD,Ogre::SBO_ADD);
    mPass->setSeparateSceneBlending(Ogre::SBF_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA,Ogre::SBF_ZERO);
        
    mTexUnit = mPass->createTextureUnitState();
    mTexUnit->setTexture(mFontTex);
    mTexUnit->setTextureFiltering(Ogre::TFO_NONE);

    mRenderable.mMaterial->load();
}

void ImguiManager::createFontTexture()
{
    // Build texture atlas

	if (mFontTex != nullptr)
		TextureManager::getSingleton().remove(mFontTex);

    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    mFontTex = TextureManager::getSingleton().createManual("ImguiFontTex", "Assets", TEX_TYPE_2D,width,height,1,1,PF_BYTE_RGBA);

    const PixelBox & lockBox = mFontTex->getBuffer()->lock(Box(0, 0, width, height), HardwareBuffer::HBL_DISCARD);
	size_t texDepth = PixelUtil::getNumElemBytes(lockBox.format);

    memcpy(lockBox.data,pixels, width*height*texDepth);
	mFontTex->getBuffer()->unlock();
}

ImFont* ImguiManager::addFont(const String& name, const String& group, int size)
{
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
	FontPtr font = FontManager::getSingleton().getByName(name, group);
	OgreAssert(font, "font does not exist");
	OgreAssert(font->getType() == FT_TRUETYPE, "font must be of FT_TRUETYPE");
	DataStreamPtr dataStreamPtr = ResourceGroupManager::getSingleton().openResource(font->getSource(), font->getGroup());
	MemoryDataStream ttfchunk(dataStreamPtr, false); // transfer ownership to imgui

	ImGuiIO& io = ImGui::GetIO();
	ImFont* fnt = io.Fonts->AddFontFromMemoryTTF(ttfchunk.getPtr(), ttfchunk.size(), size, NULL, io.Fonts->GetGlyphRangesCyrillic() /*font->getTrueTypeSize()*/);

	createFontTexture();

	return fnt;
#else
	OGRE_EXCEPT(Exception::ERR_INVALID_CALL, "Ogre Overlay Component required");
	return NULL;
#endif
}

void ImguiManager::newFrame(float deltaTime,const Ogre::Rect & windowRect)
{
    mFrameEnded=false;
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaTime;

     // Read keyboard modifiers inputs
    io.KeyAlt = false;// mKeyInput->isKeyDown(OIS::KC_LMENU);
    io.KeySuper = false;

    // Setup display size (every frame to accommodate for window resizing)
     io.DisplaySize = ImVec2((float)(windowRect.right - windowRect.left), (float)(windowRect.bottom - windowRect.top));

    // Start the frame
    ImGui::NewFrame();
}
