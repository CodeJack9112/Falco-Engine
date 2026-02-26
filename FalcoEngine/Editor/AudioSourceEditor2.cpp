#include "stdafx.h"
#include "AudioSourceEditor2.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/AudioSource.h"
#include "../Engine/IO.h"

#include "PropVector2.h"
#include "PropVector3.h"
#include "PropVector4.h"
#include "PropString.h"
#include "PropBool.h"
#include "PropInt.h"
#include "PropFloat.h"
#include "PropButton.h"
#include "../Engine/ResourceMap.h"

#include "MainWindow.h"
#include "AssetsWindow.h"

AudioSourceEditor2::AudioSourceEditor2()
{
}

AudioSourceEditor2::~AudioSourceEditor2()
{
}

void AudioSourceEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(AudioSource::COMPONENT_TYPE);
	AudioSource* comp = (AudioSource*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	string fn = ResourceMap::getResourceNameFromGuid(comp->GetFileGuid());
	if (fn.empty())
		fn = ((AudioSource*)component)->GetFileName();

	PropButton* fileName = new PropButton(this, "Audio clip", IO::GetFileNameWithExt(fn));
	fileName->setSupportedFormats({ "ogg", "wav" });
	fileName->setOnDropCallback([=](TreeNode* prop, TreeNode* from) { onDropSoundFile(prop, from); });
	fileName->setOnClickCallback([=](Property* prop) { onClickSoundFile(prop); });

	PropBool* playOnStart = new PropBool(this, "Play on start", comp->GetPlayOnStart());
	playOnStart->setOnChangeCallback([=](Property* prop, bool val) { onChangePlayOnStart(prop, val); });

	PropBool* loop = new PropBool(this, "Loop", comp->GetLooped());
	loop->setOnChangeCallback([=](Property* prop, bool val) { onChangeLoop(prop, val); });

	PropBool* is2D = new PropBool(this, "2D", comp->GetIs2D());
	is2D->setOnChangeCallback([=](Property* prop, bool val) { onChangeIs2D(prop, val); });

	PropFloat* volume = new PropFloat(this, "Volume", comp->GetVolume());
	volume->setOnChangeCallback([=](Property* prop, float val) { onChangeVolume(prop, val); });

	PropFloat* minDistance = new PropFloat(this, "Min distance", comp->GetMinDistance());
	minDistance->setOnChangeCallback([=](Property* prop, float val) { onChangeMinDistance(prop, val); });

	PropFloat* maxDistance = new PropFloat(this, "Max distance", comp->GetMaxDistance());
	maxDistance->setOnChangeCallback([=](Property* prop, float val) { onChangeMaxDistance(prop, val); });

	addProperty(enabled);
	addProperty(fileName);
	addProperty(playOnStart);
	addProperty(loop);
	addProperty(is2D);
	addProperty(volume);
	addProperty(minDistance);
	addProperty(maxDistance);
}

void AudioSourceEditor2::onChangeEnabled(Property* prop, bool val)
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

void AudioSourceEditor2::onChangePlayOnStart(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		comp->SetPlayOnStart(val);
	}
}

void AudioSourceEditor2::onChangeLoop(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		comp->SetLoop(val);
	}
}

void AudioSourceEditor2::onChangeVolume(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		comp->SetVolume(val);
	}
}

void AudioSourceEditor2::onChangeMinDistance(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		comp->SetMinDistance(val);
	}
}

void AudioSourceEditor2::onChangeMaxDistance(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		comp->SetMaxDistance(val);
	}
}

void AudioSourceEditor2::onChangeIs2D(Property* prop, bool val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		comp->SetIs2D(val);
	}
}

void AudioSourceEditor2::onDropSoundFile(TreeNode* prop, TreeNode* from)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	string fullPath = from->getPath();
	((PropString*)prop)->setValue(IO::GetFileNameWithExt(fullPath));

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioSource* comp = (AudioSource*)component;
		
		comp->SetFileName(fullPath);
		comp->SetFileGuid(ResourceMap::getResourceGuidFromName(fullPath));
	}
}

void AudioSourceEditor2::onClickSoundFile(Property* prop)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();
	Component* component = sceneNodes[0]->components.at(index);
	AudioSource* comp = (AudioSource*)component;

	MainWindow::GetAssetsWindow()->focusOnFile(comp->GetFileName());
}
