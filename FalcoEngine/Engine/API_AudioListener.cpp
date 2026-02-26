#include "API_AudioListener.h"
#include "Engine.h"
#include "AudioListener.h"
#include <OgreSceneNode.h>

float API_AudioListener::getVolume(MonoObject* this_ptr)
{
	/*SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));*/

	AudioListener* listener = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&listener));

	if (listener != nullptr)
	{
		return listener->GetVolume();
	}

	return 0.0f;
}

void API_AudioListener::setVolume(MonoObject* this_ptr, float volume)
{
	AudioListener* listener = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&listener));

	if (listener != nullptr)
	{
		listener->SetVolume(volume);
	}
}

bool API_AudioListener::getPaused(MonoObject* this_ptr)
{
	AudioListener* listener = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&listener));

	if (listener != nullptr)
	{
		return listener->GetPaused();
	}

	return false;
}

void API_AudioListener::setPaused(MonoObject* this_ptr, bool value)
{
	AudioListener* listener = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&listener));

	if (listener != nullptr)
	{
		listener->SetPaused(value);
	}
}
