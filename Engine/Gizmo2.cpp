#include "Gizmo2.h"

#include <OgreSceneNode.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>

#include "Engine.h"
#include "Screen.h"

#include "SkeletonBone.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "FixedJoint.h"
#include "HingeJoint.h"
#include "CharacterJoint.h"
#include "NavMeshObstacle.h"
#include "Vehicle.h"

#include "Procedural/Procedural.h"
#include "DeferredShading/DeferredLightCP.h"
#include "Mathf.h"
#include "GUIDGenerator.h"
#include "SkinnedMeshDeformer.h"

using namespace Ogre;

float _matrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };

bool mbRight = false;
bool mbMiddle = false;
bool canBoxSelect = false;

Gizmo2::Gizmo2()
{
}

Gizmo2::~Gizmo2()
{
	delete raycast;

	if (mSelectionBox)
		delete mSelectionBox;
}

void Gizmo2::init(Ogre::SceneManager* mgr, Ogre::Camera* cam)
{
	camera = cam;
	sceneMgr = mgr;

	raycast = new RaycastGeometry(sceneMgr, camera);

	selectionRoot = sceneMgr->getRootSceneNode()->createChildSceneNode("SelectionRoot_gizmo");

	mSelectionBox = new SelectionBox("SelectionBox_gizmo");
	SceneNode* boxSelNode = sceneMgr->getRootSceneNode()->createChildSceneNode("SelectionBox_node_gizmo");
	boxSelNode->attachObject(mSelectionBox);
	boxSelNode->_setDerivedPosition(Vector3(0, 0, 0));
	boxSelNode->_setDerivedOrientation(Quaternion::IDENTITY);

	mVolQuery = sceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());
	wireframeNodes.resize(0);

	wireframeMaterial = MaterialManager::getSingleton().create("WireframeMaterial", "Editor");
	wireframeMaterial->setLightingEnabled(false);
	wireframeMaterial->setReceiveShadows(false);
	//wireframeMaterial->setDepthBias(1, 0);
	wireframeMaterial->removeAllTechniques();
	wireframeMaterial->createTechnique();
	wireframeMaterial->getTechnique(0)->setName("Forward");
	wireframeMaterial->getTechnique(0)->createPass();
	wireframeMaterial->getTechnique(0)->getPass(0)->setLineWidth(1.0);
	wireframeMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PolygonMode::PM_WIREFRAME);
	wireframeMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	wireframeMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	wireframeMaterial->getTechnique(0)->getPass(0)->setVertexProgram("transparent_vs");
	wireframeMaterial->getTechnique(0)->getPass(0)->setFragmentProgram("transparent_fs");
	wireframeMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(1.0, 1.0, 1.0, 0.45));
	//wireframeMaterial->setDepthWriteEnabled(false);
	//wireframeMaterial->setDepthCheckEnabled(false);

	hingeMaterial = MaterialManager::getSingleton().create("HingeJointMaterial", "Editor");
	Ogre::Technique* mTech = hingeMaterial->createTechnique();
	Ogre::Pass* mPass = mTech->createPass();
	mPass = hingeMaterial->getTechnique(0)->getPass(0);
	hingeMaterial->setReceiveShadows(false);
	hingeMaterial->setLightingEnabled(false);
	hingeMaterial->getTechnique(0)->setName("Forward");
	hingeMaterial->getTechnique(0)->getPass(0)->setPointSize(7.0);
	hingeMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	hingeMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	hingeMaterial->getTechnique(0)->getPass(0)->setVertexProgram("transparent_vs");
	hingeMaterial->getTechnique(0)->getPass(0)->setFragmentProgram("transparent_fs");
	hingeMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(1.0, 1.0, 0.0, 0.8));
	hingeMaterial->setDepthCheckEnabled(false);
	hingeMaterial->setDepthWriteEnabled(false);

	boneMaterial = MaterialManager::getSingleton().create("BoneMaterial", "Editor");
	mTech = boneMaterial->createTechnique();
	mTech->setName("Forward");
	mPass = mTech->createPass();
	mPass = boneMaterial->getTechnique(0)->getPass(0);
	boneMaterial->setReceiveShadows(false);
	boneMaterial->setLightingEnabled(false);
	boneMaterial->getTechnique(0)->getPass(0)->setPointSize(5.0);
	boneMaterial->getTechnique(0)->getPass(0)->setLineWidth(2.0);
	boneMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mPass->removeAllTextureUnitStates();
	mPass->setVertexProgram("transparent_vs");
	mPass->setFragmentProgram("transparent_fs");
	mPass->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(0.0, 0.0, 1.0, 0.8));
	boneMaterial->setDepthCheckEnabled(false);
	boneMaterial->setDepthWriteEnabled(false);

	colliderMaterial = MaterialManager::getSingleton().create("ColliderMaterial", "Editor");
	colliderMaterial->setLightingEnabled(false);
	colliderMaterial->setReceiveShadows(false);
	colliderMaterial->setDepthBias(1, 0);
	colliderMaterial->removeAllTechniques();
	colliderMaterial->createTechnique();
	colliderMaterial->getTechnique(0)->setName("Forward");
	colliderMaterial->getTechnique(0)->createPass();
	colliderMaterial->getTechnique(0)->getPass(0)->setLineWidth(1.0);
	colliderMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PolygonMode::PM_WIREFRAME);
	colliderMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	colliderMaterial->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	colliderMaterial->getTechnique(0)->getPass(0)->setVertexProgram("transparent_vs");
	colliderMaterial->getTechnique(0)->getPass(0)->setFragmentProgram("transparent_fs");
	colliderMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(0.0, 1.0, 0.0, 0.8));
	colliderMaterial->setDepthCheckEnabled(false);
	colliderMaterial->setDepthWriteEnabled(false);

	lineMaterialWhite = MaterialManager::getSingleton().create("LineMaterial_gizmo", "Editor");
	lineMaterialWhite->removeAllTechniques();
	mTech = lineMaterialWhite->createTechnique();
	mTech->setName("Forward");
	mPass = mTech->createPass();
	mPass->setVertexProgram("transparent_vs", "Editor");
	mPass->setFragmentProgram("transparent_fs", "Editor");
	mPass->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mPass->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(1.0, 1.0, 1.0, 0.8));
	lineMaterialWhite->setReceiveShadows(false);
	lineMaterialWhite->setLightingEnabled(false);
	lineMaterialWhite->setCullingMode(CullingMode::CULL_NONE);
	lineMaterialWhite->getTechnique(0)->getPass(0)->setPointSize(1.0);

	lineMaterialYellow = lineMaterialWhite->clone("LineMaterialYellow_gizmo");
	lineMaterialYellow->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(1.0, 1.0, 0.0, 0.8));

	terrainBrushMaterial = MaterialManager::getSingleton().create("TerrainBrushMaterial", "Editor");
	terrainBrushMaterial->setLightingEnabled(false);
	terrainBrushMaterial->setReceiveShadows(false);
	terrainBrushMaterial->removeAllTechniques();
	terrainBrushMaterial->createTechnique();
	terrainBrushMaterial->getTechnique(0)->setName("Forward");
	mPass = terrainBrushMaterial->getTechnique(0)->createPass();
	mPass->setLineWidth(1.5);
	mPass->setCullingMode(CullingMode::CULL_NONE);
	mPass->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mPass->removeAllTextureUnitStates();
	mPass->setVertexProgram("transparent_vs");
	mPass->setFragmentProgram("transparent_fs");
	mPass->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(1.0, 1.0, 1.0, 0.3));
	terrainBrushMaterial->setDepthCheckEnabled(false);
	terrainBrushMaterial->setDepthWriteEnabled(false);

	obstacleMaterial = MaterialManager::getSingleton().create("ObstacleMaterial", "Editor");
	obstacleMaterial->removeAllTechniques();
	mTech = obstacleMaterial->createTechnique();
	mPass = mTech->createPass();
	mPass = obstacleMaterial->getTechnique(0)->getPass(0);
	obstacleMaterial->setReceiveShadows(false);
	obstacleMaterial->setLightingEnabled(false);
	obstacleMaterial->getTechnique(0)->getPass(0)->setPointSize(5.0);
	obstacleMaterial->getTechnique(0)->getPass(0)->setLineWidth(2.0);
	obstacleMaterial->getTechnique(0)->getPass(0)->setPolygonMode(PolygonMode::PM_WIREFRAME);
	obstacleMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	mPass->removeAllTextureUnitStates();
	mPass->setVertexProgram("transparent_vs");
	mPass->setFragmentProgram("transparent_fs");
	mPass->getFragmentProgramParameters()->setNamedConstant("color", Ogre::ColourValue(0.4, 0.8, 1.0, 0.8));
	obstacleMaterial->setDepthWriteEnabled(false);
	obstacleMaterial->setDepthCheckEnabled(false);

	hide();
}

