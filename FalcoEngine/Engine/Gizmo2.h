#pragma once

#include <OgreCamera.h>
#include <OgreMaterial.h>
#include <OgreSceneManager.h>
#include "ImGuizmo.h"
#include "SelectionBox.h"
#include "RaycastGeometry.h"

class Gizmo2
{
public:
	enum GizmoType
	{
		GT_SELECT,
		GT_MOVE,
		GT_ROTATE,
		GT_SCALE
	};

	enum TransformSpace
	{
		TS_LOCAL,
		TS_WORLD
	};

	enum GizmoAction
	{
		GA_MOVE,
		GA_SCALE,
		GA_ROTATE
	};

	enum CenterBase
	{
		CB_CENTER,
		CB_PIVOT
	};

	typedef std::vector<Ogre::SceneNode*> SelectedObjects;

private:
	Ogre::Camera * camera = nullptr;
	Ogre::RealRect rect;
	Ogre::RealRect clipRect;
	bool visible = false;
	bool mBoxSelecting = false;
	bool canBoxSelect = true;
	Ogre::Vector2 mStart = Ogre::Vector2(0, 0);
	Ogre::Vector2 mStop = Ogre::Vector2(0, 0);
	bool moving = false;
	bool startMoving = false;
	GizmoType gizmoType = GizmoType::GT_MOVE;
	TransformSpace transformSpace = TransformSpace::TS_WORLD;
	CenterBase centerBase = CenterBase::CB_CENTER;
	float snapToGridSize = 0.0f;
	bool snapToGrid = false;
	bool canManipulate = true;
	bool isViewportHovered = false;
	bool mbPressed = false;
	bool mbPressed2 = false;
	bool isViewGizmoUsing = false;

	SelectedObjects selectedObjects;
	Ogre::SceneNode* selectionRoot = nullptr;
	Ogre::SceneManager * sceneMgr = nullptr;
	RaycastGeometry* raycast = nullptr;
	SelectionBox* mSelectionBox = nullptr;
	Ogre::PlaneBoundedVolumeListSceneQuery* mVolQuery = nullptr;

	void(*selectCallback)(SelectedObjects nodes, void* userData) = nullptr;
	void(*actionCallback)(SelectedObjects nodes, GizmoAction action) = nullptr;
	void(*manipulateCallback)(SelectedObjects nodes) = nullptr;
	void(*manipulateStartCallback)(SelectedObjects nodes) = nullptr;
	void(*manipulateEndCallback)(SelectedObjects nodes) = nullptr;

	std::vector<Ogre::SceneNode*> wireframeNodes;
	
	void getChildrenRecursive(SceneNode* root, std::vector<SceneNode*>& _return);

	std::map<Ogre::SceneNode*, Ogre::SceneNode*> parentBuffer;
	void addToGizmo();
	void removeFromGizmo();
	void moveObject(Ogre::SceneNode* obj, Ogre::SceneNode* moveto, bool addToBuffer);
	Ogre::SceneNode * getNode(float mouseScreenX, float mouseScreenY, Ogre::uint32 mask = 1 << 1);
	void performSelection(const Ogre::Vector2& first, const Ogre::Vector2& second);
	void swap(float& x, float& y);
	void clearBoxSelection();

public:
	Gizmo2();
	~Gizmo2();

	Ogre::MaterialPtr wireframeMaterial;
	Ogre::MaterialPtr terrainBrushMaterial;
	Ogre::MaterialPtr colliderMaterial;
	Ogre::MaterialPtr hingeMaterial;
	Ogre::MaterialPtr boneMaterial;
	Ogre::MaterialPtr obstacleMaterial;
	Ogre::MaterialPtr lineMaterialWhite;
	Ogre::MaterialPtr lineMaterialYellow;

	void init(Ogre::SceneManager* mgr, Ogre::Camera* cam);
	void draw();
	void setRect(Ogre::RealRect rc) { rect = rc; }
	void setClipRect(Ogre::RealRect rc) { clipRect = rc; }
	void show();
	void hide();
	bool isVisible();

	Ogre::Vector3 findMidPoint(std::vector<Ogre::SceneNode*>& nodes);

	std::map<Ogre::SceneNode*, Ogre::SceneNode*>& getParentBuffer() { return parentBuffer; }

	void mouseMove(float x, float y);
	void mouseDown(float x, float y);
	void mouseUp(float x, float y, bool multipleSelection);

	void selectObject(Ogre::SceneNode* idObject, bool multipleSelection, bool callCallback = true, void* userData = nullptr);
	void selectObjects(SelectedObjects idObjects, void* userData = nullptr, bool callCallback = true, bool updateWireframe = true);

	void setSelectCallback(void(*callback)(SelectedObjects argument, void* userData)) { selectCallback = callback; }
	void setActionCallback(void(*callback)(SelectedObjects nodes, GizmoAction action)) { actionCallback = callback; }
	void setManipulateCallback(void(*callback)(SelectedObjects nodes)) { manipulateCallback = callback; }
	void setManipulateEndCallback(void(*callback)(SelectedObjects nodes)) { manipulateEndCallback = callback; }
	void setManipulateStartCallback(void(*callback)(SelectedObjects nodes)) { manipulateStartCallback = callback; }

	GizmoType getGizmoType() { return gizmoType; }
	TransformSpace getTransformSpace() { return transformSpace; }
	CenterBase getCenterBase() { return centerBase; }
	void setGizmoType(GizmoType type) { gizmoType = type; }
	void setTransformSpace(TransformSpace space) { transformSpace = space; }
	void setCenterBase(CenterBase value);
	void setSnapToGrid(bool snap) { snapToGrid = snap; }
	bool getSnapToGrid() { return snapToGrid; }
	void setSnapToGridSize(float size) { snapToGridSize = size; }
	float getSnapToGridSize() { return snapToGridSize; }
	void setViewportHovered(bool h) { isViewportHovered = h; }
	bool isCanManipulate() { return canManipulate; }
	void updatePosition();
	bool getIsViewGizmoUsing() { return isViewGizmoUsing; }
	SceneNode* getSelectionRootNode() { return selectionRoot; }
	void getNodesBounds(SceneNode* root, AxisAlignedBox& box);

	SelectedObjects getSelectedObjects() { return selectedObjects; }

	void showWireframe(bool show);
	void clearWireframes();
	void clearSelection();

	void callSelectCallback() {
		if (selectCallback != nullptr)
			selectCallback(getSelectedObjects(), nullptr);
	}
};