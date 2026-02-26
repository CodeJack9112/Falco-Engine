#include "stdafx.h"
#include "SceneWindow.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../Engine/TerrainManager.h"
#include "../Engine/IO.h"
#include "../Engine/SceneSerializer.h"
#include "../Engine/EditorSettings.h"

#include "MainWindow.h"
#include "HierarchyWindow.h"
#include "TreeNode.h"
#include "EditorClasses.h"
#include "InputHandler.h"

using namespace Ogre;

SceneWindow::SceneWindow()
{
}

SceneWindow::~SceneWindow()
{
}

void SceneWindow::init()
{
	cameraIcon = TextureManager::getSingleton().load("Icons/Toolbar/camera.png", "Editor");
	gridIcon = TextureManager::getSingleton().load("Icons/Toolbar/grid.png", "Editor");
	navMeshIcon = TextureManager::getSingleton().load("Icons/Toolbar/navmesh.png", "Editor");
	shadowsIcon = TextureManager::getSingleton().load("Icons/Toolbar/shadows.png", "Editor");

	loadCameraSettings();
}

void SceneWindow::update()
{
	if (opened)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		bool begin = ImGui::Begin("Scene", &opened, ImGuiWindowFlags_NoCollapse);
		ImGui::PopStyleVar();

		if (begin)
		{
			focused = ImGui::IsWindowFocused();

			if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			{
				ImGui::SetWindowFocus();
				ImGui::ClearActiveID();
			}

			position = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 22);
			size = Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 22);

			if (size != oldSize)
			{
				MainWindow::UpdateViewport();
			}

			oldSize = size;

			ImVec2 sz = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 22);
			ImGui::Image((void*)MainWindow::sceneRenderTarget->getHandle(), sz);

			hovered = ImGui::IsItemHovered();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
				{
					TreeNode* move_from = (TreeNode*)payload->Data;
					Vector2 point = Vector2(ImGui::GetMousePos().x - position.x, ImGui::GetMousePos().y - position.y);

					if (IO::GetFileExtension(move_from->alias) != "prefab")
					{
						dropObject(move_from->getPath(), point);
					}
					else
					{
						dropPrefab(MainWindow::GetProjectAssetsDir() + move_from->getPath(), point);
					}
				}
				ImGui::EndDragDropTarget();
			}

			updateToolbar();

			MainWindow::gizmo2->setViewportHovered(hovered);
			MainWindow::gizmo2->setRect(RealRect(position.x, position.y, size.x, size.y));
			MainWindow::gizmo2->setClipRect(RealRect(position.x, position.y + getToolbarHeight(), size.x, size.y - getToolbarHeight()));
			MainWindow::gizmo2->draw();
		}

		ImGui::End();
	}
}

Ogre::Vector2 SceneWindow::getPosition()
{
	return position;
}

Ogre::Vector2 SceneWindow::getSize()
{
	return size;
}

bool SceneWindow::isFocused()
{
	return focused;
}

bool SceneWindow::isHovered()
{
	return hovered;
}

float SceneWindow::getToolbarHeight()
{
	return 28.0f;
}