void Gizmo2::draw()
{
	ImGuizmo::BeginFrame();

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		if (!mbPressed)
		{
			mbPressed = true;
			canBoxSelect = true;

			if (ImGuizmo::IsOverView())
			{
				isViewGizmoUsing = true;
			}

			if (!isViewportHovered)
			{
				if (ImGuizmo::IsOver())
					canManipulate = false;
			}
		}

		if (selectedObjects.size() > 0)
		{
			if (ImGuizmo::IsOver())
			{
				if (!mbPressed2)
				{
					mbPressed2 = true;

					for (auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it)
					{
						SkinnedMeshDeformer::EnableAnimation(*it, false);
						SkinnedMeshDeformer::UpdateSkeleton(*it);
					}
				}
			}
		}
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		canManipulate = true;
		mbPressed = false;
		mbPressed2 = false;

		isViewGizmoUsing = false;
		canBoxSelect = false;
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		mbRight = true;

		if (mBoxSelecting)
		{
			mBoxSelecting = false;
			mSelectionBox->setVisible(false);
			mSelectionBox->clear();
		}
	}

	if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
	{
		mbMiddle = true;

		if (mBoxSelecting)
		{
			mBoxSelecting = false;
			mSelectionBox->setVisible(false);
			mSelectionBox->clear();
		}
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		mbRight = false;
		canBoxSelect = false;

		if (mBoxSelecting)
		{
			mBoxSelecting = false;
			mSelectionBox->setVisible(false);
			mSelectionBox->clear();
		}
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle))
	{
		mbMiddle = false;
		canBoxSelect = false;

		if (mBoxSelecting)
		{
			mBoxSelecting = false;
			mSelectionBox->setVisible(false);
			mSelectionBox->clear();
		}
	}

	Matrix4 view = camera->getViewMatrix();
	Matrix4 projection = camera->getProjectionMatrix();

	float _view[16];
	float _proj[16];

	_view[0] = view[0][0];
	_view[1] = view[1][0];
	_view[2] = view[2][0];
	_view[3] = view[3][0];

	_view[4] = view[0][1];
	_view[5] = view[1][1];
	_view[6] = view[2][1];
	_view[7] = view[3][1];

	_view[8] = view[0][2];
	_view[9] = view[1][2];
	_view[10] = view[2][2];
	_view[11] = view[3][2];

	_view[12] = view[0][3];
	_view[13] = view[1][3];
	_view[14] = view[2][3];
	_view[15] = view[3][3];

	_proj[0] = projection[0][0];
	_proj[1] = projection[1][0];
	_proj[2] = projection[2][0];
	_proj[3] = projection[3][0];

	_proj[4] = projection[0][1];
	_proj[5] = projection[1][1];
	_proj[6] = projection[2][1];
	_proj[7] = projection[3][1];

	_proj[8] = projection[0][2];
	_proj[9] = projection[1][2];
	_proj[10] = projection[2][2];
	_proj[11] = projection[3][2];

	_proj[12] = projection[0][3];
	_proj[13] = projection[1][3];
	_proj[14] = projection[2][3];
	_proj[15] = projection[3][3];

	ImGuizmo::OPERATION op;
	ImGuizmo::MODE md;

	switch (gizmoType)
	{
	case GizmoType::GT_SELECT:
		ImGuizmo::Enable(false);
		break;
	case GizmoType::GT_MOVE:
		op = ImGuizmo::OPERATION::TRANSLATE;
		ImGuizmo::Enable(true);
		break;
	case GizmoType::GT_ROTATE:
		op = ImGuizmo::OPERATION::ROTATE;
		ImGuizmo::Enable(true);
		break;
	case GizmoType::GT_SCALE:
		op = ImGuizmo::OPERATION::SCALE;
		ImGuizmo::Enable(true);
		break;
	default:
		op = ImGuizmo::OPERATION::TRANSLATE;
		ImGuizmo::Enable(true);
		break;
	}

	switch (transformSpace)
	{
	case TransformSpace::TS_WORLD:
		md = ImGuizmo::MODE::WORLD;
		break;
	case TransformSpace::TS_LOCAL:
		md = ImGuizmo::MODE::LOCAL;
		break;
	default:
		md = ImGuizmo::MODE::WORLD;
		break;
	}

	ImGuizmo::SetRect(rect.left, rect.top, rect.right, rect.bottom);
	ImGuizmo::SetClipRect(clipRect.left, clipRect.top, clipRect.right, clipRect.bottom);

	float snap[3] = { snapToGridSize, snapToGridSize, snapToGridSize };
	if (!snapToGrid)
	{
		snap[0] = 0;
		snap[1] = 0;
		snap[2] = 0;
	}

	if (mbRight || mbMiddle)
		ImGuizmo::Enable(false);

	if (canManipulate)
	{
		bool uiObject = false;

		if (selectedObjects.size() > 0)
		{
			if (selectedObjects[0]->getAttachedObjects().size() > 0)
			{
				MovableObject* obj = selectedObjects[0]->getAttachedObject(0);

				if (GetEngine->IsObjectUIElement(obj))
				{
					uiObject = true;

					AxisAlignedBox aab = selectedObjects[0]->getAttachedObject(0)->getBoundingBox();
					
					Vector3 _min = Vector3::ZERO;
					Vector3 _max = Vector3::ZERO;

					if (selectionRoot->getChildren().size() > 0)
					{
						_min = aab.getMinimum() * parentBuffer[selectedObjects[0]]->_getDerivedScale();
						_max = aab.getMaximum() * parentBuffer[selectedObjects[0]]->_getDerivedScale();
					}
					else
					{
						_min = aab.getMinimum() * selectedObjects[0]->getParent()->_getDerivedScale();
						_max = aab.getMaximum() * selectedObjects[0]->getParent()->_getDerivedScale();
					}
					
					float box[6] = { _min.x, _min.y, _min.z, _max.x, _max.y, _max.z };

					ImGuizmo::Manipulate(_view, _proj, op, md, _matrix, nullptr, snap, box);
				}
			}
		}
		
		if (!uiObject)
		{
			ImGuizmo::Manipulate(_view, _proj, op, md, _matrix, nullptr, snap);
		}

		if (ImGuizmo::IsUsing())
		{
			if (mBoxSelecting)
			{
				mBoxSelecting = false;
				mSelectionBox->setVisible(false);
				mSelectionBox->clear();
			}

			float _translation[3] = { 0, 0, 0 };
			float _rotation[3] = { 0, 0, 0 };
			float _scale[3] = { 1, 1, 1 };

			ImGuizmo::DecomposeMatrixToComponents(_matrix, _translation, _rotation, _scale);

			selectionRoot->_setDerivedPosition(Vector3(_translation));
			
			if (transformSpace == TransformSpace::TS_LOCAL)
				selectionRoot->setOrientation(Mathf::toQuaternion(Vector3(_rotation[0],_rotation[1], _rotation[2])));
			else
				selectionRoot->_setDerivedOrientation(Mathf::toQuaternion(Vector3(_rotation[0], _rotation[1], _rotation[2])));

			selectionRoot->setScale(Vector3(_scale));
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			updatePosition();
	}

	float dist = 25.0f;

	if (selectedObjects.size() > 0)
	{
		Vector3 p = selectionRoot->_getDerivedPosition();
		dist = p.distance(camera->getDerivedPosition());
	}

	ImGuizmo::ViewManipulate(_view, dist, ImVec2(rect.left + rect.right - 105, rect.top + 32), ImVec2(100, 100), 0x00000000);

	if (!mbRight && !mbMiddle)
	{
		Matrix4 vw = Matrix4(_view);
		Quaternion camRt = vw.extractQuaternion();
		vw = vw.inverse();
		Vector3 camPs = Vector3(vw[3][0], vw[3][1], vw[3][2]);
		camera->getParentSceneNode()->_setDerivedOrientation(camRt);
		camera->getParentSceneNode()->_setDerivedPosition(camPs);
	}
}

