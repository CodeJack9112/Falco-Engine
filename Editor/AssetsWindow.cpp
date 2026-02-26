#include "stdafx.h"
#include "AssetsWindow.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "MainWindow.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "PreviewWindow.h"
#include "TreeView.h"
#include "TreeNode.h"
#include "SolutionWorker.h"
#include "PropertyEditor.h"
#include "MaterialEditor2.h"
#include "CubemapEditor2.h"

#include <OgreVector2.h>
#include <OgreCamera.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreHighLevelGpuProgram.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreMaterialSerializer.h>
#include <OgreTechnique.h>
#include <OgreRoot.h>

#include "../Engine/Engine.h"
#include "../Engine/IO.h"
#include "../Engine/SceneSerializer.h"
#include "../Engine/DeferredShading/DeferredShading.h"
#include "../Engine/FBXSceneManager.h"
#include "../Engine/StringConverter.h"
#include "../Engine/ResourceMap.h"

#include "../boost/algorithm/string.hpp"

using namespace std;
using namespace Ogre;

AssetsWindow* AssetsWindow::self = nullptr;

IMPLEMENT_DYNCREATE(FileSystemWatcher, CWinThread)

AssetsWindow::AssetsWindow()
{
	self = this;

	treeView = new TreeView();
	treeView->setTag("Assets");
	treeView->setAllowMoveNodes(true);
	treeView->setProcessDragDropInTarget(true);
	treeView->setSelectCallback(onNodesSelected);
	treeView->setOnEndUpdateCallback([=]() { onTreeViewEndUpdate(); });

	treeView->getRootNode()->setSupportedFormats({ "*.", "::SceneNode" });
	treeView->getRootNode()->setOnDropCallback([=](TreeNode* node, TreeNode* from) { onDropNode(node, from); });
}

AssetsWindow::~AssetsWindow()
{
	delete treeView;

	((FileSystemWatcher*)pWinThread)->ExitInstance();
}

void AssetsWindow::init()
{
	addIcon = TextureManager::getSingleton().load("Icons/Toolbar/add.png", "Editor");
	addFolderIcon = TextureManager::getSingleton().load("Icons/Toolbar/add_folder.png", "Editor");
	closeIcon = TextureManager::getSingleton().load("Icons/Hierarchy/close.png", "Editor");

	pWinThread = AfxBeginThread(RUNTIME_CLASS(FileSystemWatcher));
}

void AssetsWindow::update()
{
	bool rootOpened = true;
	
	if (opened)
	{
		if (ImGui::Begin("Assets", &opened, ImGuiWindowFlags_NoCollapse))
		{
			updateAssetsMenu();

			/* TREEVIEW */

			ImGuiWindow* window = GImGui->CurrentWindow;
			ImGuiID id = window->GetIDNoKeepAlive("##AssetsVS");
			ImGui::BeginChild(id);
			treeView->update();
			ImGui::EndChild();

			treeView->updateDragDropTarget();

			/* TREEVIEW */

			if (ImGui::IsMouseDown(0) && ImGui::IsMouseDragging(0))
			{
				isWindowDragging = true;
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(0))
			{
				if (!isWindowDragging)
				{
					if (!ImGui::IsAnyItemHovered())
					{
						treeView->selectNodes({  });
					}
				}
			}

			if (ImGui::IsMouseReleased(0))
			{
				isWindowDragging = false;
			}
		}

		ImGui::End();
	}
}

void AssetsWindow::updateAssetsMenu()
{
	ImGui::ImageButton((void*)addIcon->getHandle(), ImVec2(16, 16));
	
	updateAssetsPopup();

	ImGui::SameLine();

	if (ImGui::ImageButton((void*)addFolderIcon->getHandle(), ImVec2(16, 16)))
	{
		createFolder();
	}

	ImGui::SameLine();

	if (ImGui::InputText("##AssetsFilter", &treeView->getFilter()))
	{
		treeView->setFilter(treeView->getFilter());
	}

	ImVec2 pos = ImGui::GetItemRectMin();
	pos = ImVec2(pos.x - ImGui::GetWindowPos().x, pos.y - ImGui::GetWindowPos().y);

	if (!treeView->getFilter().empty())
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);
		if (ImGui::ImageButton((void*)closeIcon->getHandle(), ImVec2(15, 15)))
		{
			treeView->setFilter("");
		}
	}
	else
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(pos.x + 5);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY());
		ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 0.5), "Search...");
	}

	ImGui::Separator();
}

void AssetsWindow::updateAssetsPopup(bool itemsOnly, TreeNode* selNode)
{
	bool contextMenu = false;

	if (!itemsOnly)
		contextMenu = ImGui::BeginPopupContextItem("add_asset_popup", 0);
	else
	{
		contextMenu = ImGui::BeginMenu("Create");
	}

	if (contextMenu)
	{
		if (ImGui::Selectable("Folder", false))
		{
			createFolder(selNode);
		}

		ImGui::Separator();

		if (ImGui::Selectable("C# Script", false))
		{
			if (treeView->getSelectedNodes().size() <= 1)
			{
				TreeNode* root = treeView->getRootNode();
				
				if (selNode != nullptr)
					root = selNode;
				else
				{
					auto nodes = treeView->getSelectedNodes();

					if (nodes.size() == 1)
						root = treeView->getNodeByName(nodes[0], treeView->getRootNode());
				}

				root->expanded = true;

				if (!IO::isDir(MainWindow::GetProjectAssetsDir() + CP_SYS(root->getPath())))
				{
					root = root->parent;
				}

				addedAssetNode = new TreeNode(treeView);
				addedAssetNode->name = "##" + ResourceMap::genGuid();
				addedAssetNode->icon = getIconByExtension("cs");
				addedAssetNode->enableDrag = true;
				addedAssetNode->format = "cs";
				addedAssetNode->setEditMode(true);
				addedAssetNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
				addedAssetNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
				addedAssetNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);
				addedAssetNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });

				addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool {
					string name = CP_SYS(node->alias);

					string _path = CP_SYS(root->getPath());
					if (!_path.empty()) _path = _path + "/";

					if (!name.empty() && !IO::FileExists(MainWindow::GetProjectAssetsDir() + _path + name))
					{
						AssetsTools::CreateCSharpScript(name, _path);
						addedAssetNode->alias += ".cs";
						addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
						addedAssetNode = nullptr;
						sortTree();
						MainWindow::Get()->CompileScripts();
					}
					else
					{
						deleteAssetNode = true;
					}

					return true;
					});

				root->addChild(addedAssetNode);
			}
		}

		if (ImGui::BeginMenu("Material"))
		{
			if (ImGui::Selectable("Standard Deferred", false))
			{
				createMaterial(selNode, AssetsTools::MaterialType::MT_DEFERRED);
			}

			if (ImGui::Selectable("Empty Deferred", false))
			{
				createMaterial(selNode, AssetsTools::MaterialType::MT_EMPTY_DEFERRED);
			}

			ImGui::Separator();

			if (ImGui::Selectable("Standard Forward", false))
			{
				createMaterial(selNode, AssetsTools::MaterialType::MT_FORWARD);
			}

			if (ImGui::Selectable("Empty Forward", false))
			{
				createMaterial(selNode, AssetsTools::MaterialType::MT_EMPTY_FORWARD);
			}

			ImGui::EndMenu();
		}

		if (ImGui::Selectable("Cubemap", false))
		{
			if (treeView->getSelectedNodes().size() <= 1)
			{
				TreeNode* root = treeView->getRootNode();

				if (selNode != nullptr)
					root = selNode;
				else
				{
					auto nodes = treeView->getSelectedNodes();

					if (nodes.size() == 1)
						root = treeView->getNodeByName(nodes[0], treeView->getRootNode());
				}

				root->expanded = true;

				if (!IO::isDir(MainWindow::GetProjectAssetsDir() + CP_SYS(root->getPath())))
				{
					root = root->parent;
				}

				addedAssetNode = new TreeNode(treeView);
				addedAssetNode->name = "##" + ResourceMap::genGuid();
				addedAssetNode->icon = getIconByExtension("cubemap");
				addedAssetNode->enableDrag = true;
				addedAssetNode->format = "cubemap";
				addedAssetNode->setEditMode(true);
				addedAssetNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
				addedAssetNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
				addedAssetNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);
				addedAssetNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });

				addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool {
					string name = CP_SYS(node->alias);

					string _path = CP_SYS(root->getPath());
					if (!_path.empty()) _path = _path + "/";

					if (!name.empty() && !IO::FileExists(MainWindow::GetProjectAssetsDir() + _path + name))
					{
						AssetsTools::CreateCubemap(name, _path);
						addedAssetNode->alias += ".cubemap";
						addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
						addedAssetNode = nullptr;
						sortTree();
					}
					else
					{
						deleteAssetNode = true;
					}

					return true;
					});

				root->addChild(addedAssetNode);
			}
		}

		ImGui::Separator();

		if (ImGui::Selectable("Vertex Shader", false))
		{
			if (treeView->getSelectedNodes().size() <= 1)
			{
				TreeNode* root = treeView->getRootNode();

				if (selNode != nullptr)
					root = selNode;
				else
				{
					auto nodes = treeView->getSelectedNodes();

					if (nodes.size() == 1)
						root = treeView->getNodeByName(nodes[0], treeView->getRootNode());
				}

				root->expanded = true;

				if (!IO::isDir(MainWindow::GetProjectAssetsDir() + CP_SYS(root->getPath())))
				{
					root = root->parent;
				}

				addedAssetNode = new TreeNode(treeView);
				addedAssetNode->name = "##" + ResourceMap::genGuid();
				addedAssetNode->icon = getIconByExtension("glslv");
				addedAssetNode->enableDrag = true;
				addedAssetNode->format = "glslv";
				addedAssetNode->setEditMode(true);
				addedAssetNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
				addedAssetNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
				addedAssetNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);
				addedAssetNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });

				addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool {
					string name = CP_SYS(node->alias);

					string _path = CP_SYS(root->getPath());
					if (!_path.empty()) _path = _path + "/";

					if (!name.empty() && !IO::FileExists(MainWindow::GetProjectAssetsDir() + _path + name))
					{
						AssetsTools::CreateVertexShader(name, _path);
						addedAssetNode->alias += ".glslv";
						addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
						addedAssetNode = nullptr;
						sortTree();
					}
					else
					{
						deleteAssetNode = true;
					}

					return true;
					});

				root->addChild(addedAssetNode);
			}
		}

		if (ImGui::Selectable("Fragment Shader", false))
		{
			if (treeView->getSelectedNodes().size() <= 1)
			{
				TreeNode* root = treeView->getRootNode();

				if (selNode != nullptr)
					root = selNode;
				else
				{
					auto nodes = treeView->getSelectedNodes();

					if (nodes.size() == 1)
						root = treeView->getNodeByName(nodes[0], treeView->getRootNode());
				}

				root->expanded = true;

				if (!IO::isDir(MainWindow::GetProjectAssetsDir() + CP_SYS(root->getPath())))
				{
					root = root->parent;
				}

				addedAssetNode = new TreeNode(treeView);
				addedAssetNode->name = "##" + ResourceMap::genGuid();
				addedAssetNode->icon = getIconByExtension("glslf");
				addedAssetNode->enableDrag = true;
				addedAssetNode->format = "glslf";
				addedAssetNode->setEditMode(true);
				addedAssetNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
				addedAssetNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
				addedAssetNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);
				addedAssetNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });

				addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool {
					string name = CP_SYS(node->alias);

					string _path = CP_SYS(root->getPath());
					if (!_path.empty()) _path = _path + "/";

					if (!name.empty() && !IO::FileExists(MainWindow::GetProjectAssetsDir() + _path + name))
					{
						AssetsTools::CreateFragmentShader(name, _path);
						addedAssetNode->alias += ".glslf";
						addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
						addedAssetNode = nullptr;
						sortTree();
					}
					else
					{
						deleteAssetNode = true;
					}

					return true;
					});

				root->addChild(addedAssetNode);
			}
		}

		if (!itemsOnly)
			ImGui::EndPopup();
		else
		{
			ImGui::EndMenu();
		}
	}

	if (itemsOnly)
		ImGui::Separator();
}

