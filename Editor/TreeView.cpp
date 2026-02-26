#include "stdafx.h"
#include "TreeView.h"
#include "MainWindow.h"
#include "InputHandler.h"
#include "PropVector3.h"
#include "TreeNode.h"
#include "PropEditorHost.h"
#include "PropertyEditor.h"

#include "../Engine/Engine.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../Engine/GUI/ImGUIWidgets.h"
#include "../Engine/IO.h"
#include "../Engine/StringConverter.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

TreeNode* TreeView::lastClickedNode = nullptr;

TreeView::TreeView()
{
	boost::uuids::random_generator uuid_gen;
	boost::uuids::uuid u = uuid_gen();

	string rootName = boost::uuids::to_string(u);

	rootNode = new TreeNode(this);
	rootNode->name = rootName;
}

TreeView::~TreeView()
{
	deleteNode(rootNode);
}

void TreeView::deleteNode(TreeNode* node)
{
	TreeNode* parent = node->parent;

	if (parent != nullptr)
	{
		auto it = std::find(parent->children.begin(), parent->children.end(), node);

		if (it != parent->children.end())
			parent->children.erase(it);
	}

	if (lastClickedNode == node)
		lastClickedNode = nullptr;

	if (shiftSelectNode == node)
		shiftSelectNode = nullptr;

	if (editNode == node)
		editNode = nullptr;

	if (reorderNode == node)
		reorderNode = nullptr;

	if (focusNode == node)
		focusNode = nullptr;

	if (move_to == node)
		move_to = nullptr;

	if (move_from == node)
		move_from = nullptr;

	delete node;

	if (node != rootNode)
		setFilter(getFilter());
}

void TreeView::update()
{
	bool wasClicked = false;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 2));
	for (auto it = rootNode->children.begin(); it != rootNode->children.end(); ++it)
		updateRecursive(*it, wasClicked);
	ImGui::PopStyleVar();

	if (editNode != nullptr)
	{
		if (clickTime > 1.0f && clickTime < 3.0f)
		{
			editNode->setEditMode(true);
			editNode = nullptr;

			clickTime = 0.0f;
		}
		else
		{
			editNode = nullptr;
			clickTime = 0.0f;
		}
	}
	else
	{
		if (clickTime < 4.0f)
			clickTime += 1.0f * GetEngine->GetDeltaTime();
	}

	if (!processDragDropInTarget)
	{
		processDragDrop();
		updateSelection(wasClicked);
		isWindowDragging = false;

		if (endUpdateCallback != nullptr)
			endUpdateCallback();
	}
}

void TreeView::clear()
{
	lastClickedNode = nullptr;
	shiftSelectNode = nullptr;
	editNode = nullptr;
	reorderNode = nullptr;
	focusNode = nullptr;
	move_to = nullptr;
	move_from = nullptr;

	for (auto it = rootNode->children.begin(); it != rootNode->children.end(); ++it)
		delete *it;

	rootNode->children.clear();
}

void TreeView::updateSelection(bool wasClicked)
{
	if (ImGui::IsMouseReleased(0))
	{
		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_ChildWindows) && !ImGui::IsAnyItemHovered())
		{
			if (!isWindowDragging)
			{
				if (!wasClicked)
				{
					bool edit = false;

					for (auto it = selectedNodes.begin(); it != selectedNodes.end(); ++it)
					{
						TreeNode* n = getNodeByName(*it, rootNode);
						if (n != nullptr)
						{
							if (n->editMode)
							{
								edit = true;
								break;
							}
						}
					}

					if (!edit)
					{
						if (!InputHandler::IsCtrlPressed() && !InputHandler::IsShiftPressed())
						{
							selectedNodes.clear();
							selectNodes({ });
						}
					}
				}
			}
		}
	}
}

void TreeView::updateDragDropTarget()
{
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		if (ImGui::BeginDragDropTarget())
		{
			isWindowDragging = true;

			const ImGuiPayload* pl = ImGui::GetDragDropPayload();
			if (pl != nullptr)
			{
				if (move_from == nullptr && move_to == nullptr)
				{
					move_to = rootNode;
					move_from = (TreeNode*)pl->Data;

					if (move_to->treeView != move_from->treeView)
					{
						move_to = nullptr;
						move_from = nullptr;
					}
				}
			}

			ImGui::EndDragDropTarget();
		}
	}
	
	if (processDragDropInTarget)
	{
		processDragDrop();
		updateSelection(false);
		isWindowDragging = false;

		if (endUpdateCallback != nullptr)
			endUpdateCallback();
	}
}

std::map<std::string, bool> TreeView::getItemsExpandStates()
{
	std::map<std::string, bool> _map;

	getItemsExpandStatesRecursive(rootNode, _map);

	return _map;
}

void TreeView::setItemsExpandStates(std::map<std::string, bool> states)
{
	for (auto it = states.begin(); it != states.end(); ++it)
	{
		TreeNode* node = getNodeByName(it->first, rootNode);

		if (node != nullptr)
			node->expanded = it->second;
	}
}

void TreeView::getItemsExpandStatesRecursive(TreeNode* root, std::map<std::string, bool>& outMap)
{
	for (auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		outMap[(*it)->name] = (*it)->expanded;

		getItemsExpandStatesRecursive(*it, outMap);
	}
}

void getPlainNodeList(TreeNode* root, std::vector<TreeNode*>& outList)
{
	for (auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		outList.push_back(*it);
		if ((*it)->expanded)
			getPlainNodeList(*it, outList);
	}
}

std::vector<string> getNodesRange(TreeNode* root, TreeNode* start, TreeNode* end, string filter, std::vector<TreeNode*> filteredNodes)
{
	root->treeView->expandParent(start);
	root->treeView->expandParent(end);

	std::vector<TreeNode*> plainList;
	std::vector<string> selList;

	getPlainNodeList(root, plainList);

	auto s = find(plainList.begin(), plainList.end(), start);
	auto e = find(plainList.begin(), plainList.end(), end);

	if (distance(plainList.begin(), s) > distance(plainList.begin(), e))
	{
		auto m = s;
		s = e;
		e = m;
		selList.push_back(start->name);
	}
	else
	{
		selList.push_back(end->name);
	}

	for (auto it = s; it != e; ++it)
	{
		TreeNode* cur = *it;

		bool nodeFiltered = true;

		if (!filter.empty())
		{
			if (find(filteredNodes.begin(), filteredNodes.end(), cur) == filteredNodes.end())
				nodeFiltered = false;
		}

		if (nodeFiltered)
			selList.push_back(cur->name);
	}

	plainList.clear();

	return selList;
}

void filterNodesRecursive(TreeNode* rootMain, TreeNode* root, std::string filter, std::vector<TreeNode*> & outList)
{
	for each (TreeNode * node in root->children)
	{
		string cmp1 = boost::algorithm::to_lower_copy(node->alias);
		string cmp2 = boost::algorithm::to_lower_copy(filter);

		if (cmp1.find(cmp2) != string::npos)
		{
			TreeNode* par = node->parent;
			while (par != nullptr && par != rootMain)
			{
				if (find(outList.begin(), outList.end(), par) == outList.end())
					outList.push_back(par);

				par = par->parent;
			}

			outList.push_back(node);
		}

		filterNodesRecursive(rootMain, node, filter, outList);
	}
}

void TreeView::setFilter(std::string value)
{
	filter = value;
	filteredNodes.clear();
	filterNodesRecursive(rootNode, rootNode, value, filteredNodes);
}

void TreeView::updateRecursive(TreeNode* root, bool& wasClicked)
{
	if (root == nullptr)
		return;

	std::vector<TreeNode*> nstack;
	nstack.push_back(root);

	while (nstack.size() > 0)
	{
		TreeNode* curNode = *nstack.begin();
		nstack.erase(nstack.begin());

		if (curNode == nullptr)
		{
			ImGui::TreePop();
			continue;
		}

		bool nodeFiltered = true;

		if (!filter.empty())
		{
			if (find(filteredNodes.begin(), filteredNodes.end(), curNode) == filteredNodes.end())
				nodeFiltered = false;
		}

		if (!nodeFiltered)
		{
			int j = 0;
			for (auto it = curNode->children.begin(); it != curNode->children.end(); ++it, ++j)
			{
				TreeNode* nd = *it;
				nstack.insert(nstack.begin() + j, nd);
			}

			continue;
		}

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (curNode->children.size() == 0)
		{
			if (!curNode->alwaysShowArrow)
				flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (!useColumns)
		{
			if (curNode->fullSpaceWidth)
				flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		}
		else
		{
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		}

		flags |= curNode->flags;

		if (std::find(selectedNodes.begin(), selectedNodes.end(), curNode->name) != selectedNodes.end())
		{
			if (!curNode->editMode)
				flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (separatorOnTopNode == curNode)
		{
			ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1, 1, 0, 1));
			ImGui::Separator();
			ImGui::PopStyleColor();

			separatorOnTopNode = nullptr;
		}

		if (useColumns)
		{
			if (!curNode->fullSpaceWidth)
			{
				ImGui::BeginColumns(string(curNode->name + "_columns").c_str(), 2, ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder);
			}
		}

		if (curNode->showCheckBox)
		{
			bool clicked = ImGui::Checkbox(("##" + curNode->name + "_checkbox").c_str(), &curNode->checked);

			if (checkChildren)
			{
				if (clicked)
				{
					if (curNode->checked)
						checkNodeParent(curNode, curNode->checked);

					checkNodeChildren(curNode, curNode->checked);
				}
			}

			ImGui::SameLine();
		}

		if (curNode->icon != nullptr)
		{
			ImGui::Image((void*)curNode->icon->getHandle(), ImVec2(16.0f, 16.0f));
			ImGui::SameLine();
		}

		if (curNode->expanded)
			ImGui::SetNextItemOpen(true);
		else
			ImGui::SetNextItemOpen(false);

		bool nodeOpen = false;

		string _nalias = curNode->alias;
		if (curNode->editMode)
			_nalias = "";

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(curNode->color[0], curNode->color[1], curNode->color[2], curNode->color[3]));
		if (!curNode->fullSpaceWidth)
		{
			nodeOpen = ImGui::TreeNodeEx2(curNode->name.c_str(), flags, _nalias.c_str());
		}
		else
		{
			nodeOpen = ImGui::TreeNodeEx(curNode->name.c_str(), flags, _nalias.c_str());
		}
		ImGui::PopStyleColor();

		if (focusNode == curNode)
		{
			ImGui::SetScrollHereY();
			focusNode = nullptr;
		}

		ImGuiWindow* window = GImGui->CurrentWindow;
		ImRect dndRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		if (useColumns)
			dndRect.Max.x = window->Pos.x + window->Size.x - 8 - window->Scroll.x;

		ImGuiDragDropFlags src_flags = 0;
		src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;

		if (curNode->enableDrag)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover))
			{
				if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
				{
					string _tnodes = "";

					int k = 0;
					for (auto it2 = selectedNodes.begin(); it2 != selectedNodes.end(); ++it2, ++k)
					{
						if (k < 4)
						{
							string alias = getNodeByName(*it2, rootNode)->alias;
							_tnodes += alias;

							if (k < 3)
							{
								_tnodes += "\n";
							}
						}
					}

					if (k > 4)
						_tnodes += "\n...";

					ImGui::Text(_tnodes.c_str());
				}

				ImGui::SetDragDropPayload("DND_TreeView", curNode, sizeof(TreeNode));
				ImGui::EndDragDropSource();

				if (!dragStarted)
				{
					reorderNode = curNode;
					dragStarted = true;
					onDragStarted(curNode);
				}
			}

			/*-----REORDERING BEGIN-----*/
			if (allowReorder && dragStarted)
			{
				ImVec2 min = ImGui::GetItemRectMin();
				ImVec2 max = ImGui::GetItemRectMax();
				ImVec2 mouse = ImGui::GetMousePos();

				if (mouse.x > min.x && mouse.x < max.x)
				{
					//Top for root
					if (rootNode->children.size() > 0)
					{
						if (curNode == rootNode->children[0])
						{
							if (mouse.y < min.y && mouse.y > min.y - 10)
							{
								separatorOnTopNode = curNode;

								if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
								{
									tempDisableDragDrop = true;

									std::vector<string> nds = selectedNodes;

									MainWindow::AddOnEndUpdateCallback([=]()
									{
										auto _nds = *const_cast<std::vector<string>*>(&nds);// selectedNodes;

										if (reorderNode != nullptr)
										{
											if (_nds.size() == 0)
												_nds.push_back(reorderNode->name);
											else
											{
												if (find(_nds.begin(), _nds.end(), reorderNode->name) == _nds.end())
												{
													_nds.clear();
													_nds.push_back(reorderNode->name);
												}
											}
										}

										//int _idx = 0;
										TreeNode* _to = curNode;
										if (find(_nds.begin(), _nds.end(), _to->name) == _nds.end())
										{
											for (auto _it = _nds.begin(); _it != _nds.end(); ++_it)
											{
												TreeNode* _n1 = getNodeByName(*_it, rootNode);

												if (_n1->treeView != _to->treeView)
													continue;

												if (_n1->parent != _to->parent)
												{
													if (_to->parent->dropCallback != nullptr)
													{
														_to->parent->dropCallback(_to->parent, _n1);
													}

													if (dropCallback != nullptr)
													{
														dropCallback(_n1, _to->parent);
													}

													moveNodes(_to->parent, _n1, false);
												}

												TreeNode* _par = _to->parent;

												auto __1 = find(_par->children.begin(), _par->children.end(), _n1);
												auto __2 = find(_par->children.begin(), _par->children.end(), _to);

												_par->children.erase(__1);
												_par->children.insert(_par->children.begin()/* + _idx*/, _n1);

												if (reorderCallback != nullptr)
													reorderCallback(_n1, 0/* + _idx*/);

												//++_idx;
											}
										}

										tempDisableDragDrop = false;
										reorderNode = nullptr;

										selectNodes(nds);
									});
								}
							}
						}
					}

					//Bottom for others
					if (mouse.y > max.y - 1 && mouse.y < max.y + 4)
					{
						ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1, 1, 0, 1));
						ImGui::Separator();
						ImGui::PopStyleColor();

						if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							tempDisableDragDrop = true;

							std::vector<string> nds = selectedNodes;

							MainWindow::AddOnEndUpdateCallback([=]()
							{
								auto _nds = *const_cast<std::vector<string>*>(&nds);// selectedNodes;

								if (reorderNode != nullptr)
								{
									if (_nds.size() == 0)
										_nds.push_back(reorderNode->name);
									else
									{
										if (find(_nds.begin(), _nds.end(), reorderNode->name) == _nds.end())
										{
											_nds.clear();
											_nds.push_back(reorderNode->name);
										}
									}
								}

								//int _idx = 0;
								TreeNode* _to = curNode;
								if (find(_nds.begin(), _nds.end(), _to->name) == _nds.end())
								{
									bool move_to_begin = false;

									if (_to->children.size() > 0 && _to->expanded)
									{
										_to = _to->children[0];
										move_to_begin = true;
									}

									for (auto _it = _nds.begin(); _it != _nds.end(); ++_it)
									{
										TreeNode* _n1 = getNodeByName(*_it, rootNode);

										if (_n1->treeView != _to->treeView)
											continue;

										if (_n1 == _to)
											continue;

										bool _mtChild = false;
										TreeNode* tmp = _to->parent;
										while (tmp != nullptr)
										{
											if (_n1 == tmp)
											{
												_mtChild = true;
												tmp = nullptr;
												break;
											}

											tmp = tmp->parent;
										}

										if (_mtChild)
											continue;

										if (_n1->parent != _to->parent)
										{
											if (_to->parent->dropCallback != nullptr)
											{
												_to->parent->dropCallback(_to->parent, _n1);
											}

											if (dropCallback != nullptr)
											{
												dropCallback(_n1, _to->parent);
											}

											moveNodes(_to->parent, _n1, false);
										}

										TreeNode* _par = _to->parent;

										auto __1 = find(_par->children.begin(), _par->children.end(), _n1);
										auto __2 = find(_par->children.begin(), _par->children.end(), _to);

										_par->children.erase(__1);

										if (__1 > __2)
										{
											if (move_to_begin)
											{
												_par->children.insert(_par->children.begin()/* + _idx*/, _n1);

												if (reorderCallback != nullptr)
													reorderCallback(_n1, 0/* + _idx*/);
											}
											else if (*__2 == _par->children[0])
											{
												_par->children.insert(_par->children.begin() + 1/* + _idx*/, _n1);

												if (reorderCallback != nullptr)
													reorderCallback(_n1, 1 /*+ _idx*/);
											}
											else
											{
												_par->children.insert(__2/* + _idx*/ + 1, _n1);

												if (reorderCallback != nullptr)
												{
													int _i = distance(_par->children.begin(), __2);
													reorderCallback(_n1, _i/* + _idx*/ + 1);
												}
											}
										}
										else
										{
											_par->children.insert(__2/* + _idx*/, _n1);

											if (reorderCallback != nullptr)
											{
												int _i = distance(_par->children.begin(), __2);
												reorderCallback(_n1, _i/* + _idx*/);
											}
										}

										//++_idx;
									}
								}

								tempDisableDragDrop = false;
								reorderNode = nullptr;

								selectNodes(nds);
							});
						}
					}
				}
			}
			/*-----REORDERING END-----*/
		}

		if (useColumns)
		{
			if (!curNode->fullSpaceWidth)
			{
				ImGui::NextColumn();
			}
		}

		if (ImGui::IsItemToggledOpen())
		{
			curNode->expanded = !curNode->expanded;
		}

		curNode->update(nodeOpen);

		if (useColumns)
		{
			if (!curNode->fullSpaceWidth)
			{
				ImGui::EndColumns();
			}
		}

		if (useColumns)
		{
			if (!curNode->fullSpaceWidth)
			{
				dndRect.Max.y = ImGui::GetItemRectMax().y;
				ImVec2 cp = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(dndRect.Min.x - window->Pos.x + window->Scroll.x, dndRect.Min.y - window->Pos.y + window->Scroll.y));
				ImGui::InvisibleButton2(curNode->guid1.c_str(), dndRect.GetSize());
				ImGui::SetCursorPos(cp);
			}
		}

		if (curNode->supportedFormats.size() > 0)
		{
			if (ImGui::BeginDragDropTarget())
			{
				isWindowDragging = true;

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
				{
					if (move_from == nullptr && move_to == nullptr)
					{
						move_from = (TreeNode*)payload->Data;
						move_to = curNode;
					}
				}

				ImGui::EndDragDropTarget();
			}
		}

		ImGuiContext& g = *GImGui;
		bool checkArrow = false;
		if (curNode->children.size() > 0)
		{
			if (g.ActiveIdClickOffset.x > g.FontSize + g.Style.FramePadding.x * 2)
				checkArrow = true;
		}
		else
		{
			checkArrow = true;
		}

		bool dblClick = false;

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !isWindowDragging)
		{
			wasClicked = true;
		}

		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !isWindowDragging && checkArrow)
		{
			dblClick = true;
			clickTime = 4.0f;

			MainWindow::AddOnEndUpdateCallback([=]() {
				if (curNode->doubleClickCallback != nullptr)
					curNode->doubleClickCallback(curNode);
				});
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !isWindowDragging && checkArrow)
		{
			lastClickedNode = curNode;
			bool _mulSel = false;

			if (allowMultipleSelection)
			{
				if (InputHandler::IsCtrlPressed())
				{
					_mulSel = true;

					auto cur = find(selectedNodes.begin(), selectedNodes.end(), curNode->name);

					if (cur == selectedNodes.end())
						selectedNodes.push_back(curNode->name);
					else
						selectedNodes.erase(cur);

					std::sort(selectedNodes.begin(), selectedNodes.end(), [=](string& a, string& b) -> bool {
						if (tag == "Hierarchy")
						{
							SceneNode* _a = GetEngine->GetSceneManager()->getSceneNodeFast(a);
							SceneNode* _b = GetEngine->GetSceneManager()->getSceneNodeFast(b);

							return _a->index > _b->index;
						}

						if (tag == "Assets")
						{
							return a > b;
						}

						return false;
						});

					MainWindow::AddOnEndUpdateCallback([=]() { selectNodes(selectedNodes); });
				}
				else if (InputHandler::IsShiftPressed())
				{
					_mulSel = true;

					if (shiftSelectNode != nullptr)
					{
						selectedNodes.clear();

						auto sel = getNodesRange(rootNode, shiftSelectNode, curNode, filter, filteredNodes);
						selectedNodes = sel;
						sel.clear();

						MainWindow::AddOnEndUpdateCallback([=]() { selectNodes(selectedNodes); });
					}
				}
			}

			if (!_mulSel)
			{
				if (selectedNodes.size() == 1)
				{
					if (!dblClick)
					{
						bool selected = find(selectedNodes.begin(), selectedNodes.end(), curNode->name) != selectedNodes.end();

						if (selected)
						{
							editNode = curNode;
						}
					}
				}

				if (curNode != nullptr)
				{
					selectedNodes.clear();
					selectedNodes = { curNode->name };

					shiftSelectNode = curNode;

					MainWindow::AddOnEndUpdateCallback([=]() { selectNodes(selectedNodes); });
				}
			}
		}

		if (nodeOpen)
		{
			int j = 0;
			for (auto it = curNode->children.begin(); it != curNode->children.end(); ++it, ++j)
			{
				TreeNode* nd = *it;
				nstack.insert(nstack.begin() + j, nd);
			}

			nstack.insert(nstack.begin() + j, nullptr);
		}
	}
}

void TreeView::selectNodes(std::vector<string> names, bool callback, void* userData)
{
	selectedNodes = names;

	for (auto it = names.begin(); it != names.end(); ++it)
	{
		TreeNode* node = getNodeByName(*it, rootNode);
		if (node != nullptr)
			expandParent(node);
	}

	if (callback)
	{
		if (selectCallback != nullptr)
			selectCallback(names, userData);
	}
}

void TreeView::setSelectCallback(void(*callback)(std::vector<string>names, void* userData))
{
	selectCallback = callback;
}

void TreeView::expandParent(TreeNode* node)
{
	if (node->parent != rootNode)
	{
		node->parent->expanded = true;

		expandParent(node->parent);
	}
}

TreeNode* TreeView::getNodeByName(string name, TreeNode* root, bool recursive)
{
	TreeNode* node = nullptr;
	getNodeByCondRecursive(name, root, node, SearchCond::SC_NAME, recursive);

	return node;
}

TreeNode* TreeView::getNodeByAlias(string alias, TreeNode* root, bool recursive)
{
	TreeNode* node = nullptr;
	getNodeByCondRecursive(alias, root, node, SearchCond::SC_ALIAS, recursive);

	return node;
}

TreeNode* TreeView::getNodeByPath(string path, TreeNode* root)
{
	std::vector<std::string> results;
	boost::split(results, path, [](char c) { return c == '/' || c == '\\'; });

	TreeNode * item = root;
	for (auto it = results.begin(); it != results.end(); ++it)
	{
		bool found = false;
		auto children = item->children;

		if (item->getClassName() == "PropEditorHost")
		{
			PropertyEditor* hosted = ((PropEditorHost*)item)->getHostedEditor();
			if (hosted != nullptr)
			{
				TreeView* hostedTree = hosted->getTreeView();
				children = hostedTree->getRootNode()->children;
			}
		}

		for (auto it2 = children.begin(); it2 != children.end(); ++it2)
		{
			if ((*it2)->alias == *it)
			{
				item = *it2;
				found = true;
				break;
			}
		}

		if (!found)
		{
			item = root;
			break;
		}
	}

	if (item == root)
		item = nullptr;

	return item;
}

string TreeView::getNodePath(TreeNode* node)
{
	string path = "";

	TreeNode* parent = node->parent;

	if (parent == node->treeView->getRootNode())
	{
		if (node->getClassName() == "Property" || node->getClassName() == "PropEditorHost")
		{
			TreeNode* parentEditorNode = ((Property*)node)->getEditor()->getParentEditorNode();
			if (parentEditorNode != nullptr)
				parent = parentEditorNode;
		}
	}

	//if (IO::GetFileExtension(CP_SYS(node->alias)).empty())
	//	path = "/";

	while (parent != nullptr)
	{
		path = node->alias + path;
		node = parent;
		parent = node->parent;

		if (parent == node->treeView->getRootNode())
		{
			if (node->getClassName() == "Property" || node->getClassName() == "PropEditorHost")
			{
				TreeNode* parentEditorNode = ((Property*)node)->getEditor()->getParentEditorNode();
				if (parentEditorNode != nullptr)
					parent = parentEditorNode;
			}
		}

		if (parent != nullptr)
			path = "/" + path;
	}

	return path;
}

void TreeView::getNodeByCondRecursive(string search, TreeNode* root, TreeNode*& outNode, SearchCond cond, bool recursive)
{
	if (recursive)
	{
		if (outNode != nullptr)
		{
			if (cond == SearchCond::SC_NAME)
			{
				if (outNode->name == search)
					return;
			}
			else
			{
				if (outNode->alias == search)
					return;
			}
		}

		if (cond == SearchCond::SC_NAME)
		{
			if (root->name == search)
			{
				outNode = root;
			}
			else
			{
				for (auto it = root->children.begin(); it != root->children.end(); ++it)
				{
					getNodeByCondRecursive(search, *it, outNode, cond, recursive);
				}
			}
		}
		else
		{
			if (root->alias == search)
			{
				outNode = root;
			}
			else
			{
				for (auto it = root->children.begin(); it != root->children.end(); ++it)
				{
					getNodeByCondRecursive(search, *it, outNode, cond, recursive);
				}
			}
		}
	}
	else
	{
		if (cond == SearchCond::SC_NAME)
		{
			for (auto it = root->children.begin(); it != root->children.end(); ++it)
			{
				if ((*it)->name == search)
				{
					outNode = *it;
					break;
				}
			}
		}
		else
		{
			for (auto it = root->children.begin(); it != root->children.end(); ++it)
			{
				if ((*it)->alias == search)
				{
					outNode = *it;
					break;
				}
			}
		}
	}
}

void TreeView::checkNodeChildren(TreeNode* root, bool check)
{
	for (auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		TreeNode* node = *it;
		node->checked = check;

		checkNodeChildren(node, check);
	}
}

void TreeView::checkNodeParent(TreeNode* root, bool check)
{
	if (root->parent != nullptr)
	{
		if (root->parent != rootNode)
		{
			root->parent->checked = check;
			checkNodeParent(root->parent, check);
		}
	}
}

void TreeView::moveNodes(TreeNode* moveto, TreeNode* movefrom, bool useSelected)
{
	if (movefrom->treeView == this)
	{
		auto sel = selectedNodes;
		
		if (useSelected)
		{
			for (auto it = sel.begin(); it != sel.end(); ++it)
			{
				TreeNode* fn = getNodeByName(*it, rootNode);

				if (!moveto->isChildOf(fn) && moveto != fn->parent && moveto != fn)
				{
					fn->parent->removeChild(fn);
					moveto->addChild(fn);
				}
			}
		}
		else
		{
			if (!moveto->isChildOf(movefrom) && moveto != movefrom->parent && moveto != movefrom)
			{
				movefrom->parent->removeChild(movefrom);
				moveto->addChild(movefrom);
			}
		}
	}

	setFilter(getFilter());
}

void TreeView::onDragStarted(TreeNode* node)
{
	if (beginDragCallback != nullptr)
	{
		beginDragCallback(node);
	}

	/*if (selectedNodes.size() <= 1)
	{
		resetLastNodes = true;
		lastSelectedNodes.clear();
		lastSelectedNodes = selectedNodes;
		selectedNodes.clear();
		selectedNodes.push_back(node->name);
	}
	else
	{*/
	auto it = find(selectedNodes.begin(), selectedNodes.end(), node->name);

	if (it == selectedNodes.end())
	{
		resetLastNodes = true;
		lastSelectedNodes.clear();
		lastSelectedNodes = selectedNodes;
		selectedNodes.clear();
		selectedNodes.push_back(node->name);
	}
	else
	{
		resetLastNodes = true;
		lastSelectedNodes.clear();
		lastSelectedNodes = selectedNodes;
	}
	//}

	std::sort(selectedNodes.begin(), selectedNodes.end(), [=](string& a, string& b) -> bool {
		if (tag == "Hierarchy")
		{
			SceneNode* _a = GetEngine->GetSceneManager()->getSceneNodeFast(a);
			SceneNode* _b = GetEngine->GetSceneManager()->getSceneNodeFast(b);

			return _a->index > _b->index;
		}

		if (tag == "Assets")
		{
			return a > b;
		}

		return false;
	});
}

void TreeView::onDragEnded()
{
	dragStarted = false;

	if (resetLastNodes)
	{
		resetLastNodes = false;

		selectedNodes.clear();
		selectedNodes = lastSelectedNodes;
		lastSelectedNodes.clear();
	}
}

void TreeView::processDragDrop()
{
	if (move_from != nullptr && move_to != nullptr)
	{
		if (!tempDisableDragDrop)
		{
			if (move_to->isFormatSupported(move_from->format, move_from->treeView))
			{
				if (move_to->dropCallback != nullptr)
				{
					move_to->dropCallback(move_to, move_from);
				}

				if (dropCallback != nullptr)
				{
					dropCallback(move_from, move_to);
				}
			}

			if (allowMoveNodes)
			{
				moveNodes(move_to, move_from);
			}
		}

		move_from = nullptr;
		move_to = nullptr;
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		if (dragStarted)
			onDragEnded();
	}
}