#include "stdafx.h"

#include "../Engine/Engine.h"
#include "../Engine/TerrainManager.h"

#include "InputHandler.h"
#include "EditorClasses.h"
#include "TerrainEditor.h"
#include "MainWindow.h"

bool InputHandler::mLButtonDown = false;
bool InputHandler::mMButtonDown = false;
bool InputHandler::mRButtonDown = false;
bool InputHandler::mWasGizmoUndo = false;
bool InputHandler::wPressed = false;
bool InputHandler::sPressed = false;
bool InputHandler::aPressed = false;
bool InputHandler::dPressed = false;
bool InputHandler::qPressed = false;
bool InputHandler::ePressed = false;
bool InputHandler::keysPressed = false;
bool InputHandler::ctrlPressed = false;
bool InputHandler::shiftPressed = false;

float InputHandler::cameraSpeed = 1.0f;
float InputHandler::cameraSpeedNormal = 1.0f;
float InputHandler::cameraSpeedFast = 2.0f;

Vector2 InputHandler::mPrevMousePoint = Vector2::ZERO;
Vector2 InputHandler::mStartMousePoint = Vector2::ZERO;

void InputHandler::OnLMouseDown(int x, int y)
{
	TerrainEditor::OnLMouseDown(x, y);

	if (GetEngine->IsInitialized() && !TerrainEditor::GetTerrainMode())
	{
		if (!mRButtonDown && !mMButtonDown)
			MainWindow::gizmo2->mouseDown(x, y);

		mStartMousePoint = Vector2(x, y);
	}

	if (!mRButtonDown && !mMButtonDown)
		mLButtonDown = true;

	mRButtonDown = false;
	mMButtonDown = false;
}

void InputHandler::OnLMouseUp(int x, int y)
{
	if (GetEngine->IsInitialized() && !TerrainEditor::GetTerrainMode())
	{
		MainWindow::gizmo2->mouseUp(x, y, ctrlPressed);
	}

	TerrainEditor::OnLMouseUp(x, y);

	mLButtonDown = false;
	mRButtonDown = false;
	mMButtonDown = false;
}

void InputHandler::OnRMouseDown(int x, int y)
{
	if (GetEngine->IsInitialized())
	{
		if (!mLButtonDown && !mMButtonDown)
			mRButtonDown = true;

		mLButtonDown = false;
		mMButtonDown = false;
	}
}

void InputHandler::OnRMouseUp(int x, int y)
{
	if (GetEngine->IsInitialized())
	{
		mLButtonDown = false;
		mRButtonDown = false;
		mMButtonDown = false;
		keysPressed = false;
	}
}

void InputHandler::OnMMouseDown(int x, int y)
{
	if (GetEngine->IsInitialized())
	{
		if (!mLButtonDown && !mRButtonDown)
			mMButtonDown = true;

		mLButtonDown = false;
		mRButtonDown = false;
	}
}

void InputHandler::OnMMouseUp(int x, int y)
{
	if (GetEngine->IsInitialized())
	{
		mLButtonDown = false;
		mRButtonDown = false;
		mMButtonDown = false;
	}
}

void InputHandler::OnMouseMove(int x, int y)
{
	if (GetEngine->IsInitialized())
	{
		//Camera moving
		Real rOffsetX = (Real)x - (Real)mPrevMousePoint.x;
		Real rOffsetY = (Real)y - (Real)mPrevMousePoint.y;

		Camera* pCamera = MainWindow::mainCamera;
		if (mRButtonDown)
		{
			rOffsetX *= 0.01f;
			rOffsetY *= 0.01f;
			EditorClasses::RotateCamera(pCamera, rOffsetX, rOffsetY);
		}
		if (mMButtonDown)
		{
			rOffsetX *= 0.1f;
			rOffsetY *= 0.1f;
			EditorClasses::PanningCamera(pCamera, rOffsetX, rOffsetY);
		}

		mPrevMousePoint = Vector2(x, y);

		///Gizmo mouse move

		if (!TerrainEditor::GetTerrainMode())
		{
			if (mLButtonDown && !mRButtonDown && !mMButtonDown)
			{
				MainWindow::gizmo2->mouseMove(x, y);
			}

			TerrainEditor::HideBrushGizmo();
		}
		else
		{
			if (mLButtonDown)
			{
				TerrainEditor::OnMouseMove(x, y);
			}

			TerrainEditor::UpdateBrushGizmo(x, y);
		}
	}
}

