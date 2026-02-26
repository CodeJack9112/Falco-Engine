#pragma once

#include <OgreVector2.h>
#include <OgreTexture.h>
#include "../Ogre/source/Components/Overlay/include/OgreFont.h"
#include "../Ogre/source/OgreMain/custom/Cubemap.h"
#include "../Engine/FBXScene.h"

class PreviewWindow
{
public:
	enum CurrentPreview { C_NONE, C_MATERIAL, C_FBX, C_CUBEMAP, C_TEXTURE, C_FONT, C_PREFAB, C_TEXT };

	PreviewWindow();
	~PreviewWindow();

	void update();
	void init();
	void show(bool show) { opened = show; }
	bool getVisible() { return opened; }

	void previewMaterial(MaterialPtr material);
	void previewFbx(FBXScenePtr fbx);
	void previewCubemap(CubemapPtr cubemap);
	void previewTexture(TexturePtr texture);
	void previewFont(Ogre::FontPtr font);
	void previewPrefab(std::string prefabPath);
	void previewText(std::string path);

	Viewport* getViewport() { return mViewport; }
	void clearPreview();

	CurrentPreview getCurrentPreview() { return currentPreview; }

private:
	bool opened = true;

	Ogre::Vector2 size = Ogre::Vector2(0, 0);
	Ogre::Vector2 oldSize = Ogre::Vector2(0, 0);

	float fbxScale = 1.0f;
	
	bool rButtonDown = false;
	bool mButtonDown = false;
	bool rotateOnly = false;

	float cameraSpeed = 1;

	SceneNode* fbxNode = nullptr;
	Ogre::TexturePtr previewTex;
	Ogre::TexturePtr sceneRenderTarget;
	std::string textureName = "";
	std::string previewTxt = "";
	std::string previewTxtFile = "";

	SceneManager* sceneMgr = nullptr;
	Camera* camera = nullptr;
	Viewport* mViewport = nullptr;
	SceneNode* cameraNode = nullptr;
	SceneNode* rotationNode = nullptr;

	MaterialPtr textureMat;
	MaterialPtr cubemapMat;

	CurrentPreview currentPreview = C_NONE;

	void setupLighting();
	void setupGrid();

	void updateViewport();
	void updateControls();
};