void AssetsWindow::onNodesSelected(std::vector<std::string> names, void* userData)
{
	if (names.size() == 1)
	{
		TreeNode* node = self->treeView->getNodeByName(names[0], self->treeView->getRootNode());

		if (node != nullptr)
		{
			string path = CP_SYS(node->getPath());
			string ext = IO::GetFileExtension(path);

			PropertyEditor* editor = nullptr;

			if (ext == "material")
			{
				MaterialPtr mat = MaterialManager::getSingleton().getByName(path, "Assets");

				if (mat == nullptr)
					mat = MaterialManager::getSingleton().getByName(IO::GetFileName(path), "Assets");

				if (mat != NULL)
				{
					mat->reload();

					editor = new MaterialEditor2();
					((MaterialEditor2*)editor)->init(mat, mat);
					MainWindow::GetPreviewWindow()->previewMaterial(((MaterialEditor2*)editor)->getMaterial());

					((MaterialEditor2*)editor)->setOnEditorUpdateCallback([=]() {
						MainWindow::GetPreviewWindow()->previewMaterial(((MaterialEditor2*)editor)->getMaterial());
					});
				}
			}

			if (ext == "cubemap")
			{
				CubemapPtr cubemap = CubemapManager::getSingleton().getByName(path, "Assets");

				if (cubemap == nullptr)
					cubemap = CubemapManager::getSingleton().getByName(IO::GetFileName(path), "Assets");

				if (cubemap != NULL)
				{
					editor = new CubemapEditor2();
					((CubemapEditor2*)editor)->init(cubemap);
					MainWindow::GetPreviewWindow()->previewCubemap(cubemap);
				}
			}

			if (ext == "fbx")
			{
				FBXScenePtr fbx = FBXSceneManager::getSingleton().getByName(path, "Assets");
				MainWindow::GetPreviewWindow()->previewFbx(fbx);
			}

			if (ext == "prefab")
			{
				MainWindow::GetPreviewWindow()->previewPrefab(path);
			}

			std::vector<string> imgs = MainWindow::GetImagesFileFormats();

			if (std::find(imgs.begin(), imgs.end(), ext) != imgs.end())
			{
				TexturePtr tex = TextureManager::getSingleton().load(path, "Assets");
				MainWindow::GetPreviewWindow()->previewTexture(tex);
			}

			if (ext == "ttf" || ext == "otf")
			{
				Ogre::FontPtr font = FontManager::getSingleton().getByName(path, "Assets");
				MainWindow::GetPreviewWindow()->previewFont(font);
			}

			if (ext == "txt")
			{
				MainWindow::GetPreviewWindow()->previewText(path);
			}

			if (ext.empty())
				MainWindow::GetPreviewWindow()->clearPreview();

			MainWindow::GetHierarchyWindow()->getTreeView()->selectNodes({ }, false);
			MainWindow::GetInspectorWindow()->setEditor(editor);
			MainWindow::gizmo2->clearSelection();
		}
		else
		{
			MainWindow::GetPreviewWindow()->clearPreview();
			MainWindow::GetInspectorWindow()->setEditor(nullptr);
		}
	}
	else
	{
		MainWindow::GetPreviewWindow()->clearPreview();
		MainWindow::GetInspectorWindow()->setEditor(nullptr);
	}
}

void AssetsWindow::onTreeViewEndUpdate()
{
	if (updateFiles)
	{
		updateFiles = false;
		movingFiles = false;
		reloadFiles();
	}

	if (deleteAssetNode)
	{
		deleteAssetNode = false;
		treeView->deleteNode(addedAssetNode);
		addedAssetNode = nullptr;
	}

	if (deleteNodes.size() > 0)
	{
		for (auto it = deleteNodes.begin(); it != deleteNodes.end(); ++it)
		{
			treeView->deleteNode(*it);
		}

		deleteNodes.clear();
	}
}

