#include "stdafx.h"
#include "TerrainWindow.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreTextureUnitState.h>

#include "../Engine/Engine.h"
#include "../Engine/TerrainManager.h"
#include "../Engine/IO.h"
#include "../Engine/ResourceMap.h"

#include "MainWindow.h"
#include "TerrainEditor.h"
#include "TreeNode.h"
#include "TreeView.h"

using namespace std;
using namespace Ogre;

TerrainWindow::TerrainWindow()
{
}

TerrainWindow::~TerrainWindow()
{
}

void TerrainWindow::init()
{
	raiseTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_raise.png", "Editor");
	lowerTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_lower.png", "Editor");
	smoothTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_smooth.png", "Editor");
	paintTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_paint.png", "Editor");
	treeTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_detail_meshes.png", "Editor");
	grassTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_grass.png", "Editor");
	settingsTexture = TextureManager::getSingleton().load("Icons/Terrain/terrain_settings.png", "Editor");

	brushSize = TerrainEditor::GetTerrainBrushSize();
	brushStrength = TerrainEditor::GetTerrainBrushStrength();
}

void TerrainWindow::update()
{
	int removeTexture = -1;
	int removeGrass = -1;
	int removeMesh = -1;

	if (opened)
	{
		if (ImGui::Begin("Terrain", &opened, ImGuiWindowFlags_NoCollapse))
		{
			TerrainManager* terrMgr = GetEngine->GetTerrainManager();
			TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
			Terrain* terrain = terrGroup->getTerrain(0, 0);

			ImGuiButtonFlags btnActive = ImGuiButtonFlags_None;
			ImGuiButtonFlags btnDisabled = ImGuiButtonFlags_Disabled;

			bool terrainMode = TerrainEditor::GetTerrainMode();

			if (terrain != nullptr)
			{
				if (terrainMode) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.55f, 0.55f, 1.0f));
				if (terrainMode) ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
				if (ImGui::ButtonEx("Edit terrain", ImVec2(0, 0)))
				{
					TerrainEditor::SetTerrainMode(!TerrainEditor::GetTerrainMode());
				}
				if (terrainMode) ImGui::PopStyleColor(2);

				ImGui::SameLine();
			}

			if (terrain != nullptr)
			{
				if (ImGui::Button("Remove terrain"))
				{
					terrMgr->RemoveAllTerrains();
					TerrainEditor::SetTerrainMode(false);
				}
			}
			else
			{
				if (ImGui::Button("Create terrain"))
				{
					MainWindow::Get()->OnCreateTerrain();
				}
			}

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

			if (terrain != nullptr)
			{
				if (terrainMode)
				{
					ImVec4 col1 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
					ImVec4 col2 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
					ImVec4 col3 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
					ImVec4 col4 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
					ImVec4 col5 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
					ImVec4 col6 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
					ImVec4 col7 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_RAISE) col1 = ImVec4(1, 1, 1, 1);
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_LOWER) col2 = ImVec4(1, 1, 1, 1);
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_SMOOTH) col3 = ImVec4(1, 1, 1, 1);
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_PAINT_TEXTURE) col4 = ImVec4(1, 1, 1, 1);
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_PAINT_DETAIL_MESHES) col5 = ImVec4(1, 1, 1, 1);
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_PAINT_GRASS) col6 = ImVec4(1, 1, 1, 1);
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_SETTINGS) col7 = ImVec4(1, 1, 1, 1);

					ImGui::Text("Tools");

					if (ImGui::ImageButton((void*)raiseTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col1))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_RAISE);
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)lowerTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col2))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_LOWER);
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)smoothTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col3))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_SMOOTH);
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)paintTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col4))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_PAINT_TEXTURE);
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)treeTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col5))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_PAINT_DETAIL_MESHES);
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)grassTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col6))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_PAINT_GRASS);
					}
					ImGui::SameLine();
					if (ImGui::ImageButton((void*)settingsTexture->getHandle(), ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), col7))
					{
						TerrainEditor::SetTerrainEditMode(TerrainEditor::TerrainEditMode::EM_SETTINGS);
					}

					ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

					if (TerrainEditor::GetTerrainEditMode() != TerrainEditor::TerrainEditMode::EM_SETTINGS)
					{
						ImGui::Text("Brush size");
						ImGui::SameLine();
						if (ImGui::SliderFloat("##brush_size", &brushSize, 0.0f, 20.0f, "%.4f"))
						{
							TerrainEditor::SetTerrainBrushSize(brushSize);
						}

						ImGui::Text("Brush strength");
						ImGui::SameLine();
						if (ImGui::SliderFloat("##brush_strength", &brushStrength, 0.0f, 20.0f, "%.4f"))
						{
							TerrainEditor::SetTerrainBrushStrength(brushStrength);
						}
					}

					//Texture list
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_PAINT_TEXTURE)
					{
						ImGuiWindow* window = GImGui->CurrentWindow;
						ImGuiID id = window->GetIDNoKeepAlive("##PaintTexturesVS");
						ImGui::BeginChild(id, ImVec2(0, 180), true);

						auto paintTextures = terrMgr->GetTextures();
						int i = 0;
						int j = 1;
						int cnt = (ImGui::GetWindowWidth() - 20) / 74.0;

						for (auto it = paintTextures.begin(); it != paintTextures.end(); ++it, ++i, ++j)
						{
							TexturePtr tex = TextureManager::getSingleton().load(it->diffusePath, "Assets");

							int curTex = TerrainEditor::GetSelectedTexture();

							if (i == curTex)
							{
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
								ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.65f, 0.65f, 1.0f));
							}
							ImGui::ImageButton((void*)tex->getHandle(), ImVec2(64, 64));
							if (ImGui::IsItemClicked())
							{
								TerrainEditor::SetSelectedTexture(i);
							}
							if (editTexture == -1)
							{
								if (ImGui::BeginPopupContextItem(("##texturePopup" + to_string(i)).c_str(), 1))
								{
									bool selected = false;

									if (ImGui::Selectable("Edit", false))
									{
										editTexture = i;
									}
									if (ImGui::Selectable("Remove", false))
									{
										removeTexture = i;
									}

									ImGui::EndPopup();
								}
							}
							if (i == curTex) ImGui::PopStyleColor(2);

							if (j < cnt)
								ImGui::SameLine();
							else
								j = 0;
						}

						ImGui::EndChild();

						if (ImGui::Button("Add texture"))
						{
							addTerrainTexture();
						}
					}

					//Meshes list
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_PAINT_DETAIL_MESHES)
					{
						ImGuiWindow* window = GImGui->CurrentWindow;
						ImGuiID id = window->GetIDNoKeepAlive("##PaintMeshesVS");
						ImGui::BeginChild(id, ImVec2(0, 180), true);

						auto list = terrMgr->GetDetailMeshes();
						int i = 0;
						int j = 1;
						int cnt = (ImGui::GetWindowWidth() - 20) / 74.0;

						for (auto it = list.begin(); it != list.end(); ++it, ++i, ++j)
						{
							int curTex = TerrainEditor::GetSelectedMesh();

							if (i == curTex)
							{
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
								ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.65f, 0.65f, 1.0f));
							}
							string btnName = IO::GetFileName(it->path);
							if (btnName.empty())
								btnName = "Mesh " + to_string(i);
							ImGui::Button(btnName.c_str(), ImVec2(64, 64));
							if (ImGui::IsItemClicked())
							{
								TerrainEditor::SetSelectedMesh(i);
							}
							if (editMesh == -1)
							{
								if (ImGui::BeginPopupContextItem(("##meshPopup" + to_string(i)).c_str(), 1))
								{
									bool selected = false;

									if (ImGui::Selectable("Edit", false))
									{
										editMesh = i;
									}
									if (ImGui::Selectable("Remove", false))
									{
										removeMesh = i;
									}

									ImGui::EndPopup();
								}
							}
							if (i == curTex) ImGui::PopStyleColor(2);

							if (j < cnt)
								ImGui::SameLine();
							else
								j = 0;
						}

						ImGui::EndChild();

						if (ImGui::Button("Add mesh"))
						{
							addTerrainMesh();
						}
					}

					//Grass list
					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_PAINT_GRASS)
					{
						ImGuiWindow* window = GImGui->CurrentWindow;
						ImGuiID id = window->GetIDNoKeepAlive("##PaintGrassVS");
						ImGui::BeginChild(id, ImVec2(0, 180), true);

						auto list = terrMgr->GetGrass();
						int i = 0;
						int j = 1;
						int cnt = (ImGui::GetWindowWidth() - 20) / 74.0;

						for (auto it = list.begin(); it != list.end(); ++it, ++i, ++j)
						{
							MaterialPtr mat;
							if (MaterialManager::getSingleton().resourceExists(it->path, "Assets"))
								mat = MaterialManager::getSingleton().getByName(it->path, "Assets");

							TexturePtr tex;

							if (mat != nullptr)
							{
								if (mat->getNumTechniques() > 0)
								{
									if (mat->getTechnique(0)->getNumPasses() > 0)
									{
										if (mat->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0)
										{
											if (mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->_getTexturePtr() != nullptr)
											{
												tex = TextureManager::getSingleton().load(mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName(), "Assets");
											}
										}
									}
								}
							}

							if (tex == nullptr)
								tex = TextureManager::getSingleton().load("default", "Editor");

							int curTex = TerrainEditor::GetSelectedGrass();

							if (i == curTex)
							{
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
								ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.65f, 0.65f, 1.0f));
							}
							ImGui::ImageButton((void*)tex->getHandle(), ImVec2(64, 64));
							if (ImGui::IsItemClicked())
							{
								TerrainEditor::SetSelectedGrass(i);
							}
							if (editGrass == -1)
							{
								if (ImGui::BeginPopupContextItem(("##grassPopup" + to_string(i)).c_str(), 1))
								{
									bool selected = false;

									if (ImGui::Selectable("Edit", false))
									{
										editGrass = i;
									}
									if (ImGui::Selectable("Remove", false))
									{
										removeGrass = i;
									}

									ImGui::EndPopup();
								}
							}
							if (i == curTex) ImGui::PopStyleColor(2);

							if (j < cnt)
								ImGui::SameLine();
							else
								j = 0;
						}

						ImGui::EndChild();

						if (ImGui::Button("Add grass"))
						{
							addTerrainGrass();
						}
					}

					if (TerrainEditor::GetTerrainEditMode() == TerrainEditor::TerrainEditMode::EM_SETTINGS)
					{
						ImGui::Text("Terrain size");

						int current = terrain->getSize() - 1;
						if (ImGui::BeginCombo("##terrain_size", to_string(current).c_str()))
						{
							for (int n = 0; n < terrainSizes.size(); n++)
							{
								bool is_selected = (current == terrainSizes[n]);

								if (ImGui::Selectable(to_string(terrainSizes[n]).c_str(), is_selected))
								{
									current = terrainSizes[n];

									terrain->setSize(current + 1);

									terrMgr->RecreateGrassDensityMaps();
									terrMgr->GetDetailPagedGeometry()->reloadGeometry();
									terrMgr->GetGrassPagedGeometry()->reloadGeometry();
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}

							ImGui::EndCombo();
						}

						ImGui::Text("Terrain world size");

						float wsz = terrain->getWorldSize();
						if (ImGui::InputFloat("##terrain_world_size", &wsz))
						{
							terrain->setWorldSize(wsz);

							terrMgr->UpdateGrassBounds();
							terrMgr->GetDetailPagedGeometry()->reloadGeometry();
							terrMgr->GetGrassPagedGeometry()->reloadGeometry();
						}

						ImGui::Text("Detail meshes draw distance");

						int mddist = terrMgr->GetDetailMeshesDrawDistance();
						if (ImGui::InputInt("##detail_meshes_draw_distance", &mddist))
						{
							terrMgr->SetDetailMeshesDrawDistance(mddist);
						}

						ImGui::Text("Grass draw distance");

						int gddist = terrMgr->GetGrassDrawDistance();
						if (ImGui::InputInt("##grass_draw_distance", &gddist))
						{
							terrMgr->SetGrassDrawDistance(gddist);
						}
					}
				}
			}
			else
			{
				ImGui::Text("No terrain in scene");
			}

			if (editTexture > -1)
				textureEditorWindow();

			if (editGrass > -1)
				grassEditorWindow();

			if (editMesh > -1)
				meshEditorWindow();
		}

		ImGui::End();
	}

	if (removeTexture > -1)
	{
		removeTerrainTexture(removeTexture);
		removeTexture = -1;
	}

	if (removeGrass > -1)
	{
		removeTerrainGrass(removeGrass);
		removeGrass = -1;
	}

	if (removeMesh > -1)
	{
		removeTerrainMesh(removeMesh);
		removeMesh = -1;
	}
}

