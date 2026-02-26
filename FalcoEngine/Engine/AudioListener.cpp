#include "stdafx.h"
#include "AudioListener.h"
#include "Engine.h"

std::string AudioListener::COMPONENT_TYPE = "AudioListener";

AudioListener::AudioListener(SceneNode * parent) : Component(parent, GetEngine->GetMonoRuntime()->audiolistener_class)
{
	SoundManager * mgr = GetEngine->GetSoundManager();

	if (mgr != nullptr)
	{
		mgr->SetListener(this);
	}
}

AudioListener::~AudioListener()
{
	SoundManager * mgr = GetEngine->GetSoundManager();

	if (mgr != nullptr)
	{
		if (mgr->GetListener() == this)
		{
			mgr->SetListener(nullptr);
		}
	}
}

void AudioListener::SetVolume(float value)
{
	volume = value;

	SoundManager* mgr = GetEngine->GetSoundManager();
	mgr->SetMasterVolume(value);
}

bool AudioListener::GetPaused()
{
	SoundManager* mgr = GetEngine->GetSoundManager();
	return mgr->GetPaused();
}

void AudioListener::SetPaused(bool value)
{
	SoundManager* mgr = GetEngine->GetSoundManager();
	mgr->SetPaused(value);
}
