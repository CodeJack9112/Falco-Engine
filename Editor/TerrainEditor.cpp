#include "stdafx.h"
#include "TerrainEditor.h"
#include "MainWindow.h"
#include "InputHandler.h"

#include "../Engine/Engine.h"
#include "../Engine/TerrainManager.h"
#include "../Engine/Mathf.h"

#include <OgreViewport.h>

bool TerrainEditor::terrainMode = false;
TerrainEditor::TerrainEditMode TerrainEditor::terrainEditMode = TerrainEditMode::EM_RAISE;
float TerrainEditor::terrainBrushSize = 2.0f;
float TerrainEditor::terrainBrushStrength = 2.0f;
int TerrainEditor::selectedTexture = 0;
int TerrainEditor::selectedGrass = 0;
int TerrainEditor::selectedMesh = 0;

SceneNode* TerrainEditor::brushNode = nullptr;
ManualObject* TerrainEditor::brushGizmo = nullptr;

float detailDist = 0;

void TerrainEditor::OnLMouseDown(int x, int y)
{
	if (GetTerrainMode())
	{
		MainWindow::gizmo2->selectObject(nullptr, false);

		if (GetTerrainEditMode() == TerrainEditMode::EM_RAISE || GetTerrainEditMode() == TerrainEditMode::EM_LOWER || GetTerrainEditMode() == TerrainEditMode::EM_SMOOTH)
		{
			TerrainManager* terrMgr = GetEngine->GetTerrainManager();
			PagedGeometry* geom1 = terrMgr->GetDetailPagedGeometry();
			PagedGeometry* geom2 = terrMgr->GetGrassPagedGeometry();

			if (geom1 != nullptr)
			{
				//geom1->setVisible(false);
				detailDist = terrMgr->GetDetailMeshesDrawDistance();
				terrMgr->SetDetailMeshesDrawDistance(0);
			}

			if (geom2 != nullptr)
				geom2->setVisible(false);
		}
	}
}

void TerrainEditor::OnLMouseUp(int x, int y)
{
	if (GetTerrainMode())
	{
		if (GetTerrainEditMode() == TerrainEditMode::EM_PAINT_DETAIL_MESHES)
		{
			Terrain* terrain = GetEngine->GetTerrainManager()->GetTerrainGroup()->getTerrain(0, 0);
			float mBrushSize = GetTerrainBrushSize() / 100.0f; // 0.2 max
			float mBrushStrength = GetTerrainBrushStrength() / 10.0f;

			if (terrain != nullptr)
			{
				// fire ray
				Ray ray;
				ray = MainWindow::mainCamera->getCameraToViewportRay((float)x / (float)MainWindow::mainCamera->getViewport()->getActualWidth(), (float)y / (float)MainWindow::mainCamera->getViewport()->getActualHeight());

				TerrainGroup::RayResult rayResult = GetEngine->GetTerrainManager()->GetTerrainGroup()->rayIntersects(ray);
				if (rayResult.hit)
				{
					Vector3 tsPos;
					terrain->getTerrainPosition(rayResult.position, &tsPos);

					//CTerrainWnd* terrainWnd = ((CMainFrame*)m_pMainWnd)->GetTerrainWindow();
					int meshIndex = selectedMesh;// terrainWnd->GetSelectedMesh();

					float size = (terrain->getSize() - 1);

					if (!InputHandler::IsShiftPressed())
					{
						int meshCount = (mBrushStrength * 2) + 1;
						for (int i = 0; i < meshCount; ++i)
						{
							float rx = Mathf::RandomFloat(-mBrushSize / 5, mBrushSize / 5);
							float rz = Mathf::RandomFloat(-mBrushSize / 5, mBrushSize / 5);

							long startx = (tsPos.x + rx) * size;
							long starty = (tsPos.y + rz) * size;
							float y = terrain->getHeightAtPoint(startx, starty);

							Vector3 terr = Vector3(startx, starty, y);
							Vector3 world;
							terrain->convertPosition(Terrain::Space::POINT_SPACE, terr, Terrain::Space::WORLD_SPACE, world);

							GetEngine->GetTerrainManager()->PlaceDetailMesh(meshIndex, world);
						}
					}
				}
			}
		}
		if (GetTerrainEditMode() == TerrainEditMode::EM_RAISE || GetTerrainEditMode() == TerrainEditMode::EM_LOWER || GetTerrainEditMode() == TerrainEditMode::EM_SMOOTH)
		{
			TerrainManager* terrMgr = GetEngine->GetTerrainManager();
			PagedGeometry* geom1 = terrMgr->GetDetailPagedGeometry();
			PagedGeometry* geom2 = terrMgr->GetGrassPagedGeometry();

			if (geom1 != nullptr)
			{
				//geom1->setVisible(true);
				//geom1->reloadGeometry();
				terrMgr->SetDetailMeshesDrawDistance(detailDist);
			}

			if (geom2 != nullptr)
			{
				geom2->setVisible(true);
				geom2->reloadGeometry();
			}
		}
	}
}