static void SceneHelpMarker(const char* desc)
{
	if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f)
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void SceneWindow::updateToolbar()
{
	ImGui::SetCursorPos(ImVec2(0, 21));

	ImVec4 _col = ImGui::GetStyleColorVec4(ImGuiCol_ChildBg);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(_col.x, _col.y, _col.z, 0.8f));
	bool begin = ImGui::BeginChild("SceneToolbar", ImVec2(size.x, 28));
	ImGui::PopStyleColor();
	if (begin)
	{
		SceneNode * camNode = MainWindow::mainCamera->getParentSceneNode();
		Vector3 camPos = camNode->_getDerivedPosition();
		float fcp = MainWindow::mainCamera->getFarClipDistance();

		ImGui::SetCursorPos(ImVec2(3, 6));
		ImGui::Image((void*)cameraIcon->getHandle(), ImVec2(16, 16));

		ImGui::SameLine();

		ImGui::SetCursorPosY(3.0f);

		if (cameraSpeedPreset != 0)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		bool btn1 = ImGui::Button("1", ImVec2(22, 22));
		
		if (cameraSpeedPreset != 0)
			ImGui::PopStyleVar();

		SceneHelpMarker("Camera Speed 1");

		if (btn1)
		{
			InputHandler::cameraSpeedNormal = cameraSpeed1;
			InputHandler::cameraSpeedFast = cameraSpeed1 * 2.0f;
			InputHandler::cameraSpeed = cameraSpeed1;
			cameraSpeedPreset = 0;

			saveCameraSettings();
		}
		
		ImGui::SameLine();

		if (cameraSpeedPreset != 1)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		bool btn2 = ImGui::Button("2", ImVec2(22, 22));
		
		if (cameraSpeedPreset != 1)
			ImGui::PopStyleVar();

		SceneHelpMarker("Camera Speed 2");

		if (btn2)
		{
			InputHandler::cameraSpeedNormal = cameraSpeed2;
			InputHandler::cameraSpeedFast = cameraSpeed2 * 2.0f;
			InputHandler::cameraSpeed = cameraSpeed2;
			cameraSpeedPreset = 1;

			saveCameraSettings();
		}

		ImGui::SameLine();

		if (cameraSpeedPreset != 2)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		bool btn3 = ImGui::Button("3", ImVec2(22, 22));

		if (cameraSpeedPreset != 2)
			ImGui::PopStyleVar();

		SceneHelpMarker("Camera Speed 3");

		if (btn3)
		{
			InputHandler::cameraSpeedNormal = cameraSpeed3;
			InputHandler::cameraSpeedFast = cameraSpeed3 * 2.0f;
			InputHandler::cameraSpeed = cameraSpeed3;
			cameraSpeedPreset = 2;

			saveCameraSettings();
		}

		ImGui::SameLine();

		ImGui::SetNextItemWidth(128);
		if (ImGui::InputFloat("##CameraSpeed", &InputHandler::cameraSpeedNormal, 0.25f, 0.5f))
		{
			if (InputHandler::cameraSpeedNormal < 0.05f)
				InputHandler::cameraSpeedNormal = 0.05f;

			InputHandler::cameraSpeedFast = InputHandler::cameraSpeedNormal * 2.0f;
			InputHandler::cameraSpeed = InputHandler::cameraSpeedNormal;

			if (cameraSpeedPreset == 0)
				cameraSpeed1 = InputHandler::cameraSpeedNormal;

			if (cameraSpeedPreset == 1)
				cameraSpeed2 = InputHandler::cameraSpeedNormal;

			if (cameraSpeedPreset == 2)
				cameraSpeed3 = InputHandler::cameraSpeedNormal;

			saveCameraSettings();
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		ImGui::Text("X: ");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(64);
		if (ImGui::InputFloat("##CameraPositionX", &camPos[0]))
		{
			camNode->_setDerivedPosition(camPos);
		}

		ImGui::SameLine();
		ImGui::Text("Y: ");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(64);
		if (ImGui::InputFloat("##CameraPositionY", &camPos[1]))
		{
			camNode->_setDerivedPosition(camPos);
		}

		ImGui::SameLine();
		ImGui::Text("Z: ");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(64);
		if (ImGui::InputFloat("##CameraPositionZ", &camPos[2]))
		{
			camNode->_setDerivedPosition(camPos);
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		
		ImGui::Text("FCP: ");
		ImGui::SameLine();

		ImGui::SetNextItemWidth(64);
		if (ImGui::InputFloat("##CameraFarClipPlane", &fcp))
		{
			MainWindow::mainCamera->setFarClipDistance(fcp);

			saveCameraSettings();
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		ImVec4 col1 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 col2 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 col3 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");
		bool nav = navMesh != nullptr && MainWindow::Get()->navMeshVisible;
		bool grid = MainWindow::Get()->gridVisible;
		bool shadows = MainWindow::Get()->shadowsEnabled;

		if (grid) col1 = ImVec4(1, 1, 1, 1);
		if (nav) col2 = ImVec4(1, 1, 1, 1);
		if (shadows) col3 = ImVec4(1, 1, 1, 1);

		if (ImGui::ImageButton((void*)gridIcon->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col1)) { MainWindow::Get()->OnShowGrid(); } // Grid
		ImGui::SameLine(); SceneHelpMarker("Toggle grid");
		if (ImGui::ImageButton((void*)navMeshIcon->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col2)) { MainWindow::Get()->OnShowNavMesh(); } // NavMesh
		ImGui::SameLine(); SceneHelpMarker("Toggle NavMesh");
		if (ImGui::ImageButton((void*)shadowsIcon->getHandle(), ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col3)) { MainWindow::Get()->OnEnableShadows(); } // Shadows
		ImGui::SameLine(); SceneHelpMarker("Toggle shadows");

		ImGui::EndChild();
	}
}

void SceneWindow::saveCameraSettings()
{
	EditorSettings settings;
	settings.Load(MainWindow::GetProjectSettingsDir() + "Editor.settings");

	settings.cameraSpeed = cameraSpeed1;
	settings.cameraSpeed2 = cameraSpeed2;
	settings.cameraSpeed3 = cameraSpeed3;
	settings.cameraSpeedPreset = cameraSpeedPreset;
	if (MainWindow::mainCamera != nullptr)
		settings.cameraFarClipPlane = MainWindow::mainCamera->getFarClipDistance();

	settings.Save(MainWindow::GetProjectSettingsDir() + "Editor.settings");
}

void SceneWindow::loadCameraSettings()
{
	EditorSettings settings;
	settings.Load(MainWindow::GetProjectSettingsDir() + "Editor.settings");
	
	cameraSpeed1 = settings.cameraSpeed;
	cameraSpeed2 = settings.cameraSpeed2;
	cameraSpeed3 = settings.cameraSpeed3;
	cameraSpeedPreset = settings.cameraSpeedPreset;
	if (MainWindow::mainCamera != nullptr)
		MainWindow::mainCamera->setFarClipDistance(settings.cameraFarClipPlane);

	if (cameraSpeedPreset == 0)
		InputHandler::cameraSpeedNormal = cameraSpeed1;
	if (cameraSpeedPreset == 1)
		InputHandler::cameraSpeedNormal = cameraSpeed2;
	if (cameraSpeedPreset == 2)
		InputHandler::cameraSpeedNormal = cameraSpeed3;

	InputHandler::cameraSpeedFast = InputHandler::cameraSpeedNormal * 2.0f;
	InputHandler::cameraSpeed = InputHandler::cameraSpeedNormal;
}

void SceneWindow::dropObject(std::string path, Ogre::Vector2 point)
{
	string ext = IO::GetFileExtension(path);

	if (!ext.empty())
	{
		FILE* pFile = fopen((MainWindow::GetProjectAssetsDir() + path).c_str(), "rb");

		if (pFile)
		{
			fclose(pFile);

			if (ext._Equal("mesh"))
			{
				SceneNode* node = EditorClasses::PlaceSceneModel(GetEngine->GetSceneManager(), Ogre::String(path.c_str()), point);

				MainWindow::GetHierarchyWindow()->addNode(node);
				MainWindow::gizmo2->selectObject(node, false);

				//theApp.AddUndo();
			}

			if (ext._Equal("fbx"))
			{
				SceneNode* node = EditorClasses::PlaceFBXScene(GetEngine->GetSceneManager(), Ogre::String(path.c_str()), point);

				if (node != nullptr)
				{
					MainWindow::Get()->ProcessSceneNode(node, true);
					MainWindow::AddOnEndUpdateCallback([=]() {
						MainWindow::gizmo2->selectObject(node, false);
					});

					//theApp.AddUndo();
				}
			}
			if (ext._Equal("terrain"))
			{
				GetEngine->GetTerrainManager()->CreateTerrain(0, 0, MainWindow::GetProjectAssetsDir() + path);
			}
		}
	}
}

void SceneWindow::dropPrefab(std::string path, Ogre::Vector2 point)
{
	if (IO::FileExists(path))
	{
		SceneSerializer serializer;
		SceneNode* prefab = serializer.DeserializeFromPrefab(GetEngine->GetSceneManager(), path);
		prefab->setAlias(IO::GetFileName(path));

		MainWindow::Get()->ProcessSceneNode(prefab, true);

		Vector3 pos = EditorClasses::GetWorldPointFromScreenPoint(point);
		prefab->_setDerivedPosition(pos);

		MainWindow::AddOnEndUpdateCallback([=]() {
			MainWindow::gizmo2->selectObject(prefab, false);
		});
	}
}