Ogre::TexturePtr AssetsWindow::getIconByExtension(std::string ext)
{
	TexturePtr icon = TextureManager::getSingleton().load("Icons/Assets/file.png", "Editor");

	std::vector<std::string> imgs = getImagesFileFormats();

	if (ext.empty())
		icon = TextureManager::getSingleton().load("Icons/Assets/folder.png", "Editor");
	if (ext == "mesh" || ext == "fbx")
		icon = TextureManager::getSingleton().load("Icons/Assets/mesh.png", "Editor");
	if (std::find(imgs.begin(), imgs.end(), ext) != imgs.end())
		icon = TextureManager::getSingleton().load("Icons/Assets/texture.png", "Editor");
	if (ext == "material")
		icon = TextureManager::getSingleton().load("Icons/Assets/material.png", "Editor");
	if (ext == "scene")
		icon = TextureManager::getSingleton().load("Icons/Assets/scene.png", "Editor");
	if (ext == "glslv")
		icon = TextureManager::getSingleton().load("Icons/Assets/vertex.png", "Editor");
	if (ext == "glslf")
		icon = TextureManager::getSingleton().load("Icons/Assets/fragment.png", "Editor");
	if (ext == "cubemap")
		icon = TextureManager::getSingleton().load("Icons/Assets/cubemap.png", "Editor");
	if (ext == "cs")
		icon = TextureManager::getSingleton().load("Icons/Assets/cs.png", "Editor");
	if (ext == "ogg")
		icon = TextureManager::getSingleton().load("Icons/Assets/sound.png", "Editor");
	if (ext == "wav")
		icon = TextureManager::getSingleton().load("Icons/Assets/sound.png", "Editor");
	if (ext == "ttf" || ext == "otf")
		icon = TextureManager::getSingleton().load("Icons/Assets/font.png", "Editor");
	if (ext == "prefab")
		icon = TextureManager::getSingleton().load("Icons/Assets/prefab.png", "Editor");
	if (ext == "txt")
		icon = TextureManager::getSingleton().load("Icons/Assets/text.png", "Editor");

	return icon;
}

void AssetsWindow::reloadFiles()
{
	if (!GetEngine->IsInitialized())
		return;

	if (movingFiles)
		return;

	MainWindow::GetPreviewWindow()->clearPreview();

	//Vectors to determine new resources
	std::vector<std::string> allFilesNames;
	std::vector<std::string> allFilesNames1;
	std::vector<std::string> addedFilesNames;
	std::vector<std::string> removeFilesNames;

	//Store all items
	rememberAllItems(treeView->getRootNode(), allFilesNames);

	allFilesNames1 = allFilesNames;

	removeFilesNames = addNewFiles(allFilesNames);
	addedFilesNames = removeFilesNames;

	//Store all items again to find differences between this list and old list
	//RememberAllItems(m_wndFileView.GetRootItem(), addedFilesNames);

	addedFilesNames.erase(remove_if(begin(addedFilesNames), end(addedFilesNames), [&](auto x)
	{
		return find(begin(allFilesNames), end(allFilesNames), x) != end(allFilesNames);
	}), end(addedFilesNames));

	allFilesNames.erase(remove_if(begin(allFilesNames), end(allFilesNames), [&](auto x)
	{
		return find(begin(removeFilesNames), end(removeFilesNames), x) != end(removeFilesNames);
	}), end(allFilesNames));


	/* LOADING ORDER */
	std::map<std::string, int> loadOrder;
	std::vector<std::string> _imgs = getImagesFileFormats();

	loadOrder[""] = 10;

	for (auto it = _imgs.begin(); it != _imgs.end(); ++it)
	{
		loadOrder[*it] = 100;
	}

	loadOrder["cubemap"] = 200;
	loadOrder["material"] = 300;
	loadOrder["fbx"] = 400;
	loadOrder["scene"] = 500;
	/* LOADING ORDER */

	std::sort(addedFilesNames.begin(), addedFilesNames.end(), [=](string& a, string& b) -> bool {
		std::string _a = IO::GetFileExtension(a);
		std::string _b = IO::GetFileExtension(b);

		if (loadOrder.find(_a) != loadOrder.end() && loadOrder.find(_b) != loadOrder.end())
			return loadOrder.find(_a)->second < loadOrder.find(_b)->second;
		else return true;
	});

	for (auto it = addedFilesNames.begin(); it != addedFilesNames.end(); ++it)
	{
		String path = *it;
		String _fpath = MainWindow::GetProjectAssetsDir() + path;
		string _path = path;// IO::RemovePart(path, MainWindow::GetProjectAssetsDir());

		String ext = IO::GetFileExtension(path);
		std::vector<String> imgs = getImagesFileFormats();

		if (ext == "fbx")
		{
			if (!FBXSceneManager::getSingleton().resourceExists(_path, "Assets"))
			{
				NameValuePairList lst = { make_pair("Path", _fpath) };
				FBXSceneManager::getSingleton().create(_path, "Assets", false, 0, &lst);
			}
		}

		if (std::find(imgs.begin(), imgs.end(), ext) != imgs.end())
		{
			if (TextureManager::getSingleton().getByName(_path, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME) != nullptr && TextureManager::getSingleton().getByName(_path, "Assets") != nullptr)
			{
				TexturePtr _ptr = TextureManager::getSingleton().getByName(_path, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
				TextureManager::getSingleton().unload(_ptr->getHandle());
				TextureManager::getSingleton().remove(_ptr->getHandle());
			}

			try
			{
				std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
				if (ifs.is_open())
				{
					DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
					Image img;
					img.load(data_stream, ext);

					TexturePtr tex = TextureManager::getSingleton().create(_path, "Assets", true);
					tex->loadImage(img);

					TextureManager::getSingleton().load(_path, "Assets", TEX_TYPE_2D);

					reassignTexture(_path, _path);
				}
			}
			catch (Exception e) { MessageBoxA(0, e.getFullDescription().c_str(), "Error", MB_OK | MB_ICONERROR); }
		}

		if (ext == "material")
		{
			if (!MaterialManager::getSingleton().resourceExists(_path, "Assets"))
			{
				renameScriptResource(_path, "material");

				std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
				if (ifs.is_open())
				{
					Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
					MaterialManager::getSingleton().parseScript(data_stream, "Assets");
					ifs.close();
				}
			}
		}

		if (ext == "cubemap")
		{
			if (!CubemapManager::getSingleton().resourceExists(_path, "Assets"))
			{
				renameScriptResource(_path, "cubemap");

				std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
				if (ifs.is_open())
				{
					Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
					CubemapManager::getSingleton().parseScript(data_stream, "Assets");
					ifs.close();
				}
			}
		}

		if (ext == "glslv")
		{
			if (HighLevelGpuProgramManager::getSingleton().resourceExists(_path, "Assets"))
			{
				HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().getByName(_path, "Assets");
				HighLevelGpuProgramManager::getSingleton().remove(ptrProgram->getHandle());
			}

			if (!HighLevelGpuProgramManager::getSingleton().resourceExists(_path, "Assets"))
			{
				std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
				if (ifs.is_open())
				{
					Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
					String mMasterSource = data_stream->getAsString();

					if (mMasterSource.empty())
					{
						mMasterSource = "attribute vec4 vertex;\n"
							"mat4 worldViewProj;\n"

							"void main()\n"
							"{\n"
							"gl_Position = worldViewProj * vertex;\n"
							"}";
					}

					HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(_path, "Assets", "glsl", GPT_VERTEX_PROGRAM);
					ptrProgram->setParameter("profiles", "glsl120");
					ptrProgram->setSource(mMasterSource);
					ptrProgram->load();

					MainWindow::ShaderLog(ptrProgram);

					ifs.close();
				}
			}
		}

		if (ext == "glslf")
		{
			if (HighLevelGpuProgramManager::getSingleton().resourceExists(_path, "Assets"))
			{
				HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().getByName(_path, "Assets");
				HighLevelGpuProgramManager::getSingleton().remove(ptrProgram->getHandle());
			}

			if (!HighLevelGpuProgramManager::getSingleton().resourceExists(_path, "Assets"))
			{
				std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
				if (ifs.is_open())
				{
					Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
					String mMasterSource = data_stream->getAsString();

					if (mMasterSource.empty())
					{
						mMasterSource = "void main()\n"
							"{\n"
							"gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
							"}";
					}

					HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(_path, "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
					ptrProgram->setParameter("profiles", "glsl120");
					ptrProgram->setSource(mMasterSource);
					ptrProgram->load();

					MainWindow::ShaderLog(ptrProgram);

					ifs.close();
				}
			}
		}

		if (ext == "ttf" || ext == "otf")
		{
			if (!FontManager::getSingleton().resourceExists(_path, "Assets"))
			{
				Ogre::FontPtr mFont = FontManager::getSingleton().create(_path, "Assets");
				mFont->_notifyOrigin(_fpath);
				mFont->setType(Ogre::FT_TRUETYPE);
				mFont->setSource(_path);
				mFont->setParameter("size", "18");
				mFont->setParameter("resolution", "192");
				mFont->load();
			}
		}
	}

	if (addedFilesNames.size() > 0)
	{
		ResourceMap::identifyResources();
	}

	if (allFilesNames.size() > 0)
	{
		MainWindow::GetInspectorWindow()->setEditor(nullptr);

		for (auto it = allFilesNames.begin(); it < allFilesNames.end(); ++it)
		{
			string path = *it;

			deleteResource(path);

			TreeNode * del = treeView->getNodeByPath(CP_UNI(path), treeView->getRootNode());
			if (del != NULL)
			{
				treeView->deleteNode(del);
			}
		}

		ResourceMap::identifyResources();
	}

	//Scripts
	addScripts(allFilesNames1);

	allFilesNames.clear();
	addedFilesNames.clear();
	removeFilesNames.clear();
}

void AssetsWindow::sortTree()
{
	MainWindow::AddOnEndUpdateCallback([=]() {
		sortTree(treeView->getRootNode());
	});
}

void AssetsWindow::sortTree(TreeNode* root)
{
	std::vector<TreeNode*> vec1;
	std::vector<TreeNode*> vec2;

	for (auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		std::string _a = MainWindow::GetProjectAssetsDir() + CP_SYS((*it)->getPath());

		if (IO::isDir(_a))
		{
			vec1.push_back(*it);
		}
		else
		{
			vec2.push_back(*it);
		}
	}

	std::sort(vec1.begin(), vec1.end(), [=](TreeNode*& a, TreeNode*& b) -> bool {
		return boost::algorithm::to_lower_copy(a->alias) < boost::algorithm::to_lower_copy(b->alias);
	});

	std::sort(vec2.begin(), vec2.end(), [=](TreeNode*& a, TreeNode*& b) -> bool {
		return boost::algorithm::to_lower_copy(a->alias) < boost::algorithm::to_lower_copy(b->alias);
	});

	vec1.insert(vec1.end(), vec2.begin(), vec2.end());

	root->children.clear();
	root->children = vec1;

	vec1.clear();
	vec2.clear();

	for (auto it = root->children.begin(); it != root->children.end(); ++it)
		sortTree(*it);
}

std::vector<std::string> AssetsWindow::getImagesFileFormats()
{
	return { "jpg", "png", "jpeg", "bmp", "psd", "tga", "gif", "pic", "ppm", "pgm", "hdr", "dds" };
}

std::vector<string> AssetsWindow::addNewFiles(std::vector<string> files)
{
	std::vector<string> filesNames;
	std::vector<string> filesNames1;

	struct LambdaFunc
	{
		std::vector<string>* filesNames;

		void operator() (std::string dir)
		{
			string str = IO::RemovePart(dir, MainWindow::GetProjectAssetsDir());

			if (str.find("Cache") > 0)
			{
				filesNames->push_back(str);
			}
		}

		void operator() (std::string dir, std::string filename)
		{
			string str = IO::RemovePart(dir + filename, MainWindow::GetProjectAssetsDir());

			if (str.find("Cache") > 0)
			{
				filesNames->push_back(str);
			}
		}
	};

	LambdaFunc func;
	func.filesNames = &filesNames;

	IO::listFiles(MainWindow::GetProjectAssetsDir(), true, func, func);

	filesNames1 = filesNames;

	for (auto it = files.begin(); it != files.end(); ++it)
	{
		auto _it = std::find(filesNames.begin(), filesNames.end(), *it);
		if (_it != filesNames.end())
			filesNames.erase(_it);
	}

	for (auto it = filesNames.begin(); it != filesNames.end(); ++it)
	{
		std::string path = *it;
		if (treeView->getNodeByPath(CP_UNI(path), treeView->getRootNode()) == nullptr)
		{
			std::vector<std::string> results;
			boost::split(results, path, [](char c) { return c == '/' || c == '\\'; });

			TreeNode* item = treeView->getRootNode();

			for (auto it2 = results.begin(); it2 != results.end(); ++it2)
			{
				TreeNode * newItem = treeView->getNodeByAlias(CP_UNI(*it2), item, false);

				if (newItem == nullptr)
				{
					string ext = IO::GetFileExtension(*it2);

					TexturePtr icon = getIconByExtension(ext);

					bool isDir = IO::isDir(MainWindow::GetProjectAssetsDir() + path);

					if (isDir)
						icon = TextureManager::getSingleton().load("Icons/Assets/folder.png", "Editor");

					TreeNode* newNode = new TreeNode(treeView);
					newNode->alias = CP_UNI(*it2);
					newNode->name = ResourceMap::genGuid();
					newNode->icon = icon;
					newNode->enableDrag = true;
					newNode->format = ext;
					newNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
					newNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode * node, int val) { onNodePopupMenu(node, val); });
					newNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });
					newNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
					newNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);

					if (isDir)
					{
						newNode->setSupportedFormats({ "*.", "::SceneNode" });
						newNode->setOnDropCallback([=](TreeNode* node, TreeNode* from) { onDropNode(node, from); });
					}

					item->addChild(newNode);
					item = newNode;
				}
				else
				{
					item = newItem;
				}
			}

			results.clear();
		}
	}

	sortTree();

	return filesNames1;
}

void AssetsWindow::addScripts(std::vector<string> files)
{
	std::vector<string> scripts = files;

	if (files.size() == 0)
		rememberAllItems(treeView->getRootNode(), scripts);

	scripts.erase(remove_if(begin(scripts), end(scripts), [&](auto x)
	{
		return x.find(".cs") == string::npos;
	}), end(scripts));

	std::vector<string> _scripts;
	for (std::vector<string>::iterator it = scripts.begin(); it != scripts.end(); ++it)
	{
		std::string _str = "..\\Assets\\" + IO::ReplaceFrontSlashes(*it);
		_scripts.push_back(_str);
	}

	files.clear();
	scripts.clear();

	SolutionWorker solutionWorker;
	solutionWorker.CreateSolution(MainWindow::GetProjectSolutionDir(), MainWindow::GetProjectName(), _scripts);
}

void AssetsWindow::deleteResource(string path, bool checkFolder)
{
	MapIterator managers = ResourceGroupManager::getSingleton().getResourceManagerIterator();
	ResourcePtr resource;

	TreeNode* dirNode = treeView->getNodeByPath(CP_UNI(path), treeView->getRootNode());
	if (dirNode != nullptr && dirNode->children.size() > 0)
	{
		if (checkFolder)
		{
			if (dirNode != nullptr)
			{
				for (auto it = dirNode->children.begin(); it != dirNode->children.end(); ++it)
				{
					string _path = (*it)->getPath();
					deleteResource(_path, true);
				}
			}
		}
	}
	else
	{
		while (managers.hasMoreElements())
		{
			ResourceManager* manager = managers.getNext();

			if (manager->resourceExists(path, "Assets"))
			{
				resource = manager->getResourceByName(path, "Assets");
				manager->unload(resource->getHandle());
				manager->remove(resource->getHandle());
				break;
			}

			if (manager->resourceExists(IO::GetFileName(path), "Assets"))
			{
				path = IO::GetFileName(path);
				resource = manager->getResourceByName(path, "Assets");
				manager->unload(resource->getHandle());
				manager->remove(resource->getHandle());
				break;
			}
		}
	}
}

void AssetsWindow::renameScriptResource(string path, string resourceType)
{
	string line;
	string txt;
	ifstream _file(MainWindow::GetProjectAssetsDir() + path);
	if (_file.is_open())
	{
		while (getline(_file, line))
		{
			txt += line + '\n';
		}
		_file.close();
	}

	int p = txt.find(resourceType);
	if (p != string::npos)
	{
		int p2 = txt.find("{");
		if (p2 != string::npos)
		{
			string _out = "";

			if (p + resourceType.length() < txt.length())
			{
				string _resPath = txt.substr(p + resourceType.length() + 1, p2 - resourceType.length() - 2);

				if (_resPath.find(" ") != string::npos)
				{
					_out = resourceType + " \"" + path + "\"\n" +
						txt.substr(p2);
				}
				else
				{
					_out = resourceType + " " + path + "\n" +
						txt.substr(p2);
				}

				ofstream _file2(MainWindow::GetProjectAssetsDir() + path);
				if (_file2.is_open())
				{
					_file2 << _out;
					_file2.close();
				}
			}
		}
	}
}

void AssetsWindow::rememberExpandedItems(TreeNode* root)
{
	if (root->expanded)
		expandedItems.push_back(treeView->getNodePath(root));

	for (auto it = root->children.begin(); it != root->children.end(); ++it)
		rememberExpandedItems(*it);
}

void AssetsWindow::rememberAllItems(TreeNode* root, std::vector<string>& list)
{
	string fName = CP_SYS(treeView->getNodePath(root));

	if (fName != "")
		list.push_back(fName);

	for (auto it = root->children.begin(); it != root->children.end(); ++it)
		rememberAllItems(*it, list);
}

