#pragma once

#include "TreeNode.h"
#include <string>

class PropertyEditor;

using namespace std;

class Property : public TreeNode
{
public:
	Property(PropertyEditor* ed, string name);
	~Property();

	void update(bool opened) override;
	
	PropertyEditor* getEditor() { return editor; }

private:
	
	PropertyEditor* editor = nullptr;
};

