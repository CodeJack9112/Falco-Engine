#include "stdafx.h"
#include "AnimationListEditor2.h"

#include <OgreSceneNode.h>

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/AnimationList.h"
#include "../Engine/ResourceMap.cpp"
#include "../Engine/FBXSceneManager.h"

#include "PropString.h"
#include "PropComboBox.h"
#include "PropBool.h"
#include "PropInt.h"
#include "PropFloat.h"

AnimationListEditor2::AnimationListEditor2()
{
}

AnimationListEditor2::~AnimationListEditor2()
{
}

void AnimationListEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(AnimationList::COMPONENT_TYPE);
	AnimationList* comp = (AnimationList*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	addProperty(enabled);

	if (isAnimationsIdentical())
	{
		AnimationList::AnimationData defAnimData = comp->GetDefaultAnimationData();

		std::vector<string> anims;
		for (auto it = comp->GetAnimationDataList().begin(); it != comp->GetAnimationDataList().end(); ++it)
			anims.push_back(it->name);

		PropComboBox* defaultAnimation = new PropComboBox(this, "Default animation", anims);
		defaultAnimation->setCurrentItem(defAnimData.name);
		defaultAnimation->setOnChangeCallback([=](Property* prop, int val) { onChangeDefaultAnimation(prop, val); });

		PropBool* playAtStart = new PropBool(this, "Play at start", comp->GetPlayAtStart());
		playAtStart->setOnChangeCallback([=](Property* prop, bool val) { onChangePlayAtStart(prop, val); });

		Property* animations = new Property(this, "Animations");
		animations->setHasButton(true);
		animations->setButtonText("+");
		animations->setOnButtonClickCallback([=](TreeNode* prop) { onAddAnimation(prop); });

		int i = 0;
		for (auto it = comp->GetAnimationDataList().begin(); it != comp->GetAnimationDataList().end(); ++it, ++i)
		{
			Property* animGroup = new Property(this, to_string(i));

			string _filename = ResourceMap::getResourceNameFromGuid(it->fileGuid);
			if (_filename.empty())
				_filename = it->fileName;

			PropString* fileName = new PropString(this, "File", _filename);
			fileName->setReadonly(true);
			fileName->setSupportedFormats({ "fbx" });
			fileName->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropFbx(prop, from); });

			PropString* animName = new PropString(this, "Name", it->name);
			PropInt* startFrame = new PropInt(this, "Start frame", it->startFrame);
			PropInt* endFrame = new PropInt(this, "End frame", it->endFrame);
			PropFloat* speed = new PropFloat(this, "Speed", it->speed);
			PropBool* loop = new PropBool(this, "Loop", it->loop);

			animGroup->setUserData(static_cast<void*>(new int(i)));
			animGroup->setPopupMenu({ "Remove" }, [=](TreeNode* prop, int val) { onPopupSelectedAnimation(prop, val); });

			animGroup->addChild(fileName);
			animGroup->addChild(animName);
			animGroup->addChild(startFrame);
			animGroup->addChild(endFrame);
			animGroup->addChild(speed);
			animGroup->addChild(loop);

			fileName->setSupportedFormats({ "fbx" });

			animName->setOnChangeCallback([=](Property* prop, string val) { onChangeName(prop, val); });
			startFrame->setOnChangeCallback([=](Property* prop, int val) { onChangeStartFrame(prop, val); });
			endFrame->setOnChangeCallback([=](Property* prop, int val) { onChangeEndFrame(prop, val); });
			speed->setOnChangeCallback([=](Property* prop, float val) { onChangeSpeed(prop, val); });
			loop->setOnChangeCallback([=](Property* prop, bool val) { onChangeLoop(prop, val); });

			animations->addChild(animGroup);
		}

		addProperty(defaultAnimation);
		addProperty(playAtStart);
		addProperty(animations);
	}
}