void TerrainWindow::addTerrainTexture()
{
	TerrainManager* terrMgr = GetEngine->GetTerrainManager();
	TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
	Terrain* terrain = terrGroup->getTerrain(0, 0);

	StringVector lst;
	lst.resize(2);
	lst[0] = "Terrain/default_diffuse.jpg";
	lst[1] = "Terrain/default_normal.jpg";

	terrain->addLayer(100, &lst);
	terrain->load();

	TerrainManager::TextureData data;
	data.diffusePath = "";
	data.diffuseGuid = "";
	data.normalPath = "";
	data.normalGuid = "";
	data.worldSize = 100;
	GetEngine->GetTerrainManager()->AddTexture(data);
}

void TerrainWindow::removeTerrainTexture(int index)
{
	if (TerrainEditor::GetSelectedTexture() >= index)
	{
		if (index - 1 > 0)
			TerrainEditor::SetSelectedTexture(index - 1);
		else
			TerrainEditor::SetSelectedTexture(0);
	}

	TerrainManager* terrMgr = GetEngine->GetTerrainManager();
	TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
	Terrain* terrain = terrGroup->getTerrain(0, 0);

	terrain->removeLayer(index);
	terrain->load();

	GetEngine->GetTerrainManager()->RemoveTexture(index);
}

