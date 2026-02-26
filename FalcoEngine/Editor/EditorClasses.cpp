#include "stdafx.h"
#include "EditorClasses.h"
#include <OgreEntity.h>
#include <OgreCamera.h>
#include <OgreParticleSystem.h>
#include <OgreMovableObject.h>
#include <OgreBillboardSet.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreMesh.h>
#include <OgreBillboard.h>
#include <OgreViewport.h>
#include <OgreMeshManager.h>

#include "../Engine/FBXSceneManager.h"
#include "../Engine/StringConverter.h"
#include "../Engine/Engine.h"
#include "../Engine/IO.h"
#include "../Engine/AudioListener.h"

EditorClasses::EditorClasses()
{
}

EditorClasses::~EditorClasses()
{
}

SceneNode * EditorClasses::PlaceSceneModel(SceneManager* mSceneMgr, String path, Ogre::Vector2 point)
{
	try
	{
		//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

		Ogre::String name = GetEngine->GenerateNewName(mSceneMgr);

		Entity* model = mSceneMgr->createEntity(name, path);

		model->getMesh()->setVertexBufferPolicy(HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, true);
		model->getMesh()->setIndexBufferPolicy(HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
		
		unsigned short coord;
		unsigned short index;

		if (!model->getMesh()->suggestTangentVectorBuildParams(VertexElementSemantic::VES_TANGENT, coord, index))
		{
			model->getMesh()->buildTangentVectors(VertexElementSemantic::VES_TANGENT, coord, index);
		}

		model->getMesh()->setAutoBuildEdgeLists(true);

		SceneNode *pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
		pNode->attachObject(model);
		model->setQueryFlags(1 << 1);
		
		//Raycast

		Vector3 pos = GetWorldPointFromScreenPoint(point);
		pNode->setPosition(pos);

		/////////////////

		return pNode;
	}
	catch (Ogre::Exception e)
	{
		MessageBoxA(0, e.getDescription().c_str(), "Error", MB_OK);
		return NULL;
	}
}

SceneNode * EditorClasses::PlaceFBXScene(SceneManager * mSceneMgr, String path, Ogre::Vector2 point, float distance)
{
	FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(path, "Assets");

	SceneNode * node = fbx->ConvertToNativeFormat();
	mSceneMgr->getRootSceneNode()->addChild(node);

	node->setAlias(IO::GetFileName(path));

	Vector3 pos;

	if (distance == 0.0f)
		pos = GetWorldPointFromScreenPoint(point);
	else
		pos = GetScreenCenter(mSceneMgr, distance);

	node->setPosition(pos);

	GetEngine->UpdateSceneNodeIndexes();

	return node;
}

SceneNode * EditorClasses::CreateLight(SceneManager * mSceneMgr, Light::LightTypes lightType)
{
	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "Light_");

	SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Light* light = mSceneMgr->createLight(name);
	light->setDiffuseColour(ColourValue::White);
	light->setType(lightType);
	light->setAttenuation(70, 1.0, 0.045, 0.0075);
	light->setSpecularColour(ColourValue(1.0, 1.0, 1.0, 1.0));
	light->setPowerScale(1.0);

	if (light->getType() == Light::LightTypes::LT_POINT)
	{
		light->setCustomParameter(1, Vector4(0.0, 0.0, 0.0, 0.0));
		light->setShadowBias(0.0004f);
	}

	if (light->getType() == Light::LightTypes::LT_SPOTLIGHT)
	{
		light->setCustomParameter(1, Vector4(1.0, 0.0, 0.0, 0.0));
		light->setShadowBias(0.0027);
	}

	if (light->getType() == Light::LightTypes::LT_DIRECTIONAL)
	{
		light->setCustomParameter(1, Vector4(2.0, 0.0, 0.0, 0.0));
		light->setShadowBias(0.0f);
	}

	lightNode->attachObject(light);

	//Billboard image of this point light
	CreateBillboard(mSceneMgr, lightNode, name + "_gizmo", "LightBillboard");

	lightNode->setPosition(GetScreenCenter(mSceneMgr, 30));

	GetEngine->UpdateSceneNodeIndexes();

	return lightNode;
}

SceneNode * EditorClasses::CreateEmptyObject(SceneManager * mSceneMgr)
{
	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "Empty_");

	SceneNode* emptyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Empty * empty = (Empty*)mSceneMgr->createMovableObject(name, EmptyObjectFactory::FACTORY_TYPE_NAME);
	emptyNode->attachObject(empty);

	emptyNode->setPosition(GetScreenCenter(mSceneMgr, 30));

	GetEngine->UpdateSceneNodeIndexes();

	return emptyNode;
}

SceneNode * EditorClasses::CreateCamera(SceneManager * mSceneMgr)
{
	//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "Camera_");

	SceneNode* cameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	Camera * camera = mSceneMgr->createCamera(name);
	camera->setFOVy(Radian(Degree(60)));
	camera->setNearClipDistance(5);
	camera->setFarClipDistance(1000);
	cameraNode->attachObject(camera);

	//Billboard image of this camera
	CreateBillboard(mSceneMgr, cameraNode, name + "_gizmo", "CameraBillboard");

	cameraNode->setPosition(GetScreenCenter(mSceneMgr, 30));

	bool hasCam = false;

	auto cams = mSceneMgr->getCameras();

	for (auto it = cams.begin(); it != cams.end(); ++it)
	{
		Camera* cam = it->second;
		if (cam->getMainCamera())
		{
			hasCam = true;
			break;
		}
	}

	if (!hasCam)
	{
		camera->setMainCamera(true);

		AudioListener* audio = new AudioListener(cameraNode);
		cameraNode->components.push_back(audio);
	}

	GetEngine->UpdateSceneNodeIndexes();

	return cameraNode;
}

SceneNode * EditorClasses::CreateUICanvas(SceneManager * mSceneMgr)
{
	//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "UI_Canvas_");

	SceneNode* uiNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	UICanvas * uiCanvas = (UICanvas*)GetEngine->GetSceneManager()->createMovableObject(name, UICanvasFactory::FACTORY_TYPE_NAME);

	GetEngine->UpdateSceneNodeIndexes();

	if (uiCanvas->getParentSceneNode() != nullptr)
	{
		return uiCanvas->getParentSceneNode();
	}
	else
	{
		uiNode->attachObject((MovableObject*)uiCanvas);
		uiNode->setPosition(Vector3(0, 0, 0));
		return uiNode;
	}
}

SceneNode * EditorClasses::CreateUIButton(SceneManager * mSceneMgr)
{
	//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "UI_Button_");

	SceneNode* uiNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	UIButton * uiElement = (UIButton*)GetEngine->GetSceneManager()->createMovableObject(name, UIButtonFactory::FACTORY_TYPE_NAME);
	uiNode->attachObject((MovableObject*)uiElement);

	if (UICanvasFactory::uiCanvas[mSceneMgr] != nullptr)
	{
		uiNode->getParentSceneNode()->removeChild(uiNode);
		UICanvasFactory::uiCanvas[mSceneMgr]->getParentSceneNode()->addChild(uiNode);
	}

	uiNode->setPosition(Vector3(0, 0, 0));

	GetEngine->UpdateSceneNodeIndexes();

	return uiNode;
}

SceneNode * EditorClasses::CreateUIText(SceneManager * mSceneMgr)
{
	//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "UI_Text_");

	SceneNode* uiNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	UIText * uiElement = (UIText*)GetEngine->GetSceneManager()->createMovableObject(name, UITextFactory::FACTORY_TYPE_NAME);
	uiNode->attachObject((MovableObject*)uiElement);

	if (UICanvasFactory::uiCanvas[mSceneMgr] != nullptr)
	{
		uiNode->getParentSceneNode()->removeChild(uiNode);
		UICanvasFactory::uiCanvas[mSceneMgr]->getParentSceneNode()->addChild(uiNode);
	}

	uiNode->setPosition(Vector3(0, 0, 0));

	GetEngine->UpdateSceneNodeIndexes();

	return uiNode;
}

SceneNode * EditorClasses::CreateUIImage(SceneManager * mSceneMgr)
{
	//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "UI_Image_");

	SceneNode* uiNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	UIImage * uiElement = (UIImage*)GetEngine->GetSceneManager()->createMovableObject(name, UIImageFactory::FACTORY_TYPE_NAME);
	uiNode->attachObject((MovableObject*)uiElement);

	if (UICanvasFactory::uiCanvas[mSceneMgr] != nullptr)
	{
		uiNode->getParentSceneNode()->removeChild(uiNode);
		UICanvasFactory::uiCanvas[mSceneMgr]->getParentSceneNode()->addChild(uiNode);
	}

	uiNode->setPosition(Vector3(0, 0, 0));

	GetEngine->UpdateSceneNodeIndexes();

	return uiNode;
}

SceneNode * EditorClasses::CreateParticleSystem(SceneManager * mSceneMgr)
{
	//((CMainFrame*)theApp.m_pMainWnd)->GetPreviewWindow()->ClearPreview();

	Ogre::String name = GetEngine->GenerateNewName(mSceneMgr, "ParticleSystem_");

	SceneNode* particleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);
	ParticleSystem * particleSystem = mSceneMgr->createParticleSystem(name, 10000, "Assets");
	particleSystem->setDefaultDimensions(1.0f, 1.0f);
	particleNode->attachObject(particleSystem);

	//Billboard image
	CreateBillboard(mSceneMgr, particleNode, name + "_gizmo", "ParticleSystemBillboard");

	particleNode->setPosition(GetScreenCenter(mSceneMgr, 30));

	GetEngine->UpdateSceneNodeIndexes();

	return particleNode;
}

BillboardSet * EditorClasses::CreateBillboard(SceneManager * mSceneMgr, SceneNode * parentNode, string name, string materialName)
{
	SceneNode * billboardNode = parentNode->createChildSceneNode(name + "_gizmo");
	billboardNode->setScale(Vector3(0.015));
	BillboardSet * billboardSet = mSceneMgr->createBillboardSet(1);
	billboardSet->setCastShadows(false);
	billboardSet->setMaterialName(materialName, "Editor");
	billboardSet->setQueryFlags(1 << 1);
	billboardSet->setRenderQueueGroup(RENDER_QUEUE_6 + 1);
	Billboard * billboard = billboardSet->createBillboard(Vector3());
	billboardNode->attachObject(billboardSet);

	billboardSet->getBillboard(0)->setPosition(0, 0, 0);
	billboardSet->_updateBounds();

	return billboardSet;
}

Vector3 EditorClasses::GetScreenCenter(SceneManager* mSceneMgr, float distance)
{
	//Raycast
	Camera * camera = (Camera*)mSceneMgr->getSceneNode("EditorCamera")->getAttachedObject(0);
	float width = (float)camera->getViewport()->getActualWidth(); // viewport width
	float height = (float)camera->getViewport()->getActualHeight(); // viewport height

	Ray ray = camera->getCameraToViewportRay((float)(width / 2) / width, (float)(height / 2) / height);
	Vector3 point = ray.getPoint(distance);

	return point;
}

Vector3 EditorClasses::GetWorldPointFromScreenPoint(Ogre::Vector2 point)
{
	Camera * camera = (Camera*)GetEngine->GetSceneManager()->getSceneNode("EditorCamera")->getAttachedObject(0);
	float width = (float)camera->getViewport()->getActualWidth();
	float height = (float)camera->getViewport()->getActualHeight();

	Ray ray = camera->getCameraToViewportRay((float)point.x / width, (float)point.y / height);

	float distance = -1.0;
	RaycastGeometry * raycast = new RaycastGeometry(GetEngine->GetSceneManager(), camera);
	raycast->SetTypeMask(SceneManager::ENTITY_TYPE_MASK);
	raycast->SetMask(1 << 1);
	MovableObject * entity = raycast->RaycastFromPoint(Vector2((float)point.x / width, (float)point.y / height), distance);

	Vector3 _retPoint;

	if (entity != NULL)
	{
		_retPoint = ray.getPoint(distance);
	}
	else
	{
		_retPoint = ray.getPoint(30);
	}

	delete raycast;

	return _retPoint;
}

void EditorClasses::RotateCamera(Camera* pTargetCamera, Real rOffsetX, Real rOffsetY)
{
	Quaternion qRotX;
	qRotX.FromAngleAxis(Radian(rOffsetX), Vector3::UNIT_X);
	pTargetCamera->getParentSceneNode()->yaw(-Radian(rOffsetX * 0.3f), Ogre::Node::TS_WORLD);

	Quaternion qRotY;
	//Right
	qRotY.FromAngleAxis(Radian(rOffsetY), Vector3::UNIT_Y);
	pTargetCamera->getParentSceneNode()->pitch(-Radian(rOffsetY * 0.3f), Ogre::Node::TS_LOCAL);
}

void EditorClasses::RotateCameraOrbit(Camera* pTargetCamera, Vector3 vOrbitCenter, Real rOffsetX, Real rOffsetY)
{
	Quaternion qRotX;
	qRotX.FromAngleAxis(Radian(rOffsetX), Vector3::UNIT_X);
	pTargetCamera->getParentSceneNode()->yaw(-Radian(rOffsetX * 0.3f), Ogre::Node::TS_WORLD);

	Quaternion qRotY;
	//Right
	qRotY.FromAngleAxis(Radian(rOffsetY), Vector3::UNIT_Y);
	pTargetCamera->getParentSceneNode()->pitch(-Radian(rOffsetY * 0.3f), Ogre::Node::TS_LOCAL);

	Vector3 vPos = qRotY * qRotX * (pTargetCamera->getParentSceneNode()->getPosition() - vOrbitCenter);
	pTargetCamera->getParentSceneNode()->setPosition(vPos + vOrbitCenter);
}

void EditorClasses::PanningCamera(Camera* pTargetCamera, Real rOffsetX, Real rOffsetY)
{
	Vector3 vCamPos = pTargetCamera->getParentSceneNode()->getPosition();
	Vector3 vDirUp = pTargetCamera->getRealUp();
	Vector3 vDirRight = pTargetCamera->getRealRight();

	vCamPos += (vDirRight * -rOffsetX) + (vDirUp * rOffsetY);
	pTargetCamera->getParentSceneNode()->setPosition(vCamPos);
}

void EditorClasses::MoveCamera(Camera* pTargetCamera, Vector3 vDirection, Real rDistance)
{
	Vector3 vPosition = pTargetCamera->getParentSceneNode()->getPosition();
	vPosition += vDirection * rDistance;
	pTargetCamera->getParentSceneNode()->setPosition(vPosition);
}

void EditorClasses::ZoomCamera(Camera* pTargetCamera, Real rDistance)
{
	Vector3 vCamPos = pTargetCamera->getParentSceneNode()->getPosition();
	Vector3 vCamDir = pTargetCamera->getRealDirection();
	vCamPos += vCamDir * rDistance;
	pTargetCamera->getParentSceneNode()->setPosition(vCamPos);
}

void EditorClasses::ZoomCamera(Camera* pTargetCamera, Vector3 dir, Real rDistance)
{
	Vector3 vCamPos = pTargetCamera->getParentSceneNode()->_getDerivedPosition();
	vCamPos += dir * rDistance;
	pTargetCamera->getParentSceneNode()->_setDerivedPosition(vCamPos);
}

void EditorClasses::MoveCamera(Camera* pTargetCamera, Vector3 dir)
{
	pTargetCamera->getParentSceneNode()->translate(dir);
}

void EditorClasses::RotateSceneNode(SceneNode* pTargetNode, Real rOffsetX, Real rOffsetY)
{
	Quaternion qRotX;
	qRotX.FromAngleAxis(Radian(rOffsetX), Vector3::UNIT_X);
	pTargetNode->yaw(-Radian(rOffsetX * 0.3f), Ogre::Node::TS_WORLD);

	Quaternion qRotY;
	//Right
	qRotY.FromAngleAxis(Radian(rOffsetY), Vector3::UNIT_Y);
	pTargetNode->pitch(-Radian(rOffsetY * 0.3f), Ogre::Node::TS_LOCAL);
}

void EditorClasses::PanningSceneNode(SceneNode* pTargetNode, Real rOffsetX, Real rOffsetY)
{
	Vector3 vCamPos = pTargetNode->getPosition();
	Vector3 vDirUp = pTargetNode->_getDerivedOrientation() * Vector3::UNIT_Y; // ->getRealUp();
	Vector3 vDirRight = pTargetNode->_getDerivedOrientation() * Vector3::UNIT_X; //pTargetCamera->getRealRight();

	vCamPos += (vDirRight * -rOffsetX) + (vDirUp * rOffsetY);
	pTargetNode->setPosition(vCamPos);
}

std::string EditorClasses::CSTR2STR(CString str)
{
	std::string _str = ::StringConvert::ws2s(str.GetString(), GetACP());

	return _str;
}

MaterialPtr EditorClasses::CreateColoredMaterial(string name, ColourValue color)
{
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(name, "Editor", true);
	Ogre::Technique* mTech = mMat->createTechnique();
	Ogre::Pass* mPass = mTech->createPass();
	mPass = mMat->getTechnique(0)->getPass(0);
	mMat->setReceiveShadows(false);
	mMat->setLightingEnabled(false);
	mMat->getTechnique(0)->setName("Forward");
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState();
	mMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setColourOperationEx(
		Ogre::LBX_SOURCE1,
		Ogre::LBS_MANUAL,
		Ogre::LBS_CURRENT,
		color);

	return mMat;
}

MaterialPtr EditorClasses::CreateBillboardMaterial(string name, TexturePtr texture)
{
	Ogre::MaterialPtr mMat = Ogre::MaterialManager::getSingleton().create(name, "Editor");
	Ogre::Technique* mTech = mMat->createTechnique();
	Ogre::Pass* mPass = mTech->createPass();
	mPass = mMat->getTechnique(0)->getPass(0);
	mMat->setReceiveShadows(false);
	mMat->setLightingEnabled(false);
	mMat->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mMat->setDepthWriteEnabled(false);
	mMat->getTechnique(0)->setName("Forward");
	mMat->getTechnique(0)->getPass(0)->createTextureUnitState();
	mMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTexture(texture);

	return mMat;
}
