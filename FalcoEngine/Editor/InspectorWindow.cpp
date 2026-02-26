#include "stdafx.h"
#include "InspectorWindow.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "MainWindow.h"
#include "PropertyEditor.h"
#include "TreeNode.h"
#include "TreeView.h"
#include "PropEditorHost.h"
#include "ObjectEditor2.h"

#include "../Engine/EditorSettings.h"

InspectorWindow::InspectorWindow()
{
	
}

InspectorWindow::~InspectorWindow()
{
	
}

void InspectorWindow::update()
{
	bool rootOpened = true;

	if (opened)
	{
		if (ImGui::Begin("Inspector", &opened, ImGuiWindowFlags_NoCollapse))
		{
			if (setScrollPosValue > -1)
			{
				ImGui::SetScrollY(setScrollPosValue);
				setScrollPosValue = -1;
			}

			if (currentEditor != nullptr)
			{
				currentEditor->update();
			}
		}

		scrollPos = ImGui::GetScrollY();

		ImGui::End();
	}
}

void InspectorWindow::setEditor(PropertyEditor* editor)
{
	saveCollapsedProperties();

	if (currentEditor != nullptr)
		delete currentEditor;

	currentEditor = editor;

	loadCollapsedProperties();
}

void InspectorWindow::updateCurrentEditor()
{
	if (currentEditor != nullptr)
		currentEditor->updateEditor();
}

void InspectorWindow::updateObjectEditorTransform()
{
	if (currentEditor != nullptr)
	{
		if (dynamic_cast<ObjectEditor2*>(currentEditor))
		{
			ObjectEditor2* ed = (ObjectEditor2*)currentEditor;
			ed->updateTransform();
		}
	}
}

void InspectorWindow::saveCollapsedProperties()
{
	if (currentEditor != nullptr)
	{
		TreeView* tree = currentEditor->getTreeView();

		if (tree != nullptr)
		{
			EditorSettings settings;
			settings.Load(MainWindow::GetProjectSettingsDir() + "Editor.settings");

			auto editors = settings.propertyEditorsData;
			auto i = find_if(editors.begin(), editors.end(), [=](PropertyEditorData& ed) -> bool { return ed.editorName == currentEditor->getEditorName(); });

			PropertyEditorData dt;

			if (i != editors.end())
			{
				dt = *i;
				editors.erase(i);
			}

			dt.editorName = currentEditor->getEditorName();

			std::vector<TreeNode*> propList;
			getAllProperties(currentEditor->getTreeView()->getRootNode(), propList);

			for (auto it = propList.begin(); it != propList.end(); ++it)
			{
				TreeNode* node = *it;
				string path = node->getPath();

				PropertyState state;

				auto j = find_if(dt.properties.begin(), dt.properties.end(), [=](PropertyState& st) -> bool { return st.path == path; });

				if (j != dt.properties.end())
				{
					state = *j;
					dt.properties.erase(j);
				}

				state.path = path;
				state.expanded = node->expanded;

				dt.properties.push_back(state);
			}

			editors.push_back(dt);
			settings.propertyEditorsData = editors;

			settings.Save(MainWindow::GetProjectSettingsDir() + "Editor.settings");
		}
	}
}

void InspectorWindow::loadCollapsedProperties()
{
	if (currentEditor != nullptr)
	{
		TreeView* tree = currentEditor->getTreeView();

		if (tree != nullptr)
		{
			EditorSettings settings;
			settings.Load(MainWindow::GetProjectSettingsDir() + "Editor.settings");

			auto props = settings.propertyEditorsData;
			auto editorData = find_if(props.begin(), props.end(), [=](PropertyEditorData& ed) -> bool { return ed.editorName == currentEditor->getEditorName(); });

			if (editorData != props.end())
			{
				PropertyEditorData dt = *editorData;

				for (auto it = dt.properties.begin(); it != dt.properties.end(); ++it)
				{
					string path = it->path;

					TreeNode* node = currentEditor->getTreeView()->getNodeByPath(path, tree->getRootNode());
					if (node != nullptr)
						node->expanded = it->expanded;
				}
			}
		}
	}
}

void InspectorWindow::getAllProperties(TreeNode* root, std::vector<TreeNode*>& list)
{
	auto children = root->children;

	for (auto it = children.begin(); it != children.end(); ++it)
	{
		TreeNode* node = *it;

		list.push_back(node);

		if (dynamic_cast<PropEditorHost*>(node))
		{
			PropertyEditor* hosted = ((PropEditorHost*)node)->getHostedEditor();
			TreeView* hostedTree = hosted->getTreeView();

			getAllProperties(hostedTree->getRootNode(), list);
		}
		else
			getAllProperties(*it, list);
	}
}