bool AnimationListEditor2::isAnimationsIdentical()
{
	bool result = true;

	auto sceneNodes = getSceneNodes();
	SceneNode* node = sceneNodes[0];

	AnimationList* animList = (AnimationList*)node->GetComponent(AnimationList::COMPONENT_TYPE);
	std::vector<AnimationList::AnimationData> animDataList = animList->GetAnimationDataList();

	for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if (!result)
			break;

		SceneNode* currNode = *it;
		AnimationList* currAnimList = (AnimationList*)currNode->GetComponent(AnimationList::COMPONENT_TYPE);
		std::vector<AnimationList::AnimationData> currAnimDataList = currAnimList->GetAnimationDataList();

		if (animList->GetAnimationDataList().size() != currAnimDataList.size())
		{
			result = false;
			break;
		}

		int i = 0;
		for (std::vector<AnimationList::AnimationData>::iterator _it = animDataList.begin(); _it != animDataList.end(); ++_it, ++i)
		{
			if (currAnimDataList.at(i).name != _it->name)
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

void AnimationListEditor2::onChangeEnabled(Property* prop, bool val)
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

void AnimationListEditor2::onChangeDefaultAnimation(Property* prop, int val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->SetDefaultAnimation(val);
	}
}

void AnimationListEditor2::onChangePlayAtStart(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->SetPlayAtStart(val);
	}
}

void AnimationListEditor2::onChangeName(Property* prop, string val)
{
	void* data = prop->parent->getUserData();
	void* data1 = prop->parent->parent->parent->getUserData();
	int animIndex = *static_cast<int*>(data);
	int index = *static_cast<int*>(data1);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->GetAnimationDataAt(animIndex).name = val;
	}
}

void AnimationListEditor2::onChangeStartFrame(Property* prop, int val)
{
	void* data = prop->parent->getUserData();
	void* data1 = prop->parent->parent->parent->getUserData();
	int animIndex = *static_cast<int*>(data);
	int index = *static_cast<int*>(data1);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->GetAnimationDataAt(animIndex).startFrame = val;
	}
}

void AnimationListEditor2::onChangeEndFrame(Property* prop, int val)
{
	void* data = prop->parent->getUserData();
	void* data1 = prop->parent->parent->parent->getUserData();
	int animIndex = *static_cast<int*>(data);
	int index = *static_cast<int*>(data1);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->GetAnimationDataAt(animIndex).endFrame = val;
	}
}

void AnimationListEditor2::onChangeSpeed(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	void* data1 = prop->parent->parent->parent->getUserData();
	int animIndex = *static_cast<int*>(data);
	int index = *static_cast<int*>(data1);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->GetAnimationDataAt(animIndex).speed = val;
	}
}

void AnimationListEditor2::onChangeLoop(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	void* data1 = prop->parent->parent->parent->getUserData();
	int animIndex = *static_cast<int*>(data);
	int index = *static_cast<int*>(data1);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		animList->GetAnimationDataAt(animIndex).loop = val;
	}
}

void AnimationListEditor2::onAddAnimation(TreeNode* prop)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = ((AnimationList*)component);

		animList->AddAnimationData("Animation_" + to_string(animList->GetAnimationDataList().size() + 1), animList->GetFBXFileName(), 0, 20);
	}

	updateEditor();
}

void AnimationListEditor2::onPopupSelectedAnimation(TreeNode* prop, int val)
{
	if (val == 0)
	{
		void* data = prop->getUserData();
		void* data1 = prop->parent->parent->getUserData();
		int animIndex = *static_cast<int*>(data);
		int index = *static_cast<int*>(data1);

		auto sceneNodes = getSceneNodes();

		for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
		{
			Component* component = (*it)->components.at(index);
			AnimationList* animList = (AnimationList*)component;

			animList->RemoveAnimationDataAt(animIndex);
		}
	}

	updateEditor();
}

void AnimationListEditor2::onDropFbx(TreeNode* prop, TreeNode* from)
{
	void* data = prop->parent->getUserData();
	void* data1 = prop->parent->parent->parent->getUserData();
	int animIndex = *static_cast<int*>(data);
	int index = *static_cast<int*>(data1);

	string fullPath = from->getPath();
	((PropString*)prop)->setValue(fullPath);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AnimationList* animList = (AnimationList*)component;

		FBXScene * fbx = FBXSceneManager::getSingleton().getByName(fullPath, "Assets").getPointer();

		animList->GetAnimationDataAt(animIndex).fileName = fullPath;
		animList->GetAnimationDataAt(animIndex).fileGuid = ResourceMap::guidMap[fbx];
	}
}