void TerrainEditor::OnMouseMove(int x, int y)
{
	//Terrain edit
	TerrainGroup* terrGroup = GetEngine->GetTerrainManager()->GetTerrainGroup();
	Terrain* terrain = terrGroup->getTerrain(0, 0);
	float mBrushSize = GetTerrainBrushSize() / 100.0f;
	float mBrushStrength = GetTerrainBrushStrength() / 10.0f;
	float dt = GetEngine->GetDeltaTime();
	if (dt > 1)
		dt = 1;

	if (terrain != nullptr)
	{
		Viewport * viewport = MainWindow::mainCamera->getViewport();
		float fw = (float)viewport->getActualWidth();
		float fh = (float)viewport->getActualHeight();

		// fire ray
		Ray ray = MainWindow::mainCamera->getCameraToViewportRay((float)x / fw, (float)y / fh);

		TerrainGroup::RayResult rayResult = terrGroup->rayIntersects(ray);
		if (rayResult.hit)
		{
			Vector3 tsPos;
			terrain->getTerrainPosition(rayResult.position, &tsPos);

			if (GetTerrainEditMode() == TerrainEditMode::EM_PAINT_DETAIL_MESHES)
			{
				if (InputHandler::IsShiftPressed())
				{
					float size = (terrain->getSize() - 1);

					long startx = tsPos.x * size;
					long starty = tsPos.y * size;
					float y = terrain->getHeightAtPoint(startx, starty);

					Vector3 terr = Vector3(startx, starty, y);
					Vector3 world;
					terrain->convertPosition(Terrain::Space::POINT_SPACE, terr, Terrain::Space::WORLD_SPACE, world);

					TreeLoader2D* loader = GetEngine->GetTerrainManager()->GetTreeLoader();
					loader->deleteTrees(world, mBrushSize * 1000);
				}
			}
			else
			{
				int mLayer = selectedTexture;
				int mGrassLayer = selectedGrass;

				TerrainLayerBlendMap* layer = nullptr;

				if (mLayer > 0 && terrain->getLayerCount() > mLayer)
					layer = terrain->getLayerBlendMap(mLayer);

				Real size = 0;
				if (GetTerrainEditMode() == TerrainEditMode::EM_PAINT_TEXTURE)
					size = terrain->getLayerBlendMapSize();
				else
					size = (terrain->getSize() - 1);

				long startx = (tsPos.x - mBrushSize) * size;
				long starty = (tsPos.y - mBrushSize) * size;
				long endx = (tsPos.x + mBrushSize) * size;
				long endy = (tsPos.y + mBrushSize) * size;
				startx = std::max(startx, 0L);
				starty = std::max(starty, 0L);
				endx = std::min(endx, (long)size);
				endy = std::min(endy, (long)size);

				for (long y = starty; y <= endy; ++y)
				{
					for (long x = startx; x <= endx; ++x)
					{
						Real tsXdist = (x / size) - tsPos.x;
						Real tsYdist = (y / size) - tsPos.y;

						Real weight = (Real)std::min((Real)1.0, Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5 * mBrushSize));
						weight = 1.0 - (weight * weight);

						if (GetTerrainEditMode() == TerrainEditMode::EM_RAISE || GetTerrainEditMode() == TerrainEditMode::EM_LOWER || GetTerrainEditMode() == TerrainEditMode::EM_SMOOTH)
						{
							float addedHeight = weight * mBrushStrength;
							float newheight = 0;
							if (GetTerrainEditMode() == TerrainEditMode::EM_RAISE)
							{
								if (!InputHandler::IsShiftPressed())
									newheight = terrain->getHeightAtPoint(x, y) + addedHeight;
								else
									newheight = terrain->getHeightAtPoint(x, y) - addedHeight;
							}
							else if (GetTerrainEditMode() == TerrainEditMode::EM_LOWER)
							{
								if (!InputHandler::IsShiftPressed())
									newheight = terrain->getHeightAtPoint(x, y) - addedHeight;
								else
									newheight = terrain->getHeightAtPoint(x, y) + addedHeight;
							}
							else if (GetTerrainEditMode() == TerrainEditMode::EM_SMOOTH)
							{
								std::vector<float> smoothGroup;
								for (long _y = y - 2; _y <= y + 2; ++_y)
								{
									for (long _x = x - 2; _x <= x + 2; ++_x)
									{
										Real tsXdist = (_x / size) - tsPos.x;
										Real tsYdist = (_y / size) - tsPos.y;

										Real weight = (Real)std::min((Real)1.0, Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5 * mBrushSize));
										weight = 1.0 - (weight * weight);

										float addedHeight = terrain->getHeightAtPoint(_x, _y) * weight;

										smoothGroup.push_back(addedHeight);
									}
								}

								float avgHeight = 0;
								for (std::vector<float>::iterator it = smoothGroup.begin(); it != smoothGroup.end(); ++it)
								{
									avgHeight += *it;
								}

								avgHeight /= smoothGroup.size();

								float h = terrain->getHeightAtPoint(x, y);
								newheight = h + (avgHeight - h) * weight * mBrushStrength * 0.01f;
							}

							terrain->setHeightAtPoint(x, y, newheight);
						}
						else if (GetTerrainEditMode() == TerrainEditMode::EM_PAINT_TEXTURE)
						{
							float paint = weight * mBrushStrength * 0.5;
							size_t imgY = size - y;

							if (x < size && x >= 0 && imgY < size && imgY >= 0)
							{
								if (layer == nullptr)
								{
									for (int i = 0; i < terrain->getLayerCount() - 1; ++i)
									{
										TerrainLayerBlendMap* _layer = terrain->getLayerBlendMap(i + 1);
										float val = _layer->getBlendValue(x, imgY) - paint;

										val = Math::Clamp(val, 0.0f, 1.0f);
										_layer->setBlendValue(x, imgY, val);
									}
								}
								else
								{
									for (int i = 0; i < terrain->getLayerCount() - 1; ++i)
									{
										if (i + 1 != mLayer)
										{
											TerrainLayerBlendMap* _layer = terrain->getLayerBlendMap(i + 1);
											float val = _layer->getBlendValue(x, imgY) - paint;

											val = Math::Clamp(val, 0.0f, 1.0f);
											_layer->setBlendValue(x, imgY, val);
										}
									}

									TerrainLayerBlendMap* layer = terrain->getLayerBlendMap(mLayer);
									float val = layer->getBlendValue(x, imgY) + paint;

									val = Math::Clamp(val, 0.0f, 1.0f);
									layer->setBlendValue(x, imgY, val);
								}
							}
						}
						else if (GetTerrainEditMode() == TerrainEditMode::EM_PAINT_GRASS)
						{
							weight = (Real)std::min((Real)1.0, Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5 * mBrushSize * 0.5));
							weight = 1.0 - (weight * weight);

							float paint = (weight) * mBrushStrength * 0.5f;
							size_t imgY = size - y;

							GrassLoader* loader = GetEngine->GetTerrainManager()->GetGrassLoader();
							if (mGrassLayer >= 0 && mGrassLayer < loader->getLayerList().size())
							{
								GrassLayer* layer = *std::next(loader->getLayerList().begin(), mGrassLayer);

								float sz = (size + 1);

								if (!InputHandler::IsShiftPressed())
								{
									float dens = layer->getDensityMap()->getDensityAt(x, imgY, FloatRect(0, 0, sz, sz));
									float val = dens + paint;
									val = Math::Clamp(val, 0.0f, 1.0f);
									if (val > dens)
										layer->getDensityMap()->setDensityAt(x, imgY, FloatRect(0, 0, sz, sz), val);
								}
								else
								{
									float dens = layer->getDensityMap()->getDensityAt(x, imgY, FloatRect(0, 0, sz, sz));
									float val = dens - paint;
									val = Math::Clamp(val, 0.0f, 1.0f);
									if (val < dens)
										layer->getDensityMap()->setDensityAt(x, imgY, FloatRect(0, 0, sz, sz), val);
								}

								Vector3 gp;
								terrain->convertPosition(Terrain::Space::POINT_SPACE, Vector3(x, y, 0), Terrain::Space::WORLD_SPACE, gp);

								float yc = terrain->getHeightAtWorldPosition(gp.x, rayResult.position.y, gp.z);
								GetEngine->GetTerrainManager()->GetGrassPagedGeometry()->reloadGeometryPage(Vector3(gp.x, yc, gp.z));
							}
						}
					}
				}

				if (GetTerrainEditMode() == TerrainEditMode::EM_PAINT_TEXTURE)
				{
					for (int i = 0; i < terrain->getLayerCount() - 1; ++i)
					{
						TerrainLayerBlendMap* _layer = terrain->getLayerBlendMap(i + 1);
						_layer->update();
					}
				}

				if (GetTerrainEditMode() != TerrainEditMode::EM_PAINT_GRASS && GetTerrainEditMode() != TerrainEditMode::EM_PAINT_DETAIL_MESHES)
					terrain->update();
			}
		}
	}
}

void TerrainEditor::UpdateBrushGizmo(int x, int y)
{
	if (terrainEditMode == TerrainEditMode::EM_SETTINGS)
	{
		if (brushNode != nullptr)
		{
			if (brushNode->getVisible())
				brushNode->setVisible(false);

			return;
		}
	}

	TerrainGroup* terrGroup = GetEngine->GetTerrainManager()->GetTerrainGroup();
	Terrain* terrain = terrGroup->getTerrain(0, 0);
	float mBrushSize = GetTerrainBrushSize() * 10.0f;

	if (terrain != nullptr)
	{
		Viewport* viewport = MainWindow::mainCamera->getViewport();
		float fw = (float)viewport->getActualWidth();
		float fh = (float)viewport->getActualHeight();

		if (brushNode == nullptr)
		{
			brushNode = GetEngine->GetSceneManager()->getRootSceneNode()->createChildSceneNode("terrainBrushNode_gizmo");
			brushGizmo = GetEngine->GetSceneManager()->createManualObject("terrainBrush_gizmo");
			brushNode->attachObject(brushGizmo);
		}
		else
		{
			if (!brushNode->getVisible())
				brushNode->setVisible(true);
		}

		MaterialPtr brushGizmoMat = MainWindow::gizmo2->terrainBrushMaterial;

		brushGizmo->clear();
		brushGizmo->begin(brushGizmoMat->getName(), Ogre::RenderOperation::OT_TRIANGLE_FAN, "Editor");

		//Draw brush gizmo circle
		Ray _ray = MainWindow::mainCamera->getCameraToViewportRay((float)x / fw, (float)y / fh);
		TerrainGroup::RayResult _rayResult = terrGroup->rayIntersects(_ray);
		if (_rayResult.hit)
		{
			int num_segments = 32;
			float radius = mBrushSize;

			float yc = terrain->getHeightAtWorldPosition(_rayResult.position.x, _rayResult.position.y, _rayResult.position.z);
			brushGizmo->position(Vector3(_rayResult.position.x, yc, _rayResult.position.z));

			for (int ii = 0; ii < num_segments + 1; ++ii)
			{
				float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
				float _x = radius * cosf(theta);
				float _y = radius * sinf(theta);

				float y = terrain->getHeightAtWorldPosition(_rayResult.position.x - _x, _rayResult.position.y, _rayResult.position.z - _y);

				brushGizmo->position(Vector3(_rayResult.position.x - _x, y, _rayResult.position.z - _y));
			}
		}

		brushGizmo->end();
	}
}

void TerrainEditor::HideBrushGizmo()
{
	if (brushNode != nullptr)
	{
		if (brushNode->getVisible())
			brushNode->setVisible(false);
	}
}