void serializeMaterial(MaterialPtr mat)
{
	if (!mat->getOrigin().empty())
	{
		if (IO::isDir(MainWindow::GetProjectAssetsDir() + IO::GetFilePath(mat->getOrigin())))
		{
			MaterialSerializer serializer;

			MaterialPtr nMat = mat->clone(mat->getName() + "_temp_" + to_string(rand()));

			while (nMat->getNumTechniques() > 1)
			{
				nMat->removeTechnique(1);
			}

			serializer.exportMaterial(nMat, MainWindow::GetProjectAssetsDir() + mat->getOrigin(), false, false, "", mat->getName());

			MaterialManager::getSingleton().remove(nMat);
		}
	}
}

void AssetsWindow::createFolder(TreeNode * selNode)
{
	if (treeView->getSelectedNodes().size() <= 1)
	{
		TreeNode* root = treeView->getRootNode();

		if (selNode != nullptr)
			root = selNode;
		else
		{
			auto nodes = treeView->getSelectedNodes();

			if (nodes.size() == 1)
				root = treeView->getNodeByName(nodes[0], treeView->getRootNode());
		}

		root->expanded = true;

		if (!IO::isDir(MainWindow::GetProjectAssetsDir() + CP_SYS(root->getPath())))
		{
			root = root->parent;
		}

		addedAssetNode = new TreeNode(treeView);
		addedAssetNode->name = "##" + ResourceMap::genGuid();
		addedAssetNode->icon = getIconByExtension("");
		addedAssetNode->enableDrag = true;
		addedAssetNode->format = "";
		addedAssetNode->setSupportedFormats({ "*.", "::SceneNode" });
		addedAssetNode->setOnDropCallback([=](TreeNode* node, TreeNode* from) { onDropNode(node, from); });
		addedAssetNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
		addedAssetNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
		addedAssetNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);
		addedAssetNode->setEditMode(true);
		addedAssetNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });
		addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool {
			string name = CP_SYS(node->alias);
			string _path = CP_SYS(node->getPath());

			if (name.find("/") != string::npos || name.find("\\") != string::npos)
				name = "";

			if (!name.empty() && !IO::DirExists(MainWindow::GetProjectAssetsDir() + _path))
			{
				IO::CreateDir(MainWindow::GetProjectAssetsDir() + _path);
				if (IO::FileExists(MainWindow::GetProjectAssetsDir() + _path))
				{
					sortTree();

					addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
					addedAssetNode = nullptr;
					updateFiles = true;
				}
				else
				{
					deleteAssetNode = true;
					MessageBoxA(MainWindow::GetWindowHandle(), "Incorrect folder name!", "Error", MB_OK);
				}
			}
			else
			{
				deleteAssetNode = true;
			}
			
			return true;
		});

		root->addChild(addedAssetNode);
	}
}

void AssetsWindow::createMaterial(TreeNode* selNode, AssetsTools::MaterialType type)
{
	if (treeView->getSelectedNodes().size() <= 1)
	{
		TreeNode* root = treeView->getRootNode();

		if (selNode != nullptr)
			root = selNode;
		else
		{
			auto nodes = treeView->getSelectedNodes();

			if (nodes.size() == 1)
				root = treeView->getNodeByName(nodes[0], treeView->getRootNode());
		}

		root->expanded = true;

		if (!IO::isDir(MainWindow::GetProjectAssetsDir() + CP_SYS(root->getPath())))
		{
			root = root->parent;
		}

		addedAssetNode = new TreeNode(treeView);
		addedAssetNode->name = "##" + ResourceMap::genGuid();
		addedAssetNode->icon = getIconByExtension("material");
		addedAssetNode->enableDrag = true;
		addedAssetNode->format = "material";
		addedAssetNode->setEditMode(true);
		addedAssetNode->setPopupMenu({ "Rename", "Delete", "Show in Explorer" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
		addedAssetNode->setCustomPopupMenuProcedure([=](TreeNode* node) { updateAssetsPopup(true, node); });
		addedAssetNode->setCustomPopupMenuPosition(TreeNode::CustomPopupMenuPosition::CPMP_BEGIN);
		addedAssetNode->setOnDoubleClickCallback([=](TreeNode* node) { onNodeDoubleClick(node); });

		addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool {
			string name = CP_SYS(node->alias);

			string _path = CP_SYS(root->getPath());
			if (!_path.empty()) _path = _path + "/";

			if (name.find("/") != string::npos || name.find("\\") != string::npos)
				name = "";

			if (!name.empty() && !IO::FileExists(MainWindow::GetProjectAssetsDir() + _path + name + ".material"))
			{
				MaterialPtr newMat = AssetsTools::CreateMaterial(name, _path, type);
				if (IO::FileExists(MainWindow::GetProjectAssetsDir() + _path + name + ".material"))
				{
					addedAssetNode->alias += ".material";
					addedAssetNode->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
					addedAssetNode = nullptr;
					sortTree();
				}
				else
				{
					deleteAssetNode = true;
					MaterialManager::getSingleton().remove(newMat);
					MessageBoxA(MainWindow::GetWindowHandle(), "Incorrect file name!", "Error", MB_OK);
				}
			}
			else
			{
				deleteAssetNode = true;
			}

			return true;
		});

		root->addChild(addedAssetNode);
	}
}

void AssetsWindow::onDropNode(TreeNode* node, TreeNode* from)
{
	if (from->treeView == treeView)
	{
		MainWindow::GetPreviewWindow()->clearPreview();

		auto items = treeView->getSelectedNodes();
		onNodesMoved(items, node);

		MainWindow::GetHierarchyWindow()->refreshHierarchy();

		//reloadFiles();
		//MainWindow::gizmo2->callSelectCallback();
	}
	else
	{
		if (from->treeView->getTag() == "Hierarchy")
		{
			string nodePath = CP_SYS(node->getPath());

			std::string fpath = MainWindow::GetProjectAssetsDir() + nodePath;
			if (!nodePath.empty())
				fpath += "/";

			std::string path = fpath + from->alias + ".prefab";

			if (IO::DirExists(fpath))
			{
				if (!IO::FileExists(path))
				{
					SceneSerializer serializer;
					SceneNode* prefabNode = GetEngine->GetSceneManager()->getSceneNode(from->name);
					serializer.SerializeToPrefab(prefabNode, path);

					MainWindow::GetHierarchyWindow()->refreshHierarchy();
					addNewFiles();
					MainWindow::GetInspectorWindow()->updateCurrentEditor();
				}
				else
				{
					MessageBoxA(nullptr, "Prefab already exists at this location. Use \"Apply to prefab\" functionality instead.", "Error", MB_OK | MB_ICONERROR);
				}
			}
		}
	}
}

std::vector<bool> AssetsWindow::onNodesMoved(std::vector<std::string> items, TreeNode* moveto)
{
	TreeNode* root = treeView->getRootNode();

	std::vector<bool> result;

	movingFiles = true;
	for (auto it = items.begin(); it != items.end(); ++it)
	{
		std::string path = CP_SYS(moveto->getPath());

		if (IO::isDir(MainWindow::GetProjectAssetsDir() + path))
		{
			TreeNode* curNode = treeView->getNodeByName(*it, root);

			std::string dir = CP_SYS(curNode->getPath());
			if (IO::isDir(MainWindow::GetProjectAssetsDir() + dir))
			{
				if (!path.empty())
					path += "/";

				if (IO::RemoveLastLocation(CP_SYS(curNode->getPath())) != path)
				{
					moveFolder(curNode, path);

					std::vector<string> _oldFiles;
					IO::listFiles(MainWindow::GetProjectAssetsDir() + dir, true, nullptr, [=](string d, string f) {
						const_cast<std::vector<string>*>(&_oldFiles)->push_back(d + f);
					});

					if (_oldFiles.size() == 0)
						IO::DirDeleteRecursive(MainWindow::GetProjectAssetsDir() + dir);
				}
			}
			else
			{
				if (!path.empty())
					path += "/";

				std::string oldFullPath = CP_SYS(curNode->getPath());
				std::string newFullPath = path + CP_SYS(curNode->alias);

				if (curNode->alias.find("/") != string::npos || curNode->alias.find("\\") != string::npos)
					newFullPath = oldFullPath;

				if (oldFullPath != newFullPath)
				{
					bool ret = moveAsset(oldFullPath, newFullPath, FOP_MOVE);

					if (!ret)
					{
						movingFiles = false;

						if (MessageBoxA(0, "Error moving asset(s). It might be blocked by another process", "Error", MB_ICONERROR | MB_RETRYCANCEL) == IDRETRY)
						{
							onNodesMoved(items, moveto);
						}
					}

					result.push_back(ret);
				}
				else
				{
					result.push_back(false);
				}
			}
		}
		else
		{
			movingFiles = false;

			result.push_back(false);
		}
	}

	movingFiles = false;
	
	return result;
}

bool AssetsWindow::moveAsset(std::string oldPath, std::string newPath, FileOp op)
{
	std::string oldNameWithExt = IO::GetFileNameWithExt(oldPath);
	std::string oldFullPath = oldPath;
	std::string newFullPath = newPath;

	if (oldFullPath == newFullPath)
		return true;

	std::string ext = IO::GetFileExtension(oldNameWithExt);

	std::vector<String> imgs = getImagesFileFormats();

	if (ext == "material")
	{
		if (!MaterialManager::getSingleton().resourceExists(newFullPath, "Assets"))
		{
			//MessageBoxA(0, "Material with this name already exists!", "Error", MB_ICONERROR | MB_OK);
			//return false;

			MaterialPtr material = MaterialManager::getSingleton().getByName(oldFullPath, "Assets");
			if (material == nullptr)
				material = MaterialManager::getSingleton().getByName(IO::GetFileName(oldFullPath), "Assets");

			if (material != nullptr)
			{
				bool _err = false;
				if (op == FOP_RENAME)
				{
					if (IO::FileExists(MainWindow::GetProjectAssetsDir() + material->getOrigin()))
					{
						boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + material->getOrigin(), MainWindow::GetProjectAssetsDir() + newFullPath);
						if (err.value() != 0)
						{
							MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
							return false;
						}
					}
				}
				else
				{
					if (IO::FileRename(MainWindow::GetProjectAssetsDir() + material->getOrigin(), MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
						_err = true;
				}

				if (!_err)
				{
					MaterialPtr newMaterial = material->clone(newFullPath);
					newMaterial->_notifyOrigin(newFullPath);

					//Search all entities with this material
					MapIterator it = GetEngine->GetSceneManager()->getMovableObjectIterator(EntityFactory::FACTORY_TYPE_NAME);

					while (it.hasMoreElements())
					{
						Entity* entity = (Entity*)it.getNext();

						for (int i = 0; i < entity->getSubEntities().size(); ++i)
						{
							SubEntity* subEntity = entity->getSubEntity(i);
							if (subEntity->getOriginalMaterial() == material)
							{
								subEntity->setOriginalMaterial(newMaterial);
							}
						}
					}

					serializeMaterial(newMaterial);

					string _oldName = material->getName();
					MaterialManager::getSingleton().unload(material->getHandle());
					MaterialManager::getSingleton().remove(material->getHandle());

					ResourceMap::setResourceName(_oldName, newFullPath);
				}
				else
					return false;
			}
		}
		else
			return false;

		return true;
	}
	else if (ext == "fbx")
	{
		if (!FBXSceneManager::getSingleton().resourceExists(newFullPath, "Assets"))
		{
			bool _err = false;
			if (op == FOP_RENAME)
			{
				if (IO::FileExists(MainWindow::GetProjectAssetsDir() + oldFullPath))
				{
					boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath);
					if (err.value() != 0)
					{
						MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
						return false;
					}
				}
			}
			else
			{
				if (IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
					_err = true;
			}

			if (!_err)
			{
				if (FBXSceneManager::getSingleton().resourceExists(oldFullPath, "Assets"))
					FBXSceneManager::getSingleton().remove(oldFullPath, "Assets");

				NameValuePairList lst = { make_pair("Path", MainWindow::GetProjectAssetsDir() + newFullPath) };
				FBXSceneManager::getSingleton().create(newFullPath, "Assets", false, 0, &lst);

				ResourceMap::setResourceName(oldFullPath, newFullPath);
			}
			else
				return false;
		}
		else
			return false;

		return true;
	}
	else if (std::find(imgs.begin(), imgs.end(), ext) != imgs.end())
	{
		bool _err = false;
		if (op == FOP_RENAME)
		{
			if (IO::FileExists(MainWindow::GetProjectAssetsDir() + oldFullPath))
			{
				boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath);
				if (err.value() != 0)
				{
					MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
					return false;
				}
			}
		}
		else
		{
			if (IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
			{
				_err = true;
			}
		}

		if (!_err)
		{
			if (TextureManager::getSingleton().resourceExists(oldFullPath, "Assets"))
				TextureManager::getSingleton().remove(oldFullPath, "Assets");

			if (TextureManager::getSingleton().resourceExists(newFullPath, "Assets"))
				TextureManager::getSingleton().remove(newFullPath, "Assets");

			string _fpath = MainWindow::GetProjectAssetsDir() + newFullPath;
			std::ifstream ifs(_fpath.c_str(), std::ios::binary | std::ios::in);
			if (ifs.is_open())
			{
				Ogre::DataStreamPtr data_stream(new Ogre::FileStreamDataStream(_fpath, &ifs, false));
				Ogre::Image img;
				img.load(data_stream, ext);
				Ogre::TextureManager::getSingleton().loadImage(newFullPath, "Assets", img);
				ifs.close();
			}

			//Reassign the link to this texture in ALL materials
			reassignTexture(oldFullPath, newFullPath);

			ResourceMap::setResourceName(oldFullPath, newFullPath);
		}
		else
			return false;

		return true;
	}
	else if (ext == "glslv" || ext == "glslf")
	{
		if (!HighLevelGpuProgramManager::getSingleton().resourceExists(newFullPath, "Assets"))
		{
			HighLevelGpuProgramPtr shader = HighLevelGpuProgramManager::getSingleton().getByName(oldFullPath, "Assets");

			if (shader != nullptr)
			{
				bool _err = false;
				if (op == FOP_RENAME)
				{
					if (IO::FileExists(MainWindow::GetProjectAssetsDir() + oldFullPath))
					{
						boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath);

						if (err.value() != 0)
						{
							MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
							return false;
						}
					}
				}
				else
				{
					if (IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
						_err = true;
				}

				if (!_err)
				{
					string _oldPath = shader->getOrigin();
					string src = shader->getSource();

					HighLevelGpuProgramManager::getSingleton().remove(oldFullPath, "Assets");

					GpuProgramType progType = GPT_VERTEX_PROGRAM;
					if (ext == "glslv")
						progType = GPT_VERTEX_PROGRAM;
					else
						progType = GPT_FRAGMENT_PROGRAM;

					HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(newFullPath, "Assets", "glsl", progType);
					ptrProgram->setSource(src);
					ptrProgram->load();
					IO::FileDelete(MainWindow::GetProjectAssetsDir() + _oldPath);

					//Reassign the link to this texture in ALL materials
					MapIterator mt = MaterialManager::getSingleton().getResourceIterator();
					while (mt.hasMoreElements())
					{
						MaterialPtr mat = static_pointer_cast<Material>(mt.getNext());
						if (!mat->getOrigin().empty())
						{
							bool hasChanges = false;

							for (int i = 0; i < mat->getNumTechniques(); ++i)
							{
								Technique* t = mat->getTechnique(i);
								for (int j = 0; j < t->getNumPasses(); ++j)
								{
									Pass* p = t->getPass(j);
									if (ext == "glslv")
									{
										if (p->hasVertexProgram())
										{
											if (p->getVertexProgramName() == oldFullPath)
											{
												p->setVertexProgram(newFullPath, false);
												hasChanges = true;
											}
										}
									}
									else
									{
										if (p->hasFragmentProgram())
										{
											if (p->getFragmentProgramName() == oldFullPath)
											{
												p->setFragmentProgram(newFullPath, false);
												hasChanges = true;
											}
										}
									}
								}
							}

							if (hasChanges)
							{
								serializeMaterial(mat);
							}
						}
					}

					ResourceMap::setResourceName(oldFullPath, newFullPath);
				}
				else
					return false;
			}
		}

		return true;
	}
	else if (ext == "ttf" || ext == "otf")
	{
		if (!FontManager::getSingleton().resourceExists(newFullPath, "Assets"))
		{
			Ogre::FontPtr font = FontManager::getSingleton().getByName(oldFullPath, "Assets");

			if (font != nullptr)
			{
				bool _err = false;
				if (op == FOP_RENAME)
				{
					if (IO::FileExists(MainWindow::GetProjectAssetsDir() + oldFullPath))
					{
						boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath);
						if (err.value() != 0)
						{
							MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
							return false;
						}
					}
				}
				else
				{
					if (IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
						_err = true;
				}

				if (!_err)
				{
					FontManager::getSingleton().remove(oldFullPath, "Assets");

					Ogre::FontPtr mFont = FontManager::getSingleton().create(newFullPath, "Assets");
					mFont->setType(Ogre::FT_TRUETYPE);
					mFont->setSource(newFullPath);
					mFont->setParameter("size", "18");
					mFont->setParameter("resolution", "192");
					mFont->load();

					ResourceMap::setResourceName(oldFullPath, newFullPath);
				}
				else
					return false;
			}
		}

		return true;
	}
	else if (ext == "cubemap")
	{
		if (!CubemapManager::getSingleton().resourceExists(newFullPath, "Assets"))
		{
			CubemapPtr cubemap = CubemapManager::getSingleton().getByName(oldFullPath, "Assets");

			if (cubemap != nullptr)
			{
				bool _err = false;
				if (op == FOP_RENAME)
				{
					if (IO::FileExists(MainWindow::GetProjectAssetsDir() + cubemap->getOrigin()))
					{
						boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + cubemap->getOrigin(), MainWindow::GetProjectAssetsDir() + newFullPath);
						if (err.value() != 0)
						{
							MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
							return false;
						}
					}
				}
				else
				{
					if (IO::FileRename(MainWindow::GetProjectAssetsDir() + cubemap->getOrigin(), MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
						_err = true;
				}

				if (!_err)
				{
					CubemapPtr newCubemap = CubemapManager::getSingleton().create(newFullPath, "Assets");
					newCubemap->_notifyOrigin(newFullPath);

					newCubemap->SetTextureBackName(cubemap->GetTextureBackName());
					newCubemap->SetTextureDownName(cubemap->GetTextureDownName());
					newCubemap->SetTextureFrontName(cubemap->GetTextureFrontName());
					newCubemap->SetTextureLeftName(cubemap->GetTextureLeftName());
					newCubemap->SetTextureRightName(cubemap->GetTextureRightName());
					newCubemap->SetTextureUpName(cubemap->GetTextureUpName());

					//Replace link to this cubemap in ALL materials
					MapIterator mt = MaterialManager::getSingleton().getResourceIterator();
					while (mt.hasMoreElements())
					{
						MaterialPtr mat = static_pointer_cast<Material>(mt.getNext());
						if (!mat->getOrigin().empty())
						{
							bool hasChanges = false;

							for (int i = 0; i < mat->getNumTechniques(); ++i)
							{
								Technique* t = mat->getTechnique(i);
								for (int j = 0; j < t->getNumPasses(); ++j)
								{
									Pass* p = t->getPass(j);
									for (int k = 0; k < p->getNumTextureUnitStates(); ++k)
									{
										TextureUnitState* tu = p->getTextureUnitState(k);

										if (tu->getCubemap() == cubemap)
										{
											tu->setCubemap(newCubemap);
											hasChanges = true;
										}
									}
								}
							}

							if (hasChanges)
							{
								serializeMaterial(mat);
							}
						}
					}

					CubemapSerializer serializer;
					serializer.exportCubemap(newCubemap, MainWindow::GetProjectAssetsDir() + newFullPath);

					CubemapManager::getSingleton().unload(cubemap->getHandle());
					CubemapManager::getSingleton().remove(cubemap->getHandle());

					ResourceMap::setResourceName(oldFullPath, newFullPath);
				}
				else
					return false;
			}
		}
		else
			return false;

		return true;
	}
	else
	{
		if (IO::FileExists(MainWindow::GetProjectAssetsDir() + oldFullPath))
		{
			bool _err = false;
			if (op == FOP_RENAME)
			{
				boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath);
				if (err.value() != 0)
				{
					MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
					return false;
				}
			}
			else
			{
				if (IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath).value() != 0)
					_err = true;
			}

			if (!_err)
			{
				ResourceMap::setResourceName(oldFullPath, newFullPath);
			}
			else
				return false;
		}

		return true;
	}

	return false;
}

void AssetsWindow::reassignTexture(std::string oldFullPath, std::string newFullPath)
{
	MapIterator mt = MaterialManager::getSingleton().getResourceIterator();
	while (mt.hasMoreElements())
	{
		MaterialPtr mat = static_pointer_cast<Material>(mt.getNext());

		bool hasChanges = false;

		for (int i = 0; i < mat->getNumTechniques(); ++i)
		{
			Technique* t = mat->getTechnique(i);
			for (int j = 0; j < t->getNumPasses(); ++j)
			{
				Pass* p = t->getPass(j);
				for (int k = 0; k < p->getNumTextureUnitStates(); ++k)
				{
					TextureUnitState* tu = p->getTextureUnitState(k);

					if (tu->getTextureName() == oldFullPath)
					{
						TexturePtr _tex = TextureManager::getSingleton().load(newFullPath, "Assets");
						tu->setTexture(_tex);
						hasChanges = true;
					}
				}
			}
		}

		if (hasChanges)
		{
			MaterialEditor2 * me = new MaterialEditor2();
			me->updateMaterial(MaterialPtr(), mat);
			delete me;

			serializeMaterial(mat);
		}
	}

	//Reassign the link to this texture in ALL cubemaps
	MapIterator ct = CubemapManager::getSingleton().getResourceIterator();
	while (ct.hasMoreElements())
	{
		CubemapPtr cub = static_pointer_cast<Cubemap>(ct.getNext());

		bool hasChanges = false;

		if (cub->GetTextureBackName() == oldFullPath)
		{
			cub->SetTextureBackName(newFullPath);
			hasChanges = true;
		}

		if (cub->GetTextureDownName() == oldFullPath)
		{
			cub->SetTextureDownName(newFullPath);
			hasChanges = true;
		}

		if (cub->GetTextureFrontName() == oldFullPath)
		{
			cub->SetTextureFrontName(newFullPath);
			hasChanges = true;
		}

		if (cub->GetTextureLeftName() == oldFullPath)
		{
			cub->SetTextureLeftName(newFullPath);
			hasChanges = true;
		}

		if (cub->GetTextureRightName() == oldFullPath)
		{
			cub->SetTextureRightName(newFullPath);
			hasChanges = true;
		}

		if (cub->GetTextureUpName() == oldFullPath)
		{
			cub->SetTextureUpName(newFullPath);
			hasChanges = true;
		}

		if (hasChanges)
		{
			if (!cub->getOrigin().empty())
			{
				if (IO::isDir(MainWindow::GetProjectAssetsDir() + IO::GetFilePath(cub->getOrigin())))
				{
					CubemapSerializer ser;
					ser.exportCubemap(cub, MainWindow::GetProjectAssetsDir() + cub->getOrigin());
				}
			}
		}
	}
}

void AssetsWindow::focusOnFile(std::string filepath)
{
	TreeNode* node = getTreeView()->getNodeByPath(filepath, getTreeView()->getRootNode());
	if (node != nullptr)
	{
		getTreeView()->selectNodes({ node->name }, false);
		getTreeView()->focusOnNode(node);
	}
}

bool AssetsWindow::moveFolder(TreeNode* item, std::string newPath)
{
	std::string rootPath = CP_SYS(item->alias);
	std::string oldFullPath = CP_SYS(item->getPath());

	if (!IO::isDir(MainWindow::GetProjectAssetsDir() + oldFullPath))
	{
		moveAsset(oldFullPath, newPath + rootPath, FOP_MOVE);
	}
	else
	{
		rootPath += "/";
		CreateDirectory(CString(MainWindow::GetProjectAssetsDir().c_str()) + CString((newPath + rootPath).c_str()), NULL);

		auto children = item->children;
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			moveFolder(*it, newPath + rootPath);
		}
	}

	return true;
}

bool AssetsWindow::renameFolder(TreeNode* item, std::string oldName)
{
	string oldFullPath = oldName;
	string newFullPath = CP_SYS(item->getPath());

	if (!IO::isDir(MainWindow::GetProjectAssetsDir() + newFullPath))
	{
		moveAsset(oldFullPath, newFullPath, FOP_RENAME);
	}
	else
	{
		oldFullPath += "/";

		auto children = item->children;

		std::map<std::string, int> loadOrder;
		std::vector<std::string> _imgs = getImagesFileFormats();

		loadOrder[""] = 10;

		for (auto it = _imgs.begin(); it != _imgs.end(); ++it)
		{
			loadOrder[*it] = 100;
		}

		loadOrder["cubemap"] = 200;
		loadOrder["material"] = 300;
		loadOrder["fbx"] = 400;
		loadOrder["scene"] = 500;
		/* LOADING ORDER */

		std::sort(children.begin(), children.end(), [=](TreeNode * a, TreeNode * b) -> bool {
			std::string _a = IO::GetFileExtension(a->alias);
			std::string _b = IO::GetFileExtension(b->alias);

			if (loadOrder.find(_a) != loadOrder.end() && loadOrder.find(_b) != loadOrder.end())
				return loadOrder.find(_a)->second < loadOrder.find(_b)->second;
			else return true;
		});
		loadOrder.clear();

		for (auto it = children.begin(); it != children.end(); ++it)
		{
			if (!renameFolder(*it, oldFullPath + (*it)->alias))
			{
				MessageBoxA(MainWindow::GetWindowHandle(), "Incorrect folder name!", "Error", MB_OK);
			}
		}
	}

	return true;
}

bool AssetsWindow::onNodeRenamed(TreeNode* item, string oldName)
{
	if (CP_SYS(item->alias) == oldName)
		return false;

	if (item->alias.find("/") != string::npos || item->alias.find("\\") != string::npos)
		return false;

	MainWindow::GetInspectorWindow()->setEditor(nullptr);

	movingFiles = true;
	bool ret = false;

	string oldFullPath = IO::RemoveLastLocation(CP_SYS(item->getPath())) + oldName;
	//string newFullPath = IO::RemoveLastLocation(CP_SYS(item->getPath())) + CP_SYS(item->alias);
	string newFullPath = CP_SYS(item->getPath());

	if (!IO::isDir(MainWindow::GetProjectAssetsDir() + oldFullPath))
	{
		ret = moveAsset(oldFullPath, newFullPath, FOP_RENAME);
		MainWindow::gizmo2->callSelectCallback();
	}
	else
	{
		boost::system::error_code err = IO::FileRename(MainWindow::GetProjectAssetsDir() + oldFullPath, MainWindow::GetProjectAssetsDir() + newFullPath);
		
		if (err.value() != 0)
		{
			MessageBoxA(0, err.message().c_str(), "Error", MB_ICONERROR | MB_OK);
			movingFiles = false;
			return false;
		}

		if (IO::DirExists(MainWindow::GetProjectAssetsDir() + newFullPath))
		{
			renameFolder(item, oldFullPath);

			movingFiles = false;
			MainWindow::gizmo2->callSelectCallback();
		}

		ret = true;
	}

	movingFiles = false;

	sortTree();

	return ret;
}

void AssetsWindow::onNodePopupMenu(TreeNode* node, int val)
{
	if (val == 0)
	{
		if (treeView->getSelectedNodes().size() <= 1)
		{
			node->setEditMode(true);
		}
	}

	if (val == 1)
	{
		MainWindow::GetInspectorWindow()->setEditor(nullptr);

		auto nodes = treeView->getSelectedNodes();

		if (nodes.size() == 0)
			nodes = { node->name };
		else if (find(nodes.begin(), nodes.end(), node->name) == nodes.end())
			nodes = { node->name };

		bool recompileScripts = false;

		for (auto it = nodes.begin(); it != nodes.end(); ++it)
		{
			TreeNode* curNode = node->treeView->getNodeByName(*it, treeView->getRootNode());
			TreeNode* par = curNode->parent;
			if (find(nodes.begin(), nodes.end(), par->name) != nodes.end())
				continue;

			string path = CP_SYS(curNode->getPath());
			string fileName = MainWindow::GetProjectAssetsDir() + path;
			CString str = CString((fileName).c_str());

			if (fileName != MainWindow::GetProjectAssetsDir())
			{
				//Delete file
				str.Replace(_T('/'), _T('\\'));
				str.AppendChar(_T('\0'));
				str.AppendChar(_T('\0'));

				SHFILEOPSTRUCT f;
				ZeroMemory(&f, sizeof(SHFILEOPSTRUCT));
				f.wFunc = FO_DELETE;
				f.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
				f.pFrom = str;
				f.pTo = NULL;
				f.hNameMappings = NULL;

				int result = SHFileOperation(&f);

				//Delete resource
				deleteResource(path, true);
				deleteNodes.push_back(curNode);

				if (IO::GetFileExtension(fileName) == "cs")
					recompileScripts = true;
			}
		}

		treeView->getSelectedNodes().clear();
		MainWindow::GetHierarchyWindow()->refreshHierarchy();
		MainWindow::GetPreviewWindow()->clearPreview();

		if (recompileScripts)
		{
			MainWindow::Get()->CompileScripts();
		}

		nodes.clear();
	}

	if (val == 2)
	{
		auto nodes = treeView->getSelectedNodes();

		if (nodes.size() == 0)
			nodes = { node->name };
		else if (find(nodes.begin(), nodes.end(), node->name) == nodes.end())
			nodes = { node->name };

		for (auto it = nodes.begin(); it != nodes.end(); ++it)
		{
			TreeNode * node = treeView->getNodeByName(*it, treeView->getRootNode());

			std::string _path = boost::replace_all_copy(MainWindow::GetProjectAssetsDir() + node->getPath(), "/", "\\");
			
			ITEMIDLIST* pidl = ILCreateFromPath(CString(_path.c_str()));
			if (pidl) {
				SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
				ILFree(pidl);
			}
		}
	}
}

void AssetsWindow::onNodeDoubleClick(TreeNode* node)
{
	string path = node->getPath();

	if (IO::GetFileExtension(path) == "scene")
	{
		MainWindow::Get()->OpenScene(MainWindow::GetProjectAssetsDir() + path);
	}

	if (IO::GetFileExtension(path) == "cs")
	{
		CString cmd = CString(("\"" + MainWindow::GetProjectSolutionDir() + MainWindow::GetProjectName() + ".sln\" \"" + MainWindow::GetProjectSolutionDir() + MainWindow::GetProjectName() + ".csproj\" \"" + MainWindow::GetProjectAssetsDir() + CP_SYS(path) + "\" /dde").c_str());
		::ShellExecute(NULL, _T("open"), CString("devenv"), cmd, NULL, 1);
	}
}

void FileSystemWatcher::WatchDirectory(LPCWSTR path)
{
	char buf[2048];
	DWORD nRet;
	BOOL result = TRUE;
	char filename[MAX_PATH];
	DirInfo[0].hDir = CreateFile(path, GENERIC_READ | FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);

	if (DirInfo[0].hDir == INVALID_HANDLE_VALUE)
	{
		return; //cannot open folder
	}

	lstrcpy(DirInfo[0].lpszDirName, path);
	OVERLAPPED PollingOverlap;

	FILE_NOTIFY_INFORMATION* pNotify;
	int offset;
	PollingOverlap.OffsetHigh = 0;
	PollingOverlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	while (result)
	{
		result = ReadDirectoryChangesW(
			DirInfo[0].hDir,// handle to the directory to be watched
			&buf,// pointer to the buffer to receive the read results
			sizeof(buf),// length of lpBuffer
			TRUE,// flag for monitoring directory or directory tree
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_LAST_ACCESS |
			FILE_NOTIFY_CHANGE_CREATION,
			&nRet,// number of bytes returned
			&PollingOverlap,// pointer to structure needed for overlapped I/O
			NULL);

		WaitForSingleObject(PollingOverlap.hEvent, INFINITE);
		offset = 0;
		int rename = 0;
		char oldName[260];
		char newName[260];
		do
		{
			pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buf + offset);
			strcpy(filename, "");
			int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
			filename[pNotify->FileNameLength / 2];
			string fn = string(filename, pNotify->FileNameLength / 2);
			fn = IO::ReplaceBackSlashes(fn);

			string ext = IO::GetFileExtension(fn);

			switch (pNotify->Action)
			{
			case FILE_ACTION_ADDED:
				if (ext == "cs" || ext.find("~") != string::npos)
				{
					MainWindow::Get()->setScriptsChanged(true);
				}

				break;

			case FILE_ACTION_REMOVED:
				if (ext == "cs" || ext.find("~") != string::npos)
				{
					MainWindow::Get()->setScriptsChanged(true);
				}

				break;

			case FILE_ACTION_MODIFIED:
				if (ext == "cs" || ext.find("~") != string::npos)
				{
					MainWindow::Get()->setScriptsChanged(true);
				}
				else
				{
					MainWindow::Get()->addChangedAsset(fn);
				}

				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				if (ext != "cs" && ext.find("~") == string::npos)
				{
					MainWindow::Get()->addChangedAsset(fn);
				}
				break;
			default:
				break;
			}

			offset += pNotify->NextEntryOffset;

		} while (pNotify->NextEntryOffset);
	}

	CloseHandle(DirInfo[0].hDir);
}

BOOL FileSystemWatcher::InitInstance()
{
	while (work)
	{
		WatchDirectory(CString(MainWindow::GetProjectAssetsDir().c_str()));
	}

	return true;
}