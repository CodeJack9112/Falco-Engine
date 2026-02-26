#include "stdafx.h"
#include "PropertyEditor.h"
#include "TreeView.h"
#include "TreeNode.h"
#include "Property.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

PropertyEditor::PropertyEditor()
{
	treeView = new TreeView();
	treeView->setAllowMultipleSelection(false);
	treeView->setUseColumns(true);
}

PropertyEditor::~PropertyEditor()
{
	delete treeView;
	properties.clear();
}

void recursiveSearch(TreeNode* root, bool & result)
{
	if (result)
		return;

	if (TreeView::getLastClickedNode() == root)
	{
		result = true;
		return;
	}

	for (auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		recursiveSearch(*it, result);
	}
}

void PropertyEditor::update()
{
	treeView->update();

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
			else
			{
				bool treeClicked = false;
				recursiveSearch(treeView->getRootNode(), treeClicked);

				if (!treeClicked)
					treeView->selectNodes({  });
			}
		}
	}

	if (ImGui::IsMouseReleased(0))
	{
		isWindowDragging = false;
	}
}

void PropertyEditor::addProperty(Property* prop)
{
	properties.push_back(prop);
	treeView->getRootNode()->addChild(prop);
}

void PropertyEditor::removeProperty(Property* prop)
{
	auto it = std::find(properties.begin(), properties.end(), prop);
	if (it != properties.end())
		properties.erase(it);

	treeView->deleteNode(prop);
}
