#include "stdafx.h"
#include "NavigationWindow.h"
#include "MainWindow.h"
#include "DialogProgress.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "../Engine/Engine.h"
#include "../Engine/NavigationManager.h"
#include "../Engine/IO.h"
#include "../Engine/EditorSettings.h"

DialogProgress* progressDialog = nullptr;

NavigationWindow::NavigationWindow()
{
	progressDialog = new DialogProgress();
	progressDialog->setTitle("Baking NavMesh");
}

NavigationWindow::~NavigationWindow()
{
	delete progressDialog;
}

void NavigationWindow::update()
{
	if (opened)
	{
		if (ImGui::Begin("Navigation", &opened, ImGuiWindowFlags_NoCollapse))
		{
			NavigationManager* navMgr = GetEngine->GetNavigationManager();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Walkable slope angle");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##walkable_slope_angle", &walkableSlopeAngle))
			{
				navMgr->SetWalkableSlopeAngle(walkableSlopeAngle);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Walkable height");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##walkable_height", &walkableHeight))
			{
				navMgr->SetWalkableHeight(walkableHeight);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Walkable climb");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##walkable_climb", &walkableClimb))
			{
				navMgr->SetWalkableClimb(walkableClimb);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Walkable radius");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##walkable_radius", &walkableRadius))
			{
				navMgr->SetWalkableRadius(walkableRadius);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Max edge length");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##max_edge_length", &maxEdgeLength))
			{
				navMgr->SetMaxEdgeLen(maxEdgeLength);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Max simplification error");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##max_simplification_error", &maxSimplificationError))
			{
				navMgr->SetMaxSimplificationError(maxSimplificationError);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Min region area");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##min_region_area", &minRegionArea))
			{
				navMgr->SetMinRegionArea(minRegionArea);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Merge region area");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##merge_region_area", &mergeRegionArea))
			{
				navMgr->SetMergeRegionArea(mergeRegionArea);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Cell size");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##cell_size", &cellSize))
			{
				navMgr->SetCellSize(cellSize);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Cell height");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##cell_height", &cellHeight))
			{
				navMgr->SetCellHeight(cellHeight);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			if (ImGui::Button("Clear NavMesh"))
			{
				std::string loadedScene = GetEngine->GetNavigationManager()->GetLoadedScene();

				string sName = IO::GetFileName(loadedScene);
				string sPath = IO::GetFilePath(loadedScene);

				if (!loadedScene.empty())
				{
					std::string _navMeshPath = sPath + "/" + sName + "/" + sName + ".navmesh";

					if (IO::FileExists(_navMeshPath))
						IO::FileDelete(_navMeshPath);
				}

				GetEngine->GetNavigationManager()->ClearNavMesh();
			}

			ImGui::SameLine();

			if (ImGui::Button("Build NavMesh"))
			{
				MainWindow::AddOnEndUpdateCallback([=]() {
					SceneNode* navMesh = GetEngine->GetSceneManager()->getSceneNodeFast("NavigationMesh_node_gizmo");

					if (navMesh != nullptr)
					{
						navMesh->setVisible(true);
						MainWindow::Get()->navMeshVisible = true;

						EditorSettings settings;
						settings.Load(MainWindow::GetProjectSettingsDir() + "Editor.settings");
						settings.showNavMesh = true;
						settings.Save(MainWindow::GetProjectSettingsDir() + "Editor.settings");
					}

					progressDialog->show();

					GetEngine->GetNavigationManager()->BuildNavMesh([=](int progress, int totalIter, int currentIter) {
						progressDialog->setStatusText("Baking NavMesh... " + to_string(progress) + "%% (" + to_string(currentIter) + "/" + to_string(totalIter) + " tiles)");
						progressDialog->setProgress((float)progress / 100.0f);
					});

					progressDialog->hide();
				});
			}
		}

		ImGui::End();
		progressDialog->update();
	}
}
