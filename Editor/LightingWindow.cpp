#include "stdafx.h"
#include "LightingWindow.h"

#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "../Engine/Engine.h"
#include "../Engine/IO.h"
#include "../Engine/DeferredShading/DeferredLightCP.h"

#include "MainWindow.h"
#include "AssetsWindow.h"
#include "TreeNode.h"
#include "TreeView.h"
#include "Lightmap.h"
#include "ConsoleWindow.h"
#include "DialogProgress.h"

using namespace std;
using namespace Ogre;

LightingWindow::LightingWindow()
{
	progressDialog = new DialogProgress();
	progressDialog->setTitle("Baking lightmaps");
}

LightingWindow::~LightingWindow()
{
	delete progressDialog;
}

void LightingWindow::update()
{
	if (opened)
	{
		if (ImGui::Begin("Lighting", &opened, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::BeginColumns(string("lightingWindowColumns1").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Skybox material");
			
			ImGui::NextColumn();

			string mat = IO::GetFileName(skyboxMaterial);
			if (ImGui::Button(mat.c_str(), ImVec2(110, 0)))
			{
				MainWindow::GetAssetsWindow()->focusOnFile(skyboxMaterial);
			}

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

							MaterialPtr mat = MaterialManager::getSingleton().getByName(path, "Assets");

							if (mat != nullptr)
							{
								GetEngine->GetSceneManager()->setSkyBox(true, path, 1300, true, Quaternion::IDENTITY, "Assets");
								skyboxMaterial = path;
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				GetEngine->GetSceneManager()->setSkyBox(false, "");
				skyboxMaterial = "None";
			}
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns2").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Ambient light color");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::ColorEdit4("##ambient_light_color", ambientLightColor.ptr()))
			{
				GetEngine->GetSceneManager()->setAmbientLight(ambientLightColor);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns3").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Default lightmap size");
			ImGui::NextColumn();
			int current = defaultLightmapSize;
			ImGui::PushItemWidth(-1);
			if (ImGui::BeginCombo("##default_lightmap_size", to_string(current).c_str()))
			{
				for (int n = 0; n < lightmapSizes.size(); n++)
				{
					bool is_selected = (current == lightmapSizes[n]);

					if (ImGui::Selectable(to_string(lightmapSizes[n]).c_str(), is_selected))
					{
						current = lightmapSizes[n];
						defaultLightmapSize = current;

						GetEngine->SetDefaultLightmapSize(current);
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns4").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Region lightmap size");
			ImGui::NextColumn();
			current = regionLightmapSize;
			ImGui::PushItemWidth(-1);
			if (ImGui::BeginCombo("##region_lightmap_size", to_string(current).c_str()))
			{
				for (int n = 0; n < regionLightmapSizes.size(); n++)
				{
					bool is_selected = (current == regionLightmapSizes[n]);

					if (ImGui::Selectable(to_string(regionLightmapSizes[n]).c_str(), is_selected))
					{
						current = regionLightmapSizes[n];
						regionLightmapSize = current;

						GetEngine->SetRegionLightmapSize(current);
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			ImGui::BeginColumns(string("lightingWindowColumns5").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			ImGui::Text("Lightmap blur radius");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputFloat("##lightmap_blur_radius", &lightmapBlurRadius))
			{
				GetEngine->SetLightmapBlurRadius(lightmapBlurRadius);
			}
			ImGui::PopItemWidth();
			ImGui::EndColumns();

			if (ImGui::Button("Clear baked data"))
			{
				MainWindow::AddOnEndUpdateCallback([=]() { clearBakedData(); });
			}

			ImGui::SameLine();

			if (ImGui::Button("Bake lighting"))
			{
				MainWindow::AddOnEndUpdateCallback([=]() { bakeLighting(); });
			}

			/*if (ImGui::Button("Bake regions"))
			{
				std::string loadedScene = IO::RemovePart(MainWindow::GetLoadedScenePath(), GetEngine->GetAssetsPath());
				string sPath = IO::GetFilePath(loadedScene);
				string sName = IO::GetFileName(loadedScene);
				std::string saveToDir = GetEngine->GetAssetsPath() + sPath + sName + "/";
				std::string saveTo = GetEngine->GetAssetsPath() + sPath + sName + "/Lightmaps/";

				MainWindow::AddOnEndUpdateCallback([=]() {
					GetEngine->ClearMaterialsInstances();
					GetEngine->buildStaticGeometry(saveTo, true, false);
					StaticGeometry* sg = GetEngine->getStaticGeometry();
					sg->reset();
				});
			}*/
		}

		ImGui::End();

		progressDialog->update();
	}

	/*StaticGeometry* sg = GetEngine->getStaticGeometry();
	auto reg = sg->getRegionIterator();
	int i = 0;
	while (reg.hasMoreElements())
	{
		StaticGeometry::Region* region = reg.getNext();

		auto lit = region->getLODIterator();
		while (lit.hasMoreElements())
		{
			StaticGeometry::LODBucket* lod = lit.getNext();

			auto mtx = lod->getMaterialIterator();
			int _m = 0;
			while (mtx.hasMoreElements())
			{
				auto mtb = mtx.getNext();

				auto qGeom = mtb->getGeometryIterator();
				int _j = 0;
				while (qGeom.hasMoreElements())
				{
					++_j;
					ImGui::Begin(("Region Texture " + to_string(i) + to_string(_m) + to_string(_j)).c_str(), &opened);

					auto git = qGeom.getNext();
					auto qGeomList = git->getQueuedGeometryList();

					int texSize = regionLightmapSize * regionLightmapSize;
					float fragSize = sqrt((float)texSize / (float)qGeomList.size());

					for (auto it = qGeomList.begin(); it != qGeomList.end(); ++it)
					{
						StaticGeometry::QueuedGeometry* qgeom = *it;
						TexturePtr t = qgeom->lightmapTexture;
						if (t == nullptr)
							continue;

						ImGui::SetCursorPos(ImVec2(qgeom->lightmapOffsetX, qgeom->lightmapOffsetY));
						ImGui::Image((void*)t->getHandle(), ImVec2(fragSize, fragSize));
					}

					ImGui::End();
				}

				++_m;
			}
		}

		++i;
	}*/
}

void LightingWindow::bakeLighting()
{
	clearBakedData();

	std::vector<SceneNode*> staticNodes;

	getSceneNodesWithEntities(GetEngine->GetSceneManager()->getRootSceneNode(), staticNodes);

	std::string loadedScene = IO::RemovePart(MainWindow::GetLoadedScenePath(), GetEngine->GetAssetsPath());
	string sPath = IO::GetFilePath(loadedScene);
	string sName = IO::GetFileName(loadedScene);
	std::string saveToDir = GetEngine->GetAssetsPath() + sPath + sName + "/";
	std::string saveTo = GetEngine->GetAssetsPath() + sPath + sName + "/Lightmaps/";

	CreateDirectory(CString(saveToDir.c_str()), NULL);
	CreateDirectory(CString(saveTo.c_str()), NULL);

	int totalObjects = staticNodes.size();

	progressDialog->show();

	int i = 0;
	for (std::vector<SceneNode*>::const_iterator it = staticNodes.begin(); it != staticNodes.end(); ++it, ++i)
	{
		float progress = (float)1.0f / (float)totalObjects * (float)i;
		float progressP = (float)100.0f / (float)totalObjects * (float)i;

		progressDialog->setStatusText("Baking lightmaps... " + to_string((int)progressP) + "%% (" + to_string(i) + "/" + to_string(totalObjects) + ")");
		progressDialog->setProgress(progress);

		Entity* pEntity = (Entity*)(*it)->getAttachedObject(0);
		int lightmapSize = GetEngine->GetDefaultLightmapSize();

		if (pEntity->getUseCustomLightmapSize())
			lightmapSize = pEntity->getCustomLightmapSize();

		CEntityLightMap* pEntityLightMap = new CEntityLightMap(pEntity, saveTo, 0, lightmapSize);
		delete pEntityLightMap;
	}

	progressDialog->setStatusText("Packing atlases...");
	GetEngine->ClearMaterialsInstances();
	GetEngine->buildStaticGeometry(saveTo, true, false);
	StaticGeometry* sg = GetEngine->getStaticGeometry();
	sg->reset();

	progressDialog->hide();

	MainWindow::GetConsoleWindow()->log("Lightmaps baking done.\n");

	DeferredLightRenderOperation::UpdateStaticShadowmaps();
}

void LightingWindow::clearBakedData()
{
	string loadedScene = IO::RemovePart(MainWindow::GetLoadedScenePath(), GetEngine->GetAssetsPath());
	string sPath = IO::GetFilePath(loadedScene);
	string sName = IO::GetFileName(loadedScene);
	string lightmapsDir = GetEngine->GetAssetsPath() + sPath + sName + "/Lightmaps/";

	IO::DirDeleteRecursive(lightmapsDir);

	//Delete lightmaps from memory
	std::vector<SceneNode*> staticNodes;
	getSceneNodesWithEntities(GetEngine->GetSceneManager()->getRootSceneNode(), staticNodes);

	for (std::vector<SceneNode*>::const_iterator it = staticNodes.begin(); it != staticNodes.end(); ++it)
	{
		Entity* entity = (Entity*)(*it)->getAttachedObject(0);

		for (int i = 0; i < entity->getNumSubEntities(); ++i)
		{
			SubEntity* subEntity = entity->getSubEntity(i);

			TexturePtr lightmap = subEntity->getLightmapTexture();
			if (lightmap != nullptr)
			{
				subEntity->setLightmapTexture(TexturePtr());
				TextureManager::getSingleton().remove(lightmap);
			}
		}
	}

	DeferredLightRenderOperation::UpdateStaticShadowmaps();
}

void LightingWindow::getSceneNodesWithEntities(Ogre::SceneNode* root, std::vector<Ogre::SceneNode*>& outList)
{
	VectorIterator it = root->getChildIterator();

	while (it.hasMoreElements())
	{
		SceneNode* node = (SceneNode*)it.getNext();

		if (GetEngine->IsEditorObject(node))
			continue;

		if (node->lightmapStatic)
		{
			if (node->getAttachedObjects().size() > 0)
			{
				if (node->getAttachedObject(0)->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
				{
					if (node->getAttachedObject(0)->getQueryFlags() == 1 << 1)
					{
						outList.push_back(node);
					}
				}
			}
		}

		getSceneNodesWithEntities(node, outList);
	}
}