void InputHandler::OnKeyDown(SDL_Scancode code)
{
	if (code == SDL_SCANCODE_LCTRL)
	{
		ctrlPressed = true;
		//MainWindow::gizmo->setSnapToGrid(true);
	}

	if (code == SDL_SCANCODE_LSHIFT)
	{
		shiftPressed = true;
		cameraSpeed = cameraSpeedFast;
	}

	if (code == SDL_SCANCODE_DELETE)
	{

	}

	// Select all
	if (ctrlPressed)
	{
		if (code == SDL_SCANCODE_A) // A key
		{
			if (MainWindow::gizmo2->getSelectedObjects().size() > 0)
			{
				MainWindow::gizmo2->clearSelection();
				MainWindow::gizmo2->selectObjects({});
			}
			else
			{
				/*CClassView* hierarchy = ((CMainFrame*)m_pMainWnd)->GetHierarchyWindow();
				CViewTree* tree = hierarchy->GetTreeView();

				HTREEITEM treeItem = tree->GetRootItem();
				treeItem = tree->GetChildItem(treeItem);

				std::vector<SceneNode*> objects;
				tree->SetRedraw(false);

				while (treeItem != nullptr)
				{
					string* __data = reinterpret_cast<string*>(tree->GetItemData(treeItem));
					string data = "";
					if (__data != nullptr)
						data = *__data;

					SceneNode* _node = GetEngine->GetSceneManager()->getSceneNode(data);
					objects.push_back(_node);

					treeItem = tree->GetNextSiblingItem(treeItem);
				}*/

				//gizmo->clearSelection();
				//gizmo->selectObjects(objects);
				//objects.clear();
			}
		}

		// Inverse selection
		if (code == SDL_SCANCODE_I) // I key
		{
			/*CClassView* hierarchy = ((CMainFrame*)m_pMainWnd)->GetHierarchyWindow();
			CViewTree* tree = hierarchy->GetTreeView();

			HTREEITEM treeItem = tree->GetRootItem();
			treeItem = tree->GetChildItem(treeItem);

			SelectInverse(tree, treeItem);

			gizmo->show();
			gizmo->CallSelectCallback();*/
		}
	}

	//if (mRButtonDown)
	{
		if (code == SDL_SCANCODE_W) // W
			wPressed = true;

		if (code == SDL_SCANCODE_S) // S
			sPressed = true;

		if (code == SDL_SCANCODE_A) // A
			aPressed = true;

		if (code == SDL_SCANCODE_D) // D
			dPressed = true;

		if (code == SDL_SCANCODE_Q) // Q
			qPressed = true;

		if (code == SDL_SCANCODE_E) // E
			ePressed = true;
	}
}

void InputHandler::OnKeyUp(SDL_Scancode code)
{
	if (code == SDL_SCANCODE_LCTRL)
	{
		ctrlPressed = false;
		//MainWindow::gizmo->setSnapToGrid(false);
	}

	if (code == SDL_SCANCODE_LSHIFT)
	{
		shiftPressed = false;
		cameraSpeed = cameraSpeedNormal;
	}

	if (code == SDL_SCANCODE_W) // W
		wPressed = false;

	if (code == SDL_SCANCODE_S) // S
		sPressed = false;

	if (code == SDL_SCANCODE_A) // A
		aPressed = false;

	if (code == SDL_SCANCODE_D) // D
		dPressed = false;

	if (code == SDL_SCANCODE_Q) // Q
		qPressed = false;

	if (code == SDL_SCANCODE_E) // E
		ePressed = false;
}

void InputHandler::OnMouseWheel(int value)
{
	if (GetEngine->IsInitialized())
	{
		if (!mLButtonDown)
		{
			Camera* pCamera = MainWindow::mainCamera;
			EditorClasses::ZoomCamera(pCamera, (Real)value * cameraSpeedFast);
		}
	}
}

void InputHandler::Update()
{
	Camera* pCamera = MainWindow::mainCamera;

	if (mRButtonDown)
	{
		if (wPressed) // W
			EditorClasses::MoveCamera(pCamera, -pCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Z * cameraSpeed);

		if (sPressed) // S
			EditorClasses::MoveCamera(pCamera, pCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Z * cameraSpeed);

		if (aPressed) // A
			EditorClasses::MoveCamera(pCamera, -pCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_X * cameraSpeed);

		if (dPressed) // D
			EditorClasses::MoveCamera(pCamera, pCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_X * cameraSpeed);

		if (qPressed) // Q
			EditorClasses::MoveCamera(pCamera, -pCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Y * cameraSpeed);

		if (ePressed) // E
			EditorClasses::MoveCamera(pCamera, pCamera->getParentSceneNode()->getLocalAxes() * Vector3::UNIT_Y * cameraSpeed);

		if (wPressed || sPressed || aPressed || dPressed || qPressed || ePressed)
		{
			keysPressed = true;
		}
		else
		{
			keysPressed = false;
		}
	}
}
