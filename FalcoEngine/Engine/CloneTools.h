#pragma once

#include "Engine.h"

class CloneTools
{
private:
	//static SceneNode * tempRoot;
	
	static std::map<std::string, std::string> remapList;
	static SceneNode * CloneSceneNodeRecursive(SceneNode * node, SceneNode * cloneTo = NULL);
	static void ReassignParamsRecursive(SceneNode * node);

public:
	CloneTools();
	~CloneTools();

	static SceneNode * CloneSceneNode(SceneNode * node, SceneNode * cloneTo = NULL);
	static void cloneScript(MonoScript* script, SceneNode* copyTo);
	static void cloneComponent(Component* component, SceneNode* copyTo);
};

