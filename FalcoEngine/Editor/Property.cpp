#include "stdafx.h"
#include "PropertyEditor.h"
#include "Property.h"
#include "TreeView.h"

#include "../imgui/imgui.h"

Property::Property(PropertyEditor* ed, string name) : TreeNode(ed->getTreeView())
{
	setClassName("Property");

	genName(name);
	editor = ed;
	this->expanded = true;

	flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;
	setEditable(false);
}

Property::~Property()
{
	
}

void Property::update(bool opened)
{
	__super::update(opened);
}