void TerrainWindow::addTerrainGrass()
{
	TerrainManager::GrassData data;
	data.path = "";
	data.guid = "";
	data.animation = true;
	data.minSize = Vector2(1.0f, 1.0f);
	data.maxSize = Vector2(1.5f, 1.5f);
	data.swayDistribution = 1.0f;
	data.swayLength = 1.0f;
	data.swaySpeed = 1.0f;
	GetEngine->GetTerrainManager()->AddGrass(data);
}

void TerrainWindow::removeTerrainGrass(int index)
{
	if (TerrainEditor::GetSelectedGrass() >= index)
	{
		if (index - 1 > 0)
			TerrainEditor::SetSelectedGrass(index - 1);
		else
			TerrainEditor::SetSelectedGrass(0);
	}

	GetEngine->GetTerrainManager()->RemoveGrass(index);
}

void TerrainWindow::addTerrainMesh()
{
	TerrainManager::DetailMeshData data;
	data.path = "";
	data.guid = "";
	data.minScale = 1.0f;
	data.maxScale = 1.5f;
	GetEngine->GetTerrainManager()->AddDetailMesh(data);
}

void TerrainWindow::removeTerrainMesh(int index)
{
	GetEngine->GetTerrainManager()->RemoveDetailMesh(index);
}

void TerrainWindow::textureEditorWindow()
{
	if (ImGui::Begin("Edit terrain texture", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking))
	{
		TerrainManager* terrMgr = GetEngine->GetTerrainManager();
		TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
		Terrain* terrain = terrGroup->getTerrain(0, 0);

		auto paintTextures = terrMgr->GetTextures();

		TexturePtr diffuse = TextureManager::getSingleton().load(paintTextures[editTexture].diffusePath, "Assets");
		TexturePtr normal = TextureManager::getSingleton().load(paintTextures[editTexture].normalPath, "Assets");

		TerrainManager::TextureData dt = paintTextures.at(editTexture);

		ImGui::ImageButton((void*)diffuse->getHandle(), ImVec2(64, 64));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
			{
				TreeNode * move_from = (TreeNode*)payload->Data;
				if (move_from->treeView->getTag() == "Assets")
				{
					auto images = MainWindow::GetImagesFileFormats();
					if (find(images.begin(), images.end(), IO::GetFileExtension(move_from->alias)) != images.end())
					{
						string path = move_from->getPath();
						terrain->setLayerTextureName(editTexture, 0, path);
						dt.diffusePath = path;
						dt.diffuseGuid = ResourceMap::getResourceGuidFromName(path);
						terrMgr->ReplaceTexture(editTexture, dt);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		ImGui::ImageButton((void*)normal->getHandle(), ImVec2(64, 64));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
			{
				TreeNode* move_from = (TreeNode*)payload->Data;
				if (move_from->treeView->getTag() == "Assets")
				{
					auto images = MainWindow::GetImagesFileFormats();
					if (find(images.begin(), images.end(), IO::GetFileExtension(move_from->alias)) != images.end())
					{
						string path = move_from->getPath();
						terrain->setLayerTextureName(editTexture, 1, path);
						dt.normalPath = path;
						dt.normalGuid = ResourceMap::getResourceGuidFromName(path);
						terrMgr->ReplaceTexture(editTexture, dt);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		ImGui::Text("World size");

		if (ImGui::InputFloat("##texture_world_size", &dt.worldSize))
		{
			terrain->setLayerWorldSize(editTexture, dt.worldSize);
			terrMgr->ReplaceTexture(editTexture, dt);
		}

		if (ImGui::Button("Close"))
		{
			editTexture = -1;
		}
	}

	ImGui::End();
}

void TerrainWindow::grassEditorWindow()
{
	if (ImGui::Begin("Edit grass data", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking))
	{
		TerrainManager* terrMgr = GetEngine->GetTerrainManager();
		TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
		Terrain* terrain = terrGroup->getTerrain(0, 0);

		auto list = terrMgr->GetGrass();

		TerrainManager::GrassData dt = list.at(editGrass);

		MaterialPtr mat;
		if (MaterialManager::getSingleton().resourceExists(dt.path, "Assets"))
			mat = MaterialManager::getSingleton().getByName(dt.path, "Assets");

		TexturePtr tex;

		if (mat != nullptr)
		{
			if (mat->getNumTechniques() > 0)
			{
				if (mat->getTechnique(0)->getNumPasses() > 0)
				{
					if (mat->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0)
					{
						if (mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->_getTexturePtr() != nullptr)
						{
							tex = TextureManager::getSingleton().load(mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName(), "Assets");
						}
					}
				}
			}
		}

		if (tex == nullptr)
			tex = TextureManager::getSingleton().load("default", "Editor");

		ImGui::ImageButton((void*)tex->getHandle(), ImVec2(64, 64));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
			{
				TreeNode* move_from = (TreeNode*)payload->Data;
				if (move_from->treeView->getTag() == "Assets")
				{
					if (IO::GetFileExtension(move_from->alias) == "material")
					{
						string path = move_from->getPath();
						dt.path = path;
						dt.guid = ResourceMap::getResourceGuidFromName(path);

						terrMgr->ReplaceGrass(editGrass, dt);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::Text(dt.path.c_str());

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		ImGui::Text("Min size");

		if (ImGui::InputFloat2("##grass_min_size", dt.minSize.ptr()))
		{
			terrMgr->ReplaceGrass(editGrass, dt);
		}

		ImGui::Text("Max size");

		if (ImGui::InputFloat2("##grass_max_size", dt.maxSize.ptr()))
		{
			terrMgr->ReplaceGrass(editGrass, dt);
		}

		ImGui::Text("Density");

		if (ImGui::InputFloat("##grass_sway_density", &dt.density))
		{
			terrMgr->ReplaceGrass(editGrass, dt);
		}

		if (ImGui::Checkbox("Enable animation", &dt.animation))
		{
			terrMgr->ReplaceGrass(editGrass, dt);
		}

		if (dt.animation)
		{
			ImGui::Text("Sway distribution");

			if (ImGui::InputFloat("##grass_sway_distribution", &dt.swayDistribution))
			{
				terrMgr->ReplaceGrass(editGrass, dt);
			}

			ImGui::Text("Sway length");

			if (ImGui::InputFloat("##grass_sway_length", &dt.swayLength))
			{
				terrMgr->ReplaceGrass(editGrass, dt);
			}

			ImGui::Text("Sway speed");

			if (ImGui::InputFloat("##grass_sway_speed", &dt.swaySpeed))
			{
				terrMgr->ReplaceGrass(editGrass, dt);
			}
		}

		if (ImGui::Button("Close"))
		{
			editGrass = -1;
		}
	}

	ImGui::End();
}

void TerrainWindow::meshEditorWindow()
{
	if (ImGui::Begin("Edit detail mesh data", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking))
	{
		TerrainManager* terrMgr = GetEngine->GetTerrainManager();
		TerrainGroup* terrGroup = terrMgr->GetTerrainGroup();
		Terrain* terrain = terrGroup->getTerrain(0, 0);

		auto list = terrMgr->GetDetailMeshes();

		TerrainManager::DetailMeshData dt = list.at(editMesh);

		ImGui::Button(IO::GetFileName(dt.path).c_str(), ImVec2(64, 64));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
			{
				TreeNode* move_from = (TreeNode*)payload->Data;
				if (move_from->treeView->getTag() == "Assets")
				{
					string ext = IO::GetFileExtension(move_from->alias);
					if (ext == "fbx" || ext == "prefab")
					{
						string path = move_from->getPath();
						dt.path = path;
						dt.guid = ResourceMap::getResourceGuidFromName(path);

						terrMgr->ReplaceDetailMesh(editMesh, dt);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::Text(dt.path.c_str());

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		ImGui::Text("Min scale");

		if (ImGui::InputFloat("##mesh_min_scale", &dt.minScale))
		{
			terrMgr->ReplaceDetailMesh(editMesh, dt);
		}

		ImGui::Text("Max scale");

		if (ImGui::InputFloat("##mesh_max_scale", &dt.maxScale))
		{
			terrMgr->ReplaceDetailMesh(editMesh, dt);
		}

		if (ImGui::Button("Close"))
		{
			editMesh = -1;
		}
	}

	ImGui::End();
}