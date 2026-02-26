#include "stdafx.h"
#include "SkeletonBoneEditor2.h"

#include <OgreSceneNode.h>
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/SkeletonBone.h"

#include "PropBool.h"

SkeletonBoneEditor2::SkeletonBoneEditor2()
{
}

SkeletonBoneEditor2::~SkeletonBoneEditor2()
{
}

void SkeletonBoneEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(SkeletonBone::COMPONENT_TYPE);
	SkeletonBone* comp = (SkeletonBone*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	addProperty(enabled);
}

void SkeletonBoneEditor2::onChangeEnabled(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		component->SetEnabled(val);
	}
}
