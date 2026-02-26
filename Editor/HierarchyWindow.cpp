#include "stdafx.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "HierarchyWindow.h"
#include "AssetsWindow.h"
#include "MainWindow.h"
#include "TreeView.h"
#include "TreeNode.h"
#include "PreviewWindow.h"
#include "ConsoleWindow.h"

#include <OgreVector2.h>
#include <OgreEntity.h>
#include <OgreLight.h>
#include <OgreCamera.h>
#include <OgreParticleSystem.h>

#include "../Engine/Engine.h"
#include "../Engine/StringConverter.h"
#include "../Engine/ResourceMap.h"
#include "../Engine/IO.h"
#include "../Engine/DeferredShading/DeferredLightCP.h"

using namespace std;
using namespace Ogre;

HierarchyWindow::HierarchyWindow()
{
	treeView = new TreeView();
	treeView->setTag("Hierarchy");
	treeView->setAllowMoveNodes(true);
	treeView->setProcessDragDropInTarget(true);
	treeView->setAllowReorder(true);
	treeView->setSelectCallback(onNodesSelected);
	treeView->setOnEndUpdateCallback([=]() { onTreeViewEndUpdate(); });
	treeView->setOnBeginDragCallback([=](TreeNode* node) { onNodeBeginDrag(node); });
	treeView->setOnReorderCallback([=](TreeNode* node, int newIndex) { onReorder(node, newIndex); });

	treeView->getRootNode()->setSupportedFormats({ "*.", "::SceneNode" });
	treeView->getRootNode()->setOnDropCallback([=](TreeNode* node, TreeNode* from) { onDropNode(node, from); });
}

HierarchyWindow::~HierarchyWindow()
{
	delete treeView;
}

void HierarchyWindow::init()
{
	addIcon = TextureManager::getSingleton().load("Icons/Toolbar/add.png", "Editor");
	closeIcon = TextureManager::getSingleton().load("Icons/Hierarchy/close.png", "Editor");
}

void HierarchyWindow::onTreeViewEndUpdate()
{

}

void HierarchyWindow::update()
{
	bool rootOpened = true;

	if (opened)
	{
		if (ImGui::Begin("Hierarchy", &opened, ImGuiWindowFlags_NoCollapse))
		{
			updateObjectMenu();

			/* TREE VIEW */

			ImGuiWindow* window = GImGui->CurrentWindow;
			ImGuiID id = window->GetIDNoKeepAlive("##HierarchyVS");
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

void HierarchyWindow::addNode(SceneNode* objectNode, string parentName, bool recursive)
{
	if (objectNode != NULL)
	{
		if (objectNode->getName().empty())
			return;

		if (GetEngine->IsEditorObject(objectNode))
			return;

		if (objectNode->getAttachedObjects().size() == 0)
			return;

		string s1 = objectNode->getAlias().c_str();

		TexturePtr icon;

		MovableObject* obj = objectNode->getAttachedObject(0);

		if (dynamic_cast<Entity*>(obj))
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/entity.png", "Editor");
		if (dynamic_cast<Light*>(obj))
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/light.png", "Editor");
		if (dynamic_cast<Empty*>(obj))
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/empty.png", "Editor");
		if (dynamic_cast<Camera*>(obj))
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/camera.png", "Editor");
		if (obj->getMovableType() == UICanvasFactory::FACTORY_TYPE_NAME)
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/canvas.png", "Editor");
		if (obj->getMovableType() == UIImageFactory::FACTORY_TYPE_NAME)
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/image.png", "Editor");
		if (obj->getMovableType() == UITextFactory::FACTORY_TYPE_NAME)
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/text.png", "Editor");
		if (obj->getMovableType() == UIButtonFactory::FACTORY_TYPE_NAME)
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/button.png", "Editor");
		if (dynamic_cast<ParticleSystem*>(obj))
			icon = TextureManager::getSingleton().load("Icons/Hierarchy/particles.png", "Editor");

		TreeNode* node = new TreeNode(treeView);
		node->name = CP_UNI(objectNode->getName());
		node->alias = CP_UNI(objectNode->getAlias());
		node->enableDrag = true;
		node->format = "::SceneNode";
		node->setSupportedFormats({ "::SceneNode" });
		node->setOnDropCallback([=](TreeNode* node, TreeNode* from) { onDropNode(node, from); });
		node->setOnEndEditCallback([=](TreeNode* node, string prevAlias) -> bool { return onNodeRenamed(node, prevAlias); });
		node->setPopupMenu({ "Rename", "Delete" }, [=](TreeNode* node, int val) { onNodePopupMenu(node, val); });
		node->icon = icon;

		bool _prefab = objectNode->prefabGuid != "" || objectNode->prefabName != "";

		if (_prefab)
		{
			string _prefabName = ResourceMap::getResourceNameFromGuid(objectNode->prefabGuid);
			if (_prefabName.empty())
				_prefabName = objectNode->prefabName;

			ImVec4 color = ImVec4(0.41f, 0.69f, 0.90f, 1.0f);

			if (!IO::FileExists(MainWindow::GetProjectAssetsDir() + _prefabName))
				color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);

			node->color[0] = color.x;
			node->color[1] = color.y;
			node->color[2] = color.z;
			node->color[3] = color.w;
		}

		if (!objectNode->getVisible())
			node->color[3] = 0.5f;

		if (!parentName.empty())
		{
			TreeNode* parent = treeView->getNodeByName(parentName, treeView->getRootNode());

			if (parent != NULL)
			{
				parent->addChild(node);
			}
			else
			{
				treeView->getRootNode()->addChild(node);
			}
		}
		else
		{
			treeView->getRootNode()->addChild(node);
		}

		if (recursive)
		{
			Node::ChildNodeIterator itr = objectNode->getChildIterator();

			while (itr.hasMoreElements())
			{
				Node* child = itr.getNext();

				addNode((SceneNode*)child, parentName, true);
			}
		}
	}
}

void HierarchyWindow::refreshHierarchy()
{
	auto states = MainWindow::GetHierarchyWindow()->getTreeView()->getItemsExpandStates();

	TreeView* tv = MainWindow::GetHierarchyWindow()->getTreeView();
	string filter = tv->getFilter();
	tv->setFilter("");

	tv->clear();
	SceneNode* root = GetEngine->GetSceneManager()->getRootSceneNode();
	MainWindow::Get()->ProcessSceneNode(root, false, true, false);

	tv->setItemsExpandStates(states);
	tv->setFilter(filter);
}

void HierarchyWindow::onNodesSelected(std::vector<string> names, void* userData)
{
	MainWindow::GetPreviewWindow()->clearPreview();

	std::vector<SceneNode*> nodes;

	for (auto it = names.begin(); it != names.end(); ++it)
	{
		SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(*it);
		nodes.push_back(node);
	}

	if (userData == nullptr)
	{
		if (names.size() > 0)
		{
			bool selectAfter = false;
			//MainWindow::gizmo->selectObjects(nodes, &selectAfter);
			MainWindow::gizmo2->selectObjects(nodes, &selectAfter);
		}
		else
		{
			bool selectAfter = false;
			//MainWindow::gizmo->clearSelection();
			//MainWindow::gizmo->selectObjects({}, &selectAfter);
			MainWindow::gizmo2->clearSelection();
			MainWindow::gizmo2->selectObjects({}, &selectAfter);
		}
	}

	MainWindow::GetAssetsWindow()->getTreeView()->selectNodes({  }, false);
}

void HierarchyWindow::onNodeBeginDrag(TreeNode* node)
{
	
}

void HierarchyWindow::onDropNode(TreeNode* node, TreeNode* from)
{
	if (from->treeView == treeView)
	{
		auto items = treeView->getLastSelectedNodes();

		/*if (items.size() == 0)
			items = { from->name };*/

		auto it = find(items.begin(), items.end(), from->name);
		if (it == items.end())
			items = { from->name };

		onNodesMoved(items, node);
	}
}

void HierarchyWindow::onReorder(TreeNode* node, int newIndex)
{
	SceneManager* manager = GetEngine->GetSceneManager();
	SceneNode* reorderNode = manager->getSceneNodeFast(node->name);

	if (reorderNode != nullptr)
	{
		SceneNode* parent = reorderNode->getParentSceneNode();

		std::map<Node*, std::pair<Node*, int>> mem;

		//Save editor nodes and remove them from hierarchy
		auto children = parent->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it)
		{
			if (GetEngine->IsEditorObject((SceneNode*)*it))
			{
				mem[*it] = make_pair((*it)->getParent(), (*it)->index);
				(*it)->getParent()->removeChild(*it);
			}
		}

		parent->removeChild(reorderNode);
		parent->insertChild(reorderNode, newIndex);

		//Restore editor nodes
		for (auto it = mem.begin(); it != mem.end(); ++it)
		{
			it->second.first->insertChild(it->first, it->second.second);
		}

		mem.clear();
	}

	MainWindow::AddOnEndUpdateCallback([=]() {
		GetEngine->UpdateSceneNodeIndexes();
	});
}

std::vector<bool> HierarchyWindow::onNodesMoved(std::vector<std::string> items, TreeNode* moveto)
{
	SceneManager* manager = GetEngine->GetSceneManager();
	SceneNode* nodeto = nullptr;

	if (!manager->hasSceneNode(moveto->name))
		nodeto = manager->getRootSceneNode();
	else
		nodeto = manager->getSceneNode(moveto->name);

	for (auto it = items.begin(); it != items.end(); ++it)
	{
		SceneNode* node = manager->getSceneNode(*it);

		if (!isNodeChildOf(nodeto, node) && nodeto != node->getParent() && nodeto != node)
		{
			Vector3 pos = node->_getDerivedPosition();
			Vector3 scl = node->getScale();
			Quaternion rot = node->_getDerivedOrientation();

			SceneNode* parent = node->getParentSceneNode();

			scl *= parent->_getDerivedScale();
			scl /= nodeto->_getDerivedScale();

			parent->removeChild(*it);
			nodeto->addChild(node);

			node->_setDerivedPosition(pos);
			node->setScale(scl);
			node->_setDerivedOrientation(rot);
		}
	}

	MainWindow::AddOnEndUpdateCallback([=]() {
		GetEngine->UpdateSceneNodeIndexes();

		if (MainWindow::gizmo2->getSelectedObjects().size() > 0)
			MainWindow::gizmo2->callSelectCallback();
	});

	return { true };
}

bool HierarchyWindow::onNodeRenamed(TreeNode* item, std::string oldName)
{
	if (item->alias.empty())
		return false;

	SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(item->name);
	node->setAlias(CP_SYS(item->alias));

	MainWindow::gizmo2->callSelectCallback();

	return true;
}

void HierarchyWindow::onNodePopupMenu(TreeNode* node, int val)
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
		auto nodes = treeView->getSelectedNodes();

		if (nodes.size() == 0)
			nodes = { node->name };

		MainWindow::AddOnEndUpdateCallback([=]() {
			for (auto it = nodes.begin(); it != nodes.end(); ++it)
			{
				TreeNode* _node = treeView->getNodeByName(*it, treeView->getRootNode());
				treeView->deleteNode(_node);

				if (GetEngine->GetSceneManager()->hasSceneNode(*it))
				{
					SceneNode* node = GetEngine->GetSceneManager()->getSceneNode(*it);
					Engine::Destroy(node);
				}
			}

			MainWindow::gizmo2->clearWireframes();
			MainWindow::gizmo2->clearSelection();
			MainWindow::gizmo2->callSelectCallback();
			GetEngine->UpdateSceneNodeIndexes();

			DeferredLightRenderOperation::UpdateStaticShadowmaps();
		});
		
		treeView->getSelectedNodes().clear();
	}
}

void HierarchyWindow::updateObjectMenu()
{
	ImGui::ImageButton((void*)addIcon->getHandle(), ImVec2(16, 16));

	if (ImGui::BeginPopupContextItem("add_object_popup", 0))
	{
		MainWindow::CreateObjectMenu();

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (ImGui::InputText("##HierarchyFilter", &treeView->getFilter()))
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

bool HierarchyWindow::isNodeChildOf(SceneNode* node, SceneNode* parent)
{
	SceneNode* p = (SceneNode*)node->getParent();

	bool result = false;
	while (p != nullptr)
	{
		if (parent == p)
		{
			result = true;
			break;
		}
		else
			p = (SceneNode*)p->getParent();
	}

	return result;
}
