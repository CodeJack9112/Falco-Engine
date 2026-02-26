#include "stdafx.h"
#include "ComponentEditor2.h"

#include "TreeView.h"
#include "TreeNode.h"
#include "MainWindow.h"
#include "InspectorWindow.h"

ComponentEditor2::ComponentEditor2()
{
	getTreeView()->setOnEndUpdateCallback([=]() { onTreeViewEndUpdate(); });
}

void ComponentEditor2::updateEditor()
{
	updateState = true;
}

void ComponentEditor2::onTreeViewEndUpdate()
{
	if (updateState)
	{
		float sp = MainWindow::GetInspectorWindow()->getScrollPos();
		MainWindow::GetInspectorWindow()->saveCollapsedProperties();

		auto props = getTreeView()->getRootNode()->children;

		for (auto it = props.begin(); it != props.end(); ++it)
			removeProperty((Property*)*it);

		init(sceneNodes);

		MainWindow::GetInspectorWindow()->loadCollapsedProperties();
		MainWindow::GetInspectorWindow()->setScrollPos(sp);

		updateState = false;
	}
}