void Gizmo2::show()
{
	ImGuizmo::Enable(true);
	visible = true;

	updatePosition();
}

void Gizmo2::hide()
{
	showWireframe(false);
	ImGuizmo::Enable(false);
	visible = false;

	updatePosition();
}

bool Gizmo2::isVisible()
{
	return visible;
}

void Gizmo2::mouseDown(float x, float y)
{
	mStart.x = x / Screen::GetScreenSize(camera->getViewport()).x;
	mStart.y = y / Screen::GetScreenSize(camera->getViewport()).y;
}

void Gizmo2::mouseUp(float x, float y, bool multipleSelection)
{
	if (!ImGuizmo::IsUsing() && !isViewGizmoUsing && !mBoxSelecting && !mbRight && !mbMiddle && canBoxSelect)
	{
		SceneNode * nodeM = getNode(x, y);

		if (nodeM != NULL)
		{
			if (!nodeM->getName().empty())
			{
				show();

				bool _prefab = nodeM->prefabGuid != "" || nodeM->prefabName != "";
				if (!_prefab)
				{
					SceneNode* _parent = nodeM->getParentSceneNode();
					_prefab = _parent->prefabGuid != "" || _parent->prefabName != "";

					while (_prefab == false && _parent != nullptr)
					{
						_parent = _parent->getParentSceneNode();
						if (_parent != nullptr)
							_prefab = _parent->prefabGuid != "" || _parent->prefabName != "";
					}

					if (_prefab)
					{
						auto it = find(selectedObjects.begin(), selectedObjects.end(), _parent);
						if (it == selectedObjects.end())
							nodeM = _parent;
					}
				}

				selectObject(nodeM, multipleSelection);
			}
		}
		else
		{
			if (!multipleSelection)
			{
				showWireframe(false);

				selectedObjects.clear();
				selectCallback(selectedObjects, nullptr);

				hide();
			}
		}
	}

	if (mBoxSelecting)
	{
		if (!multipleSelection)
		{
			selectedObjects.clear();
		}

		performSelection(mStart, mStop);
		mBoxSelecting = false;
		mSelectionBox->setVisible(false);
		mSelectionBox->clear();

		if (selectedObjects.size() > 0)
		{
			show();
			selectCallback(selectedObjects, nullptr);
		}
		else
		{
			if (!multipleSelection)
			{
				showWireframe(false);

				selectedObjects.clear();
				selectCallback(selectedObjects, nullptr);

				hide();
			}
		}
	}

	if (moving)
	{
		moving = false;
		startMoving = false;

		removeFromGizmo();

		DeferredLightRenderOperation::UpdateStaticShadowmaps();
	}

	if (ImGuizmo::IsUsing())
	{
		if (manipulateCallback != nullptr)
			manipulateCallback(selectedObjects);

		if (manipulateEndCallback != nullptr)
			manipulateEndCallback(selectedObjects);
	}

	mSelectionBox->setVisible(false);
	mSelectionBox->clear();
}

void Gizmo2::mouseMove(float x, float y)
{
	if (mbPressed)
	{
		if (!ImGuizmo::IsOver() && !isViewGizmoUsing && mBoxSelecting == false && canBoxSelect && !mbRight && !mbMiddle)
		{
			mStop.x = x / Screen::GetScreenSize(camera->getViewport()).x;
			mStop.y = y / Screen::GetScreenSize(camera->getViewport()).y;

			mSelectionBox->setCorners(mStart, mStop);

			mBoxSelecting = true;
			mSelectionBox->clear();
			mSelectionBox->setVisible(true);
		}

		if (mBoxSelecting && canBoxSelect)
		{
			mStop.x = x / Screen::GetScreenSize(camera->getViewport()).x;
			mStop.y = y / Screen::GetScreenSize(camera->getViewport()).y;

			mSelectionBox->setCorners(mStart, mStop);
		}

		if (selectedObjects.size() > 0 && ImGuizmo::IsUsing())
		{
			moving = true;

			if (!startMoving)
			{
				startMoving = true;

				if (manipulateStartCallback != nullptr)
					manipulateStartCallback(selectedObjects);

				addToGizmo();
			}
		}

		if (ImGuizmo::IsUsing())
		{
			if (manipulateCallback != nullptr)
				manipulateCallback(selectedObjects);

			if (selectedObjects.size() > 0)
			{
				for (auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it)
					SkinnedMeshDeformer::UpdateSkeleton(*it, true, parentBuffer[*it]);
			}
		}
	}
}

void Gizmo2::getNodesBounds(SceneNode* root, AxisAlignedBox& box)
{
	if (!GetEngine->IsEditorObject(root))
	{
		MovableObject* obj = root->getAttachedObject(0);

		if (obj->getMovableType() == EmptyObjectFactory::FACTORY_TYPE_NAME
			|| obj->getMovableType() == "Camera"
			|| obj->getMovableType() == LightFactory::FACTORY_TYPE_NAME
			|| obj->getMovableType() == ParticleSystemFactory::FACTORY_TYPE_NAME)
		{
			box.merge(root->_getDerivedPosition());
		}
		else
		{
			box.merge(obj->getWorldBoundingBox());
		}

		Node::ChildNodeIterator it = root->getChildIterator();
		while (it.hasMoreElements())
		{
			SceneNode* child = (SceneNode*)it.getNext();

			getNodesBounds(child, box);
		}
	}
}

Vector3 Gizmo2::findMidPoint(std::vector<SceneNode*>& nodes)
{
	Vector3 barycenter = Vector3::ZERO;

	if (centerBase == CenterBase::CB_PIVOT)
	{
		if (nodes.size() > 0)
		{
			for (std::vector<SceneNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
			{
				barycenter += (*it)->_getDerivedPosition();
			}

			barycenter *= pow(nodes.size(), -1);
		}
	}
	else
	{
		if (nodes.size() > 0)
		{
			SceneNode* first = nodes[0];

			AxisAlignedBox box;
			if (first->getAttachedObject(0)->getMovableType() != EntityFactory::FACTORY_TYPE_NAME)
				box.setExtents(first->_getDerivedPosition(), first->_getDerivedPosition());

			for (auto it = nodes.begin(); it != nodes.end(); ++it)
			{
				SceneNode* node = *it;
				getNodesBounds(node, box);
			}

			Vector3 sz = box.getSize();
			float mx = 0;
			mx = max(mx, sz.x);
			mx = max(mx, sz.y);
			mx = max(mx, sz.z);

			barycenter = box.getCenter();
		}
	}

	return barycenter;
}

void Gizmo2::addToGizmo()
{
	for (std::vector<SceneNode*>::iterator _it = selectedObjects.begin(); _it != selectedObjects.end(); ++_it)
	{
		SceneNode* _node = *_it;

		moveObject(_node, selectionRoot, true);
	}
}

void Gizmo2::removeFromGizmo()
{
	VectorIterator it = selectionRoot->getChildIterator();
	std::vector<SceneNode*> _temp;

	while (it.hasMoreElements())
	{
		SceneNode* _node = (SceneNode*)it.getNext();
		_temp.push_back(_node);
	}

	for (std::vector<SceneNode*>::iterator _it = _temp.begin(); _it != _temp.end(); ++_it)
	{
		SceneNode* _node = *_it;
		moveObject(_node, parentBuffer[_node], false);
		parentBuffer.erase(_node);
	}

	for (std::vector<SceneNode*>::iterator _it = _temp.begin(); _it != _temp.end(); ++_it)
	{
		SceneNode* _node = *_it;
		_node = _node->getParentSceneNode();

		std::sort(_node->getChildrenList().begin(), _node->getChildrenList().end(), [=](Node*& a, Node*& b) -> bool {
			return a->index < b->index;
		});
	}

	_temp.clear();
}

void Gizmo2::moveObject(Ogre::SceneNode* obj, Ogre::SceneNode* moveto, bool addToBuffer)
{
	if (addToBuffer)
		parentBuffer[obj] = obj->getParentSceneNode();

	Vector3 pos = obj->_getDerivedPosition();
	Vector3 scl = obj->getScale();
	Quaternion rot = obj->_getDerivedOrientation();

	SceneNode* parent = obj->getParentSceneNode();

	scl *= parent->_getDerivedScale();
	scl /= moveto->_getDerivedScale();

	parent->removeChild(obj);
	moveto->addChild(obj);

	obj->_setDerivedPosition(pos);
	obj->setScale(scl);
	obj->_setDerivedOrientation(rot);
}

Ogre::SceneNode* Gizmo2::getNode(float mouseScreenX, float mouseScreenY, Ogre::uint32 mask)
{
	float width = (float)this->camera->getViewport()->getActualWidth(); // viewport width
	float height = (float)this->camera->getViewport()->getActualHeight(); // viewport height

	Ray ray = this->camera->getCameraToViewportRay((float)mouseScreenX / width, (float)mouseScreenY / height);

	MovableObject* entity = NULL;

	//Initial distance must be -1.0 at start
	float distance = -1.0;

	raycast->SetMask(mask);

	entity = raycast->RaycastFromPoint(Vector2(mouseScreenX, mouseScreenY), distance);

	if (entity != nullptr)
		return entity->getParentSceneNode();
	else
		return nullptr;
}

void Gizmo2::performSelection(const Ogre::Vector2& first, const Ogre::Vector2& second)
{
	float left = first.x, right = second.x;
	float top = first.y, bottom = second.y;

	if (left > right)
		swap(left, right);

	if (top > bottom)
		swap(top, bottom);

	if ((right - left) * (bottom - top) < 0.0001)
		return;

	Ogre::Ray topLeft = camera->getCameraToViewportRay(left, top);
	Ogre::Ray topRight = camera->getCameraToViewportRay(right, top);
	Ogre::Ray bottomLeft = camera->getCameraToViewportRay(left, bottom);
	Ogre::Ray bottomRight = camera->getCameraToViewportRay(right, bottom);

	Ogre::Plane frontPlane, topPlane, leftPlane, bottomPlane, rightPlane;

	frontPlane = Ogre::Plane(
		topLeft.getOrigin(),
		topRight.getOrigin(),
		bottomRight.getOrigin());

	topPlane = Ogre::Plane(
		topLeft.getOrigin(),
		topLeft.getPoint(10),
		topRight.getPoint(10));

	leftPlane = Ogre::Plane(
		topLeft.getOrigin(),
		bottomLeft.getPoint(10),
		topLeft.getPoint(10));

	bottomPlane = Ogre::Plane(
		bottomLeft.getOrigin(),
		bottomRight.getPoint(10),
		bottomLeft.getPoint(10));

	rightPlane = Ogre::Plane(
		topRight.getOrigin(),
		topRight.getPoint(10),
		bottomRight.getPoint(10));

	Ogre::PlaneBoundedVolume vol;

	vol.planes.push_back(frontPlane);
	vol.planes.push_back(topPlane);
	vol.planes.push_back(leftPlane);
	vol.planes.push_back(bottomPlane);
	vol.planes.push_back(rightPlane);

	Ogre::PlaneBoundedVolumeList volList;
	volList.push_back(vol);

	mVolQuery->setVolumes(volList);
	Ogre::SceneQueryResult result = mVolQuery->execute();

	Ogre::SceneQueryResultMovableList::iterator it;

	for (it = result.movables.begin(); it != result.movables.end(); ++it)
	{
		if (!(*it)->isAttached())
			continue;

		if ((*it)->getParentSceneNode()->getName().empty())
			continue;

		if (GetEngine->IsEditorObject((*it)->getParentSceneNode()))
			continue;

		if ((*it)->getMovableType() != EmptyObjectFactory::FACTORY_TYPE_NAME && (*it)->getMovableType() != "Camera")
		{
			SceneNode* nodeM = (*it)->getParentSceneNode();

			if (nodeM == nullptr)
				continue;

			bool _prefab = nodeM->prefabGuid != "" || nodeM->prefabName != "";
			if (!_prefab)
			{
				SceneNode* _parent = nodeM->getParentSceneNode();
				_prefab = _parent->prefabGuid != "" || _parent->prefabName != "";

				while (_prefab == false && _parent != nullptr)
				{
					_parent = _parent->getParentSceneNode();
					if (_parent != nullptr)
						_prefab = _parent->prefabGuid != "" || _parent->prefabName != "";
				}

				if (_prefab)
				{
					nodeM = _parent;
				}
			}

			selectObject(nodeM, true, false);
		}
	}
}

void Gizmo2::swap(float& x, float& y)
{
	float temp = x;
	x = y;
	y = temp;
}

void Gizmo2::selectObject(SceneNode* idObject, bool multipleSelection, bool callCallback, void* userData)
{
	if (!multipleSelection)
		selectedObjects.clear();

	if (idObject != NULL)
	{
		if (idObject->getName().find("_gizmo") == string::npos)
		{
			SelectedObjects::iterator _ext = std::find(selectedObjects.begin(), selectedObjects.end(), idObject);
			if (_ext == selectedObjects.end())
				selectedObjects.push_back(idObject);
			else
				selectedObjects.erase(_ext);
		}

		showWireframe(true);
	}
	else
	{
		if (!multipleSelection)
		{
			showWireframe(false);
			selectedObjects.clear();
		}
	}

	updatePosition();

	if (callCallback)
	{
		if (selectedObjects.size() > 0)
		{
			show();
			selectCallback(selectedObjects, userData);
		}
		else
		{
			hide();
			selectCallback(selectedObjects, userData);
		}
	}
}

void Gizmo2::selectObjects(SelectedObjects idObjects, void* userData, bool callCallback, bool updateWireframe)
{
	selectedObjects.clear();

	for (SelectedObjects::iterator it = idObjects.begin(); it != idObjects.end(); ++it)
	{
		if ((*it)->getName().find("_gizmo") == string::npos)
			selectedObjects.push_back(*it);
	}

	updatePosition();

	if (selectedObjects.size() > 0)
	{
		show();

		if (callCallback)
			selectCallback(selectedObjects, userData);

		if (updateWireframe)
			showWireframe(true);
	}
	else
	{
		hide();

		if (callCallback)
			selectCallback(selectedObjects, userData);

		if (updateWireframe)
			showWireframe(false);
	}
}

void Gizmo2::clearSelection()
{
	showWireframe(false);
	selectObject(nullptr, false, false);
	hide();
}

void Gizmo2::clearWireframes()
{
	wireframeNodes.clear();
}

void Gizmo2::setCenterBase(CenterBase value)
{
	centerBase = value;
	selectObjects(selectedObjects, nullptr, false);
}

void Gizmo2::updatePosition()
{
	if (selectedObjects.size() > 1)
	{
		Vector3 barycenter = findMidPoint(selectedObjects);

		selectionRoot->_setDerivedPosition(barycenter);
		selectionRoot->_setDerivedOrientation(Quaternion::IDENTITY);
		selectionRoot->setScale(Vector3(1, 1, 1));
	}
	else if (selectedObjects.size() == 1)
	{
		Vector3 barycenter = findMidPoint(selectedObjects);

		//selectionRoot->_setDerivedPosition(selectedObjects[0]->_getDerivedPosition());
		selectionRoot->_setDerivedPosition(barycenter);
		selectionRoot->_setDerivedOrientation(selectedObjects[0]->_getDerivedOrientation());
		selectionRoot->setScale(selectedObjects[0]->getScale());
	}

	if (isVisible())
	{
		Vector3 pos = selectionRoot->_getDerivedPosition();
		Vector3 rot = Mathf::toEuler(selectionRoot->_getDerivedOrientation());
		Vector3 scl = selectionRoot->getScale();

		if (transformSpace == TransformSpace::TS_LOCAL)
		{
			rot = Mathf::toEuler(selectionRoot->getOrientation());
		}

		ImGuizmo::RecomposeMatrixFromComponents(pos.ptr(), rot.ptr(), scl.ptr(), _matrix);
	}
	else
	{
		//ImGuizmo::SetRect(0, 0, 0, 0);

		Vector3 pos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		Vector3 rot = Vector3(0, 0, 0);
		Vector3 scl = Vector3(1, 1, 1);

		ImGuizmo::RecomposeMatrixFromComponents(pos.ptr(), rot.ptr(), scl.ptr(), _matrix);
	}
}

void Gizmo2::getChildrenRecursive(SceneNode* root, std::vector<SceneNode*>& _return)
{
	if (std::find(_return.begin(), _return.end(), root) == _return.end())
	{
		if (root->getName().find("_gizmo") == string::npos)
			_return.push_back(root);
	}

	VectorIterator childIt = root->getChildIterator();

	while (childIt.hasMoreElements())
	{
		SceneNode* currChild = (SceneNode*)childIt.getNext();
		getChildrenRecursive(currChild, _return);
	}
}

void createBonesRecursive(SceneNode* rootRoot, SceneNode* root, ManualObject* linesObj)
{
	VectorIterator childIt = root->getChildIterator();

	while (childIt.hasMoreElements())
	{
		SceneNode* currChild = (SceneNode*)childIt.getNext();

		SkeletonBone* bone = (SkeletonBone*)currChild->GetComponent(SkeletonBone::COMPONENT_TYPE);

		if (bone != nullptr)
		{
			Vector3 pos0 = Mathf::inverseTransformPoint(rootRoot, root->_getDerivedPosition());
			Vector3 pos1 = Mathf::inverseTransformPoint(rootRoot, currChild->_getDerivedPosition());

			linesObj->position(pos0);
			linesObj->position(pos1);
		}

		createBonesRecursive(rootRoot, currChild, linesObj);
	}
}

void Gizmo2::showWireframe(bool show)
{
	//Remove wireframes
	for (std::vector<SceneNode*>::iterator wit = wireframeNodes.begin(); wit != wireframeNodes.end(); ++wit)
	{
		SceneNode* wireframeNode = *wit;
		Node* parent = wireframeNode->getParent();

		if (parent != nullptr)
		{
			parent->removeChild(wireframeNode);
		}

		VectorIterator childs = wireframeNode->getChildIterator();
		while (childs.hasMoreElements())
		{
			SceneNode* chld = (SceneNode*)childs.getNext();
			SceneNode::ObjectIterator itObject = chld->getAttachedObjectIterator();

			std::vector<MovableObject*> attachedObjs;

			while (itObject.hasMoreElements())
			{
				MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());

				if (pObject->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					if (((Entity*)pObject)->getMesh() != nullptr)
					{
						MeshPtr m = ((Entity*)pObject)->getMesh();
						m->unload();
						MeshManager::getSingleton().remove(m->getHandle());
					}
				}

				attachedObjs.push_back(pObject);
			}

			for (std::vector<MovableObject*>::iterator __it = attachedObjs.begin(); __it != attachedObjs.end(); ++__it)
			{
				chld->detachObject(*__it);
				sceneMgr->destroyMovableObject(*__it);
			}

			attachedObjs.clear();
		}

		Engine::Destroy(wireframeNode);

		wireframeNode = nullptr;
	}

	wireframeNodes.clear();

	//Add wireframes
	if (show)
	{
		if (selectedObjects.size() == 0)
			return;

		for (SelectedObjects::iterator cit = selectedObjects.begin(); cit != selectedObjects.end(); ++cit)
		{
			SceneNode* currNode = *cit;

			std::vector<SceneNode*> wireArray;

			getChildrenRecursive(currNode, wireArray);

			for (std::vector<SceneNode*>::iterator wrIt = wireArray.begin(); wrIt != wireArray.end(); ++wrIt)
			{
				SceneNode* currNodeWr = *wrIt;

				if (!currNodeWr->getVisible())
					continue;

				std::string __name = "WireframeObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo";

				if (sceneMgr->hasSceneNode(__name))
					continue;

				SceneNode* wireframeNode = currNodeWr->createChildSceneNode(__name);
				wireframeNodes.push_back(wireframeNode);

				if (currNodeWr->getAttachedObjects().size() > 0)
				{
					MovableObject* movObj = currNodeWr->getAttachedObject(0);

					//Entity
					if (movObj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
					{
						Entity* entity = (Entity*)movObj;

						ManualObject* wireframe = new ManualObject("WireframeObject_mesh_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
						wireframe->setCastShadows(false);
						wireframe->setQueryFlags(1 << 3);
						wireframeNode->attachObject(wireframe);

						size_t vertex_count;
						size_t index_count;
						Ogre::Vector3* vertices;
						unsigned long* indices;

						// get the mesh information
						RaycastGeometry::GetMeshInformation(entity->getMesh(), vertex_count, vertices, index_count, indices,
							Vector3::ZERO,
							Quaternion::IDENTITY,
							Vector3(1, 1, 1));

						//Build wireframed mesh
						wireframe->begin("WireframeMaterial", RenderOperation::OperationType::OT_TRIANGLE_LIST, "Editor");

						for (int i = 0; i < static_cast<int>(index_count); i += 3)
						{
							// check for a hit against this triangle
							wireframe->position(vertices[indices[i]]);
							wireframe->position(vertices[indices[i + 1]]);
							wireframe->position(vertices[indices[i + 2]]);
						}

						wireframe->end();

						delete[] vertices;
						delete[] indices;
					}

					//Light
					if (movObj->getMovableType() == LightFactory::FACTORY_TYPE_NAME)
					{
						Light* light = (Light*)movObj;

						if (light->getType() == Light::LT_POINT)
						{
							ManualObject* circle = new ManualObject("WireframeObject_pointLight_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							circle->setCastShadows(false);

							float const radius = light->getAttenuationRange();
							float accuracy = 35;

							circle->begin("LineMaterialYellow_gizmo", RenderOperation::OT_LINE_LIST, "Editor");
							Vector3 _scale = wireframeNode->_getDerivedScale();

							unsigned point_index = 0;
							for (float theta = 0; theta <= 2 * Math::PI; theta += Math::PI / accuracy)
							{
								circle->position(Vector3(radius * cos(theta), 0, radius * sin(theta)) / _scale);
								circle->position(Vector3(radius * cos(theta - Math::PI / accuracy), 0, radius * sin(theta - Math::PI / accuracy)) / _scale);

								circle->position(Vector3(0, radius * cos(theta), radius * sin(theta)) / _scale);
								circle->position(Vector3(0, radius * cos(theta - Math::PI / accuracy), radius * sin(theta - Math::PI / accuracy)) / _scale);

								circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
								circle->position(Vector3(radius * cos(theta - Math::PI / accuracy), radius * sin(theta - Math::PI / accuracy), 0) / _scale);
							}

							circle->end();

							wireframeNode->attachObject(circle);
						}

						if (light->getType() == Light::LT_SPOTLIGHT)
						{
							ManualObject* circle = new ManualObject("WireframeObject_spotLight_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							circle->setCastShadows(false);

							float const range = light->getAttenuationRange();
							float const radius = light->getSpotlightOuterAngle().valueDegrees();
							float accuracy = 35;

							circle->begin("LineMaterialYellow_gizmo", RenderOperation::OT_LINE_LIST, "Editor");
							Vector3 _scale = wireframeNode->_getDerivedScale();

							unsigned point_index = 0;
							for (float theta = 0; theta <= 2 * Math::PI; theta += Math::PI / accuracy)
							{
								circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);
								circle->position(Vector3(radius * cos(theta - Math::PI / accuracy), radius * sin(theta - Math::PI / accuracy), -range) / _scale);
							}

							float theta = -Math::PI / 2;
							circle->position(Vector3(0, 0, 0));
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = 0;
							circle->position(Vector3(0, 0, 0));
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = Math::PI / 2;
							circle->position(Vector3(0, 0, 0));
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = Math::PI;
							circle->position(Vector3(0, 0, 0));
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							circle->end();

							wireframeNode->attachObject(circle);
						}

						if (light->getType() == Light::LT_DIRECTIONAL)
						{
							ManualObject* circle = new ManualObject("WireframeObject_spotLight_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							circle->setCastShadows(false);

							float const range = 10;
							float const radius = 2.5;
							float accuracy = 15;

							circle->begin("LineMaterialYellow_gizmo", RenderOperation::OT_LINE_LIST, "Editor");
							Vector3 _scale = wireframeNode->_getDerivedScale();

							unsigned point_index = 0;
							for (float theta = 0; theta <= 2 * Math::PI; theta += Math::PI / accuracy)
							{
								circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
								circle->position(Vector3(radius * cos(theta - Math::PI / accuracy), radius * sin(theta - Math::PI / accuracy), 0) / _scale);

								//circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);
								//circle->position(Vector3(radius * cos(theta - Math::PI / accuracy), radius * sin(theta - Math::PI / accuracy), -range) / _scale);
							}

							float theta = -Math::PI / 2;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = 0;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = Math::PI / 2;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = Math::PI;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							/////////////////

							theta = -Math::PI / 4;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = Math::PI / 4;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = Math::PI / 2 + Math::PI / 4;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							theta = -Math::PI / 2 - Math::PI / 4;
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), 0) / _scale);
							circle->position(Vector3(radius * cos(theta), radius * sin(theta), -range) / _scale);

							circle->end();

							wireframeNode->attachObject(circle);
						}
					}

					//Camera
					if (movObj->getMovableType() == "Camera")
					{
						Camera* camera = (Camera*)movObj;

						ManualObject* frustum = new ManualObject("WireframeObject_camera_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
						frustum->setCastShadows(false);

						frustum->begin("LineMaterial_gizmo", RenderOperation::OT_LINE_LIST, "Editor");
						Vector3 _scale = wireframeNode->_getDerivedScale();

						float ncd = camera->getNearClipDistance();
						float fcd = camera->getFarClipDistance();

						Vector3 camForward, camRight, camUp;
						camForward = Vector3(0, 0, 1);
						camRight = Vector3(1, 0, 0);
						camUp = Vector3(0, 1, 0);

						Vector3 nearCenter = -camForward * ncd;
						Vector3 farCenter = -camForward * fcd;

						float w = Screen::GetScreenSize().x;
						float h = Screen::GetScreenSize().y;
						float viewRatio = w / h;

						float nearHeight = 2 * tan(camera->getFOVy().valueRadians() / 2) * ncd;
						float farHeight = 2 * tan(camera->getFOVy().valueRadians() / 2) * fcd;
						float nearWidth = nearHeight * viewRatio;
						float farWidth = farHeight * viewRatio;

						Vector3 farTopLeft = farCenter + camUp * (farHeight * 0.5) - camRight * (farWidth * 0.5);
						Vector3 farTopRight = farCenter + camUp * (farHeight * 0.5) + camRight * (farWidth * 0.5);
						Vector3 farBottomLeft = farCenter - camUp * (farHeight * 0.5) - camRight * (farWidth * 0.5);
						Vector3 farBottomRight = farCenter - camUp * (farHeight * 0.5) + camRight * (farWidth * 0.5);

						Vector3 nearTopLeft = nearCenter + camUp * (nearHeight * 0.5) - camRight * (nearWidth * 0.5);
						Vector3 nearTopRight = nearCenter + camUp * (nearHeight * 0.5) + camRight * (nearWidth * 0.5);
						Vector3 nearBottomLeft = nearCenter - camUp * (nearHeight * 0.5) - camRight * (nearWidth * 0.5);
						Vector3 nearBottomRight = nearCenter - camUp * (nearHeight * 0.5) + camRight * (nearWidth * 0.5);

						//Near frustum plane
						frustum->position(nearBottomLeft / _scale);
						frustum->position(nearTopLeft / _scale);
						frustum->position(nearTopLeft / _scale);
						frustum->position(nearTopRight / _scale);
						frustum->position(nearTopRight / _scale);
						frustum->position(nearBottomRight / _scale);
						frustum->position(nearBottomRight / _scale);
						frustum->position(nearBottomLeft / _scale);

						//Far frustum plane
						frustum->position(farBottomLeft / _scale);
						frustum->position(farTopLeft / _scale);
						frustum->position(farTopLeft / _scale);
						frustum->position(farTopRight / _scale);
						frustum->position(farTopRight / _scale);
						frustum->position(farBottomRight / _scale);
						frustum->position(farBottomRight / _scale);
						frustum->position(farBottomLeft / _scale);

						//Corner lines
						frustum->position(nearBottomLeft / _scale);
						frustum->position(farBottomLeft / _scale);
						frustum->position(nearTopLeft / _scale);
						frustum->position(farTopLeft / _scale);
						frustum->position(nearBottomRight / _scale);
						frustum->position(farBottomRight / _scale);
						frustum->position(nearTopRight / _scale);
						frustum->position(farTopRight / _scale);

						frustum->end();

						wireframeNode->attachObject(frustum);
					}
				}

				//Component's gizmos
				std::vector<Component*> components = currNodeWr->components;
				Vector3 scale = currNodeWr->_getDerivedScale();

				for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
				{
					//Capsule collider
					if ((*it)->GetComponentTypeName() == CapsuleCollider::COMPONENT_TYPE)
					{
						SceneNode* capsuleNode = wireframeNode->createChildSceneNode("capsuleCollider_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
						CapsuleCollider* capsuleCollider = (CapsuleCollider*)(*it);

						MeshPtr capsuleMesh = Procedural::CapsuleGenerator().setHeight(capsuleCollider->GetHeight()).setRadius(capsuleCollider->GetRadius()).realizeMesh("CapsuleColliderObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh");
						Entity* capsuleEntity = sceneMgr->createEntity("CapsuleColliderObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh", capsuleMesh);
						capsuleEntity->setMaterial(colliderMaterial);
						capsuleEntity->setQueryFlags(1 << 3);
						capsuleEntity->setCastShadows(false);
						capsuleEntity->setRenderQueueGroup(RENDER_QUEUE_9 - 2);

						capsuleNode->attachObject(capsuleEntity);

						capsuleNode->setPosition(capsuleCollider->GetOffset());
						capsuleNode->setOrientation(capsuleCollider->GetRotation());
					}

					//Box collider
					if ((*it)->GetComponentTypeName() == BoxCollider::COMPONENT_TYPE)
					{
						SceneNode* boxNode = wireframeNode->createChildSceneNode("boxCollider_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
						BoxCollider* boxCollider = (BoxCollider*)(*it);

						MeshPtr boxMesh = Procedural::BoxGenerator(boxCollider->GetBoxSize().x, boxCollider->GetBoxSize().y, boxCollider->GetBoxSize().z).realizeMesh("BoxColliderObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh");
						Entity* boxEntity = sceneMgr->createEntity("BoxColliderObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh", boxMesh);
						boxEntity->setMaterial(colliderMaterial);
						boxEntity->setQueryFlags(1 << 3);
						boxEntity->setCastShadows(false);
						boxEntity->setRenderQueueGroup(RENDER_QUEUE_9 - 2);

						boxNode->attachObject(boxEntity);

						boxNode->setScale(2, 2, 2);
						boxNode->setPosition(boxCollider->GetOffset());
						boxNode->setOrientation(boxCollider->GetRotation());
					}

					//Sphere collider
					if ((*it)->GetComponentTypeName() == SphereCollider::COMPONENT_TYPE)
					{
						SceneNode* sphereNode = wireframeNode->createChildSceneNode("sphereCollider_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
						SphereCollider* sphereCollider = (SphereCollider*)(*it);

						MeshPtr sphereMesh = Procedural::SphereGenerator(sphereCollider->GetRadius()).realizeMesh("SphereColliderObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh");
						Entity* sphereEntity = sceneMgr->createEntity("SphereColliderObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh", sphereMesh);
						sphereEntity->setMaterial(colliderMaterial);
						sphereEntity->setQueryFlags(1 << 3);
						sphereEntity->setCastShadows(false);
						sphereEntity->setRenderQueueGroup(RENDER_QUEUE_9 - 2);

						sphereNode->attachObject(sphereEntity);

						sphereNode->setPosition(sphereCollider->GetOffset());
					}

					//Fixed joint
					if ((*it)->GetComponentTypeName() == FixedJoint::COMPONENT_TYPE)
					{
						FixedJoint* joint = (FixedJoint*)(*it);

						SceneNode* connectedNode = nullptr;
						connectedNode = sceneMgr->getSceneNodeFast(joint->GetConnectedObjectName());

						if (connectedNode != nullptr)
						{
							ManualObject* lines1 = new ManualObject("fixedJointObj1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines1->setCastShadows(false);
							lines1->setQueryFlags(1 << 3);
							lines1->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
							lines1->position(joint->GetConnectedAnchor()/* / connectedNode->_getDerivedScale()*/);
							lines1->end();
							lines1->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
							SceneNode* linesNode1 = wireframeNode->createChildSceneNode("fixedJoint1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							linesNode1->_setDerivedPosition(connectedNode->_getDerivedPosition());
							linesNode1->_setDerivedOrientation(connectedNode->_getDerivedOrientation());
							linesNode1->attachObject(lines1);

							ManualObject* lines2 = new ManualObject("fixedJointObj2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines2->setCastShadows(false);
							lines2->setQueryFlags(1 << 3);
							lines2->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
							lines2->position(joint->GetAnchor());
							lines2->end();
							lines2->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
							SceneNode* linesNode2 = wireframeNode->createChildSceneNode("fixedJoint2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							linesNode2->attachObject(lines2);
						}
					}

					//Hinge joint
					if ((*it)->GetComponentTypeName() == HingeJoint::COMPONENT_TYPE)
					{
						HingeJoint* joint = (HingeJoint*)(*it);

						SceneNode* connectedNode = nullptr;
						connectedNode = sceneMgr->getSceneNodeFast(joint->GetConnectedObjectName());

						if (connectedNode != nullptr)
						{
							ManualObject* lines1 = new ManualObject("hingeJointObj1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines1->setCastShadows(false);
							lines1->setQueryFlags(1 << 3);
							lines1->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
							lines1->position(joint->GetConnectedAnchor());
							lines1->end();
							lines1->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
							SceneNode* linesNode1 = wireframeNode->createChildSceneNode("hingeJoint1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							linesNode1->_setDerivedPosition(connectedNode->_getDerivedPosition());
							linesNode1->_setDerivedOrientation(connectedNode->_getDerivedOrientation());
							linesNode1->attachObject(lines1);

							ManualObject* lines2 = new ManualObject("hingeJointObj2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines2->setCastShadows(false);
							lines2->setQueryFlags(1 << 3);
							lines2->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
							lines2->position(joint->GetAnchor());
							lines2->end();
							lines2->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
							SceneNode* linesNode2 = wireframeNode->createChildSceneNode("hingeJoint2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							linesNode2->attachObject(lines2);
						}
					}

					//Character joint
					if ((*it)->GetComponentTypeName() == CharacterJoint::COMPONENT_TYPE)
					{
						CharacterJoint* joint = (CharacterJoint*)(*it);

						SceneNode* connectedNode = nullptr;
						connectedNode = sceneMgr->getSceneNodeFast(joint->GetConnectedObjectName());

						if (connectedNode != nullptr)
						{
							ManualObject* lines1 = new ManualObject("characterJointObj1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines1->setCastShadows(false);
							lines1->setQueryFlags(1 << 3);
							lines1->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
							lines1->position(joint->GetConnectedAnchor());
							lines1->end();
							lines1->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
							SceneNode* linesNode1 = wireframeNode->createChildSceneNode("characterJoint1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							linesNode1->_setDerivedPosition(connectedNode->_getDerivedPosition());
							linesNode1->_setDerivedOrientation(connectedNode->_getDerivedOrientation());
							linesNode1->attachObject(lines1);

							ManualObject* lines2 = new ManualObject("characterJointObj2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines2->setCastShadows(false);
							lines2->setQueryFlags(1 << 3);
							lines2->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
							lines2->position(joint->GetAnchor());
							lines2->end();
							lines2->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
							SceneNode* linesNode2 = wireframeNode->createChildSceneNode("characterJoint2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							linesNode2->attachObject(lines2);
						}
					}

					//Skeleton bone
					if ((*it)->GetComponentTypeName() == SkeletonBone::COMPONENT_TYPE)
					{
						SkeletonBone* bone = (SkeletonBone*)(*it);

						if (bone->GetIsRootBone())
						{
							SceneNode* boneNode = wireframeNode->createChildSceneNode("skeletonBone_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");

							ManualObject* lines1 = new ManualObject("boneObj1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
							lines1->setCastShadows(false);
							lines1->setQueryFlags(1 << 3);
							lines1->begin("BoneMaterial", RenderOperation::OperationType::OT_LINE_LIST, "Editor");


							createBonesRecursive(currNodeWr, currNodeWr, lines1);

							lines1->end();
							lines1->setRenderQueueGroup(RENDER_QUEUE_9 - 1);

							boneNode->attachObject(lines1);
							
							boneNode->setPosition(Vector3(0, 0, 0));
						}
					}

					//NavMesh obstacle
					if ((*it)->GetComponentTypeName() == NavMeshObstacle::COMPONENT_TYPE)
					{
						SceneNode* boxNode = wireframeNode->createChildSceneNode("navMeshObstacle_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
						NavMeshObstacle* comp = (NavMeshObstacle*)(*it);

						MeshPtr boxMesh = Procedural::BoxGenerator(comp->getSize().x, comp->getSize().y, comp->getSize().z).realizeMesh("NavMeshObstacleObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh");
						Entity* boxEntity = sceneMgr->createEntity("NavMeshObstacleObject_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo_mesh", boxMesh);
						boxEntity->setMaterial(obstacleMaterial);
						boxEntity->setQueryFlags(1 << 3);
						boxEntity->setCastShadows(false);
						boxEntity->setRenderQueueGroup(RENDER_QUEUE_9 - 2);

						boxNode->attachObject(boxEntity);

						boxNode->setScale(2, 2, 2);

						boxNode->setPosition(comp->getOffset());
					}

					//Vehicle
					if ((*it)->GetComponentTypeName() == Vehicle::COMPONENT_TYPE)
					{
						Vehicle* vehicle = (Vehicle*)(*it);
						
						std::vector<Vehicle::WheelInfo>& wheels = vehicle->getWheels();

						for (auto it = wheels.begin(); it != wheels.end(); ++it)
						{
							auto wheel = *it;

							SceneNode* connectedNode = nullptr;
							connectedNode = sceneMgr->getSceneNodeFast(wheel.m_connectedObjectName);

							if (connectedNode != nullptr)
							{
								//Center point
								ManualObject* lines1 = new ManualObject("vehicleWheelObj1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
								lines1->setCastShadows(false);
								lines1->setQueryFlags(1 << 3);
								lines1->begin("HingeJointMaterial", RenderOperation::OperationType::OT_POINT_LIST, "Editor");
								lines1->position(wheel.m_connectionPoint);
								lines1->end();
								lines1->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
								SceneNode* linesNode1 = wireframeNode->createChildSceneNode("vehicleWheel1_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
								linesNode1->attachObject(lines1);

								//Direction
								ManualObject* lines2 = new ManualObject("vehicleWheelObj2_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
								lines2->setCastShadows(false);
								lines2->setQueryFlags(1 << 3);
								lines2->begin("HingeJointMaterial", RenderOperation::OperationType::OT_LINE_LIST, "Editor");
								lines2->position(wheel.m_connectionPoint);
								lines2->position(wheel.m_connectionPoint + ((currNodeWr->_getDerivedOrientation().Inverse() * wheel.m_direction) * (wheel.m_suspensionRestLength * 2.0f)) / scale.y);
								lines2->end();
								lines2->setRenderQueueGroup(RENDER_QUEUE_9 - 1);
								linesNode1->attachObject(lines2);

								//Circle
								ManualObject* circle = new ManualObject("WireframeObject_spotLight_" + currNodeWr->getName() + "_" + GUIDGenerator::genGuid() + "_gizmo");
								circle->setCastShadows(false);

								float const radius = wheel.m_radius;
								float accuracy = 35;

								circle->begin("HingeJointMaterial", RenderOperation::OT_LINE_LIST, "Editor");
								Vector3 dir = currNodeWr->_getDerivedOrientation().Inverse() * wheel.m_axle;
								dir.normalise();
								Vector3 right(dir.z, 0, -dir.x);
								right.normalise();
								Vector3 up = dir.crossProduct(right);
								Quaternion quat = Quaternion(right, up, dir);

								unsigned point_index = 0;
								for (float theta = 0; theta <= 2 * Math::PI; theta += Math::PI / accuracy)
								{
									circle->position((quat * Vector3(radius * cos(theta), radius * sin(theta), 0)) + wheel.m_connectionPoint);
									circle->position((quat * Vector3(radius * cos(theta - Math::PI / accuracy), radius * sin(theta - Math::PI / accuracy), 0)) + wheel.m_connectionPoint);
								}

								circle->end();

								wireframeNode->attachObject(circle);
							}
						}
					}
				}
			}

			wireArray.clear();
		}
	}
}