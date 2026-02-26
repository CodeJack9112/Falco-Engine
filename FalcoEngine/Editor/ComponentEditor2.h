#pragma once

#include <OgrePrerequisites.h>
#include "PropertyEditor.h"

using namespace Ogre;
using namespace std;

class ComponentEditor2 : public PropertyEditor
{
public:
	ComponentEditor2();
	~ComponentEditor2() {}

	virtual void init() {}
	virtual void init(std::vector<SceneNode*> nodes) { sceneNodes = nodes; }
	virtual void updateEditor();

	std::vector<SceneNode*> & getSceneNodes() { return sceneNodes; }

private:
	bool updateState = false;

	void onTreeViewEndUpdate();

	std::vector<SceneNode*> sceneNodes;
};

