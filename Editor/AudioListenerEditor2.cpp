#include "stdafx.h"
#include "AudioListenerEditor2.h"

#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Engine/AudioListener.h"

#include "PropBool.h"
#include "PropFloat.h"

AudioListenerEditor2::AudioListenerEditor2()
{
}

AudioListenerEditor2::~AudioListenerEditor2()
{
}

void AudioListenerEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	Component* component = getSceneNodes()[0]->GetComponent(AudioListener::COMPONENT_TYPE);
	AudioListener* comp = (AudioListener*)component;

	PropBool* enabled = new PropBool(this, "Enabled", comp->GetEnabled());
	enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropFloat* volume = new PropFloat(this, "Volume", comp->GetVolume());
	volume->setOnChangeCallback([=](Property* prop, float val) { onChangeVolume(prop, val); });

	addProperty(enabled);
	addProperty(volume);
}

void AudioListenerEditor2::onChangeEnabled(Property* prop, bool val)
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

void AudioListenerEditor2::onChangeVolume(Property* prop, float val)
{
	void* data = prop->parent->getUserData();
	int index = *static_cast<int*>(data);

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		Component* component = (*it)->components.at(index);
		AudioListener* comp = (AudioListener*)component;
		comp->SetVolume(val);
	}
}
