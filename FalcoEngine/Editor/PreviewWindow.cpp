#include "stdafx.h"
#include "PreviewWindow.h"
#include "MainWindow.h"
#include "EditorClasses.h"
#include "InputHandler.h"

#include <OgreTextureManager.h>
#include <OgreRoot.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreViewport.h>
#include <OgreTechnique.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreRenderTexture.h>
#include <OgreMaterialManager.h>

#include "../Engine/DynamicLines.h"
#include "../Engine/FBXSceneManager.h"
#include "../Engine/IO.h"
#include "../Engine/SceneSerializer.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

using namespace Ogre;

PreviewWindow::PreviewWindow()
{
}

PreviewWindow::~PreviewWindow()
{
}

void PreviewWindow::init()
{
	sceneRenderTarget = TextureManager::getSingleton().createManual("PreviewRTT",
		ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D,
		1024,
		1024,
		0,
		PixelFormat::PF_FLOAT32_RGB,
		Ogre::TU_RENDERTARGET);

	if (Root::getSingletonPtr()->isInitialised())
	{
		sceneMgr = Root::getSingleton().createSceneManager("OctreeSceneManager");
		sceneMgr->setShadowTechnique(ShadowTechnique::SHADOWTYPE_NONE);

		//Rotation point
		rotationNode = sceneMgr->getRootSceneNode()->createChildSceneNode("RotationCenterNode");
		rotationNode->_setDerivedPosition(Vector3(0, 0, 0));

		//Camera
		cameraNode = rotationNode->createChildSceneNode("PreviewCamera");
		camera = sceneMgr->createCamera("PreviewCamera");
		cameraNode->attachObject(camera);

		sceneRenderTarget->getBuffer()->getRenderTarget()->removeAllViewports();
		mViewport = sceneRenderTarget->getBuffer()->getRenderTarget()->addViewport(camera);
		ColourValue cColor = ColourValue(0.0f, 0.0f, 0.0f, 1.0f);
		mViewport->setBackgroundColour(cColor);

		camera->setAspectRatio(800.0f / 600.0f);
		camera->setAutoAspectRatio(true);
		camera->setNearClipDistance(0.5f);
		camera->setFarClipDistance(5500.0f);
		camera->setFOVy(Radian(Degree(60)));

		cameraNode->setPosition(0, 10, 30);
		cameraNode->lookAt(Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
	}
}

void PreviewWindow::update()
{
	if (opened)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		bool begin = ImGui::Begin("Preview", &opened, ImGuiWindowFlags_NoCollapse);
		ImGui::PopStyleVar();

		if (begin)
		{
			size = Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 22);

			if (size != oldSize)
			{
				updateViewport();
			}

			oldSize = size;

			if (previewTex != nullptr)
			{
				ImVec2 sz = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 22);
				float aspect = 1.0f;

				if (previewTex->getHeight() >= previewTex->getWidth())
					aspect = previewTex->getHeight() / previewTex->getWidth();
				else
					aspect = previewTex->getWidth() / previewTex->getHeight();

				sz = ImVec2((sz.y * aspect), (sz.y));
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - sz.x / 2, 22));
				ImGui::Image((void*)previewTex->getHandle(), sz);

				ImGui::SetCursorPos(ImVec2(7, ImGui::GetWindowSize().y - 40));
				ImGui::Text((to_string(previewTex->getWidth()) + "x" + to_string(previewTex->getHeight())).c_str());
				ImGui::SetCursorPos(ImVec2(7, ImGui::GetWindowSize().y - 22));
				ImGui::Text(textureName.c_str());
			}
			else if (!previewTxt.empty())
			{
				ImGui::SetCursorPosX(10);
				ImGui::SetCursorPosY(25);
				ImGui::TextWrapped(previewTxtFile.c_str());

				ImGuiWindow* window = GImGui->CurrentWindow;
				ImGuiID id = window->GetIDNoKeepAlive("##PreviewTextVS");
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
				ImGui::BeginChild(id, ImVec2(0, 0), true);
				ImGui::TextWrapped(previewTxt.c_str());
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
			else
			{
				ImVec2 sz = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 22);
				ImGui::Image((void*)sceneRenderTarget->getHandle(), sz);

				updateControls();

				if (fbxNode != nullptr)
				{
					ImGui::SetCursorPos(ImVec2(5, size.y - 5));
					ImGui::Text("Scale");
					ImGui::SameLine();

					if (ImGui::SliderFloat("##preview_fbx_scale", &fbxScale, 0.005f, 2.0f))
					{
						rButtonDown = false;
						mButtonDown = false;
						fbxNode->setScale(fbxScale, fbxScale, fbxScale);
					}
				}
			}
		}

		ImGui::End();
	}
}

void PreviewWindow::updateViewport()
{
	if (size.x > 0 && size.y > 0)
	{
		if (sceneRenderTarget != nullptr)
			TextureManager::getSingleton().remove(sceneRenderTarget);

		ColourValue cColor = ColourValue(0.0f, 0.0f, 0.0f, 1.0f);

		sceneRenderTarget = TextureManager::getSingleton().createManual("PreviewRTT",
			ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,
			size.x,
			size.y,
			0,
			PixelFormat::PF_FLOAT32_RGB,
			Ogre::TU_RENDERTARGET);

		// Check rtt viewport
		RenderTarget* sceneRTT = sceneRenderTarget->getBuffer()->getRenderTarget();
		sceneRTT->removeAllViewports();
		mViewport = sceneRTT->addViewport(camera);
		mViewport->setClearEveryFrame(true);
		mViewport->setOverlaysEnabled(false);
		mViewport->setBackgroundColour(cColor);

		MainWindow::Get()->SetupDeferredShading();
	}
}

void PreviewWindow::updateControls()
{
	ImVec2 p = ImGui::GetMousePos();

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			rButtonDown = true;
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
		{
			mButtonDown = true;
		}
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		rButtonDown = false;
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
	{
		mButtonDown = false;
	}

	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Right))
	{
		if (rButtonDown)
		{
			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (delta.x == 0.0f && delta.y == 0.0f) delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
			Vector2 pDiff = Vector2(delta.x, delta.y);

			if (rButtonDown)
			{
				pDiff *= 0.03f;
				EditorClasses::RotateSceneNode(rotationNode, pDiff.x, pDiff.y);
			}
		}
	}

	if (!rotateOnly)
	{
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			if (mButtonDown)
			{
				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
				Vector2 pDiff = Vector2(delta.x, delta.y);

				if (mButtonDown)
				{
					pDiff *= 0.2f;
					EditorClasses::PanningSceneNode(rotationNode, pDiff.x, pDiff.y);
				}
			}
		}

		if (InputHandler::IsShiftPressed())
			cameraSpeed = InputHandler::cameraSpeedFast;
		else
			cameraSpeed = InputHandler::cameraSpeedNormal;

		if (rButtonDown)
		{
			if (InputHandler::wPressed) // W
				EditorClasses::MoveCamera(camera, -camera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Z * cameraSpeed);

			if (InputHandler::sPressed) // S
				EditorClasses::MoveCamera(camera, camera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Z * cameraSpeed);

			if (InputHandler::aPressed) // A
				EditorClasses::MoveCamera(camera, -camera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_X * cameraSpeed);

			if (InputHandler::dPressed) // D
				EditorClasses::MoveCamera(camera, camera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_X * cameraSpeed);

			if (InputHandler::qPressed) // Q
				EditorClasses::MoveCamera(camera, -camera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Y * cameraSpeed);

			if (InputHandler::ePressed) // E
				EditorClasses::MoveCamera(camera, camera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Y * cameraSpeed);
		}
	}

	if (ImGui::IsItemHovered())
	{
		if (ImGui::GetIO().MouseWheel != 0)
		{
			EditorClasses::ZoomCamera(camera, rotationNode->_getDerivedOrientation() * -Vector3::UNIT_Z, ImGui::GetIO().MouseWheel * 5.0f);
		}
	}
}

void PreviewWindow::setupLighting()
{
	SceneNode* lightNode = sceneMgr->getRootSceneNode()->createChildSceneNode("LightNode", Vector3(-10, 10, 0));
	Light* light = sceneMgr->createLight("DirLight0");
	light->setType(Light::LightTypes::LT_DIRECTIONAL);
	light->setDirection(0.1, -0.325, -0.3);

	light->setDiffuseColour(ColourValue::White);
	light->setAttenuation(70, 1.0, 0.045, 0.0075);
	light->setSpecularColour(ColourValue(1.0, 1.0, 1.0, 1.0));
	light->setPowerScale(1.0);

	if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
		light->setCustomParameter(1, Vector4(2.0, 0.0, 0.0, 0.0));

	light->setCastShadows(false);

	lightNode->attachObject(light);

	sceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5, 1.0));
}

void PreviewWindow::setupGrid()
{
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

	MaterialPtr mat_1 = MaterialManager::getSingleton().getByName("MaterialGridGray");
	MaterialPtr mat_2 = MaterialManager::getSingleton().getByName("MaterialGridLightGray");

	DynamicLines* lines = new DynamicLines(mat_1, RenderOperation::OT_LINE_LIST);
	DynamicLines* lines_2 = new DynamicLines(mat_2, RenderOperation::OT_LINE_LIST);

	for (int i = 0; i < points.size(); i++) {
		lines->addPoint(points[i]);
	}

	for (int i = 0; i < points_2.size(); i++) {
		lines_2->addPoint(points_2[i]);
	}

	lines->update();
	lines_2->update();

	SceneNode* linesNode = sceneMgr->getRootSceneNode()->createChildSceneNode("EditorGrid");
	SceneNode* linesNode_2 = sceneMgr->getRootSceneNode()->createChildSceneNode("EditorGrid_2");
	linesNode->attachObject(lines);
	linesNode_2->attachObject(lines_2);
	lines->setRenderQueueGroup(7);
	lines_2->setRenderQueueGroup(8);
}

void PreviewWindow::clearPreview()
{
	sceneMgr->clearScene();
	sceneMgr->setSkyBoxEnabled(false);

	//Rotation point
	rotationNode = sceneMgr->getRootSceneNode()->createChildSceneNode("RotationCenterNode");
	rotationNode->_setDerivedPosition(Vector3(0, 0, 0));

	cameraNode = rotationNode->createChildSceneNode("PreviewCamera");
	cameraNode->setPosition(0, 10, 30);
	cameraNode->lookAt(Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
	cameraNode->attachObject(camera);

	if (textureMat != nullptr)
	{
		MaterialManager::getSingleton().remove(textureMat->getHandle());
		textureMat.reset();
	}

	if (cubemapMat != nullptr)
	{
		MaterialManager::getSingleton().remove(cubemapMat->getHandle());
		cubemapMat.reset();
	}

	previewTxt = "";
	previewTxtFile = "";
	previewTex.reset();
	fbxNode = nullptr;
	//fbxScale = 1.0f;

	currentPreview = C_NONE;
}

void PreviewWindow::previewMaterial(MaterialPtr material)
{
	rotateOnly = true;

	clearPreview();
	setupLighting();

	FBXScenePtr fbxSphere = FBXSceneManager::getSingleton().getByName("Primitives/Sphere.fbx", "Assets");
	fbxSphere->SetSceneManager(sceneMgr);
	SceneNode* node = fbxSphere->ConvertToNativeFormat();
	fbxSphere->SetSceneManager(GetEngine->GetSceneManager());

	if (node->getChildren().size() > 0)
	{
		SceneNode* _node = (SceneNode*)node->getChild(0);
		if (_node->getAttachedObjects().size() > 0)
		{
			((Entity*)_node->getAttachedObject(0))->setCastShadows(false);
			((Entity*)_node->getAttachedObject(0))->getSubEntity(0)->setMaterial(material);
			((Entity*)_node->getAttachedObject(0))->getSubEntity(0)->setOriginalMaterial(material);
		}
	}

	sceneMgr->getRootSceneNode()->addChild(node);
	node->_setDerivedPosition(Vector3(0, 0, 0));
	node->setScale(0.125, 0.125, 0.125);

	camera->lookAt(0, 0, 0);

	currentPreview = C_MATERIAL;
}

void updateEntities(SceneNode* root)
{
	if (root->getAttachedObjects().size() > 0)
	{
		if (root->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
		{
			Entity* ent = (Entity*)root->getAttachedObject(0);
			ent->setCastShadows(false);
			GetEngine->UpdateMaterials(ent);
		}
	}

	auto it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* n = (SceneNode*)it.getNext();
		updateEntities(n);
	}
}

void PreviewWindow::previewFbx(FBXScenePtr fbx)
{
	rotateOnly = false;

	clearPreview();
	setupGrid();
	setupLighting();

	SceneManager* prevMgr = fbx->GetSceneManager();
	fbx->SetSceneManager(sceneMgr);
	SceneNode* node = fbx->ConvertToNativeFormat();
	fbx->SetSceneManager(prevMgr);

	updateEntities(node);

	sceneMgr->getRootSceneNode()->addChild(node);
	node->_setDerivedPosition(Vector3(0, 0, 0));
	//node->setScale(0.125, 0.125, 0.125);

	fbxNode = node;

	camera->lookAt(0, 0, 0);

	fbxNode->setScale(fbxScale, fbxScale, fbxScale);

	currentPreview = C_FBX;
}

void PreviewWindow::previewCubemap(CubemapPtr cubemap)
{
	rotateOnly = true;

	clearPreview();
	setupLighting();

	cubemapMat = MaterialManager::getSingleton().create("Preview/CubemapMaterial", "Editor", true);
	cubemapMat->removeAllTechniques();
	Technique* tech = cubemapMat->createTechnique();
	tech->setName("Forward");
	Pass* pass = tech->createPass();
	pass->setCullingMode(CullingMode::CULL_NONE);
	TextureUnitState* state = pass->createTextureUnitState();
	state->setCubemap(cubemap);

	sceneMgr->setSkyBox(true, "Preview/CubemapMaterial", 1000, true, Quaternion::IDENTITY, "Editor");

	currentPreview = C_CUBEMAP;
}

void PreviewWindow::previewTexture(TexturePtr texture)
{
	clearPreview();

	previewTex = texture;
	textureName = previewTex->getName();

	currentPreview = C_TEXTURE;
}

void PreviewWindow::previewFont(Ogre::FontPtr font)
{
	TexturePtr tex = font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->_getTexturePtr();
	previewTexture(tex);
	textureName = font->getName();
	currentPreview = C_FONT;
}

void PreviewWindow::previewPrefab(std::string prefabPath)
{
	rotateOnly = false;

	clearPreview();
	setupGrid();
	setupLighting();

	std::string path = MainWindow::GetProjectAssetsDir() + prefabPath;

	if (IO::FileExists(path))
	{
		SceneSerializer serializer;
		SceneNode* prefab = serializer.DeserializeFromPrefab(sceneMgr, path, true);

		if (prefab != nullptr)
		{
			prefab->_setDerivedPosition(Vector3(0, 0, 0));
			prefab->setScale(1, 1, 1);

			updateEntities(prefab);

			fbxNode = prefab;

			fbxNode->setScale(fbxScale, fbxScale, fbxScale);
		}

		camera->lookAt(0, 0, 0);

		currentPreview = C_PREFAB;
	}
}

void PreviewWindow::previewText(std::string path)
{
	clearPreview();

	currentPreview = C_TEXT;

	string text = IO::ReadText(GetEngine->GetAssetsPath() + path);
	previewTxt = text;
	previewTxtFile = path;
}
