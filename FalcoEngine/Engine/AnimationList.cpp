#include "stdafx.h"
#include "AnimationList.h"
#include <OgreSceneNode.h>
#include "FBXSceneManager.h"

#include "Engine.h"
#include <Engine\ResourceMap.h>

std::string AnimationList::COMPONENT_TYPE = "Animation";

AnimationList::AnimationList(SceneNode * parent) : Component(parent, GetEngine->GetMonoRuntime()->animation_class)
{
	SceneNode * node = parent;
}

AnimationList::~AnimationList()
{
	//mono_thread_attach(GetEngine->GetMonoRuntime()->GetDomain());

	for (AnimationList::AnimationDataList::iterator it = animationDataList.begin(); it != animationDataList.end(); ++it)
	{
		Engine::AnimationPlay(this, it->name, false);
	}

	animationDataList.clear();
	indexedPointers.clear();
}

void AnimationList::AddAnimationData(std::string name, std::string fileName, int start, int end, bool loop, float speed)
{
	AnimationList::AnimationData anim;
	anim.name = name;
	anim.fileName = fileName;
	anim.fileGuid = ResourceMap::guidMap[FBXSceneManager::getSingleton().getByName(fileName, "Assets").getPointer()];
	anim.startFrame = start;
	anim.endFrame = end;
	anim.loop = loop;
	anim.speed = speed;

	animationDataList.push_back(anim);

	if (defaultAnimation < 0)
		defaultAnimation = 0;
}

AnimationList::AnimationData & AnimationList::GetAnimationDataAt(int index)
{
	if (animationDataList.size() > index)
		return animationDataList.at(index);
	else
		return AnimationList::AnimationData();
}

AnimationList::AnimationData & AnimationList::GetAnimationData(std::string name)
{
	auto anim = std::find_if(animationDataList.begin(), animationDataList.end(), [name](const AnimationList::AnimationData & dt) { return dt.name == name; });
	if (anim != animationDataList.end())
		return *anim;
	else
		return AnimationList::AnimationData();
}

AnimationList::AnimationData & AnimationList::GetAnimationDataFullName(std::string name)
{
	auto anim = std::find_if(animationDataList.begin(), animationDataList.end(), [name, this](const AnimationList::AnimationData & dt) { return dt.fileName + "_" + GetParentSceneNode()->getName() + "_anim_" + dt.name == name; });

	if (anim != animationDataList.end())
		return *anim;
	else
		return AnimationList::AnimationData();
}

bool AnimationList::IsAnimationDataExists(std::string name)
{
	auto anim = std::find_if(animationDataList.begin(), animationDataList.end(), [name](const AnimationList::AnimationData & dt) { return dt.name == name; });

	return anim != animationDataList.end();
}

void AnimationList::RemoveAnimationDataAt(int index)
{
	animationDataList.erase(animationDataList.begin() + index);
	if (defaultAnimation >= index)
		defaultAnimation = 0;
}

void AnimationList::RemoveAnimationData(std::string name)
{
	auto anim = std::find_if(animationDataList.begin(), animationDataList.end(), [name](const AnimationList::AnimationData & dt) { return dt.name == name; });

	if (defaultAnimation >= distance(animationDataList.begin(), anim))
		defaultAnimation = 0;

	if (anim != animationDataList.end())
		animationDataList.erase(anim);
}

void AnimationList::SetFBXFileName(std::string name)
{
	fbxFileName = name;
	fbxFileGuid = ResourceMap::guidMap[FBXSceneManager::getSingleton().getByName(name, "Assets").getPointer()];
}

AnimationList::AnimationData & AnimationList::GetDefaultAnimationData()
{
	if (animationDataList.size() > defaultAnimation)
		return GetAnimationDataAt(defaultAnimation);
	else
		return AnimationList::AnimationData("[None]");
}

int AnimationList::GetAnimationDataIndex(std::string name)
{
	int index = 0;

	for (AnimationList::AnimationDataList::iterator it = animationDataList.begin(); it != animationDataList.end(); ++it)
	{
		if (it->name == name)
			return index;

		++index;
	}

	return -1;
}