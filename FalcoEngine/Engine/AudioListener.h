#pragma once

#include "SoundManager.h"
#include "../Ogre/source/OgreMain/custom/Component.h"
#include <string>

using namespace std;

class AudioListener : public Component
{
private:
	float volume = 1.0f;

public:
	AudioListener(SceneNode * parent);
	virtual ~AudioListener();

	float GetVolume() { return volume; }
	void SetVolume(float value);

	bool GetPaused();
	void SetPaused(bool value);

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }
};

