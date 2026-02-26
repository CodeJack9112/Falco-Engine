#pragma once

#include "../Engine/RaycastGeometry.h"

class EditorClasses
{
public:
	EditorClasses();
	~EditorClasses();

	//Camera operations
	static void RotateCamera(Camera* pTargetCamera, Real rOffsetX, Real rOffsetY);
	static void RotateCameraOrbit(Camera* pTargetCamera, Vector3 vOrbitCenter, Real rOffsetX, Real rOffsetY);
	static void PanningCamera(Camera* pTargetCamera, Real rOffsetX, Real rOffsetY);
	static void MoveCamera(Camera* pTargetCamera, Vector3 vDirection, Real rDistance);
	static void ZoomCamera(Camera* pTargetCamera, Real rDistance);
	static void ZoomCamera(Camera* pTargetCamera, Vector3 dir, Real rDistance);
	static void MoveCamera(Camera* pTargetCamera, Vector3 dir);

	//SceneNode operations
	static void RotateSceneNode(SceneNode* pTargetNode, Real rOffsetX, Real rOffsetY);
	static void PanningSceneNode(SceneNode* pTargetNode, Real rOffsetX, Real rOffsetY);

	//Objects operations
	static SceneNode * PlaceSceneModel(SceneManager* mSceneMgr, String path, Ogre::Vector2 point);
	static SceneNode * PlaceFBXScene(SceneManager* mSceneMgr, String path, Ogre::Vector2 point, float distance = 0.0f);
	static SceneNode * CreateLight(SceneManager* mSceneMgr, Light::LightTypes lightType);
	static SceneNode * CreateEmptyObject(SceneManager* mSceneMgr);
	static SceneNode * CreateCamera(SceneManager* mSceneMgr);
	static SceneNode * CreateUICanvas(SceneManager* mSceneMgr);
	static SceneNode * CreateUIButton(SceneManager* mSceneMgr);
	static SceneNode * CreateUIText(SceneManager* mSceneMgr);
	static SceneNode * CreateUIImage(SceneManager* mSceneMgr);
	static SceneNode * CreateParticleSystem(SceneManager* mSceneMgr);
	static BillboardSet * CreateBillboard(SceneManager * mSceneMgr, SceneNode * parentNode, string name, string materialName);

	static Vector3 GetScreenCenter(SceneManager* mSceneMgr, float distance);
	static Vector3 GetWorldPointFromScreenPoint(Ogre::Vector2 point);

	//Helpers
	static std::string CSTR2STR(CString str);
	static MaterialPtr CreateColoredMaterial(string name, ColourValue color);
	static MaterialPtr CreateBillboardMaterial(string name, TexturePtr texture);
};

#define CSTR2STRW EditorClasses::CSTR2STR