#include "stdafx.h"
#include "API_AudioSource.h"
#include "Engine.h"
#include "AudioSource.h"
#include "StringConverter.h"
#include "ResourceMap.h"

#include <OgreSceneNode.h>

void API_AudioSource::play(MonoObject * this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->Play();
}

void API_AudioSource::pause(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->Pause();
}

void API_AudioSource::resume(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->Resume();
}

void API_AudioSource::stop(MonoObject * this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->Stop();
}

bool API_AudioSource::isPlaying(MonoObject * this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->IsPlaying();

	return false;
}

bool API_AudioSource::isPaused(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->IsPaused();

	return false;
}

bool API_AudioSource::getLoop(MonoObject * this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->GetLooped();

	return false;
}

void API_AudioSource::setLoop(MonoObject * this_ptr, bool loop)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->SetLoop(loop);
}

float API_AudioSource::getVolume(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->GetVolume();

	return 0.0f;
}

void API_AudioSource::setVolume(MonoObject* this_ptr, float volume)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->SetVolume(volume);
}

float API_AudioSource::getMinDistance(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->GetMinDistance();

	return 0.0f;
}

void API_AudioSource::setMinDistance(MonoObject* this_ptr, float value)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->SetMinDistance(value);
}

float API_AudioSource::getMaxDistance(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->GetMaxDistance();

	return 0.0f;
}

void API_AudioSource::setMaxDistance(MonoObject* this_ptr, float value)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->SetMaxDistance(value);
}

bool API_AudioSource::getIs2D(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		return audio->GetIs2D();

	return false;
}

void API_AudioSource::setIs2D(MonoObject* this_ptr, bool value)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
		audio->SetIs2D(value);
}

MonoString* API_AudioSource::getFileName(MonoObject* this_ptr)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
	{
		std::string _audioPath = ResourceMap::getResourceNameFromGuid(audio->GetFileGuid());
		if (_audioPath.empty())
			_audioPath = audio->GetFileName();

		MonoString* str = mono_string_new(GetEngine->GetMonoRuntime()->GetDomain(), CP_UNI(_audioPath).c_str());
	}

	return false;
}

void API_AudioSource::setFileName(MonoObject* this_ptr, MonoString* value)
{
	AudioSource* audio = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&audio));

	if (audio != nullptr)
	{
		std::string _str = (const char*)mono_string_to_utf8((MonoString*)value);
		_str = CP_SYS(_str);

		audio->SetFileName(_str);
		audio->SetFileGuid(ResourceMap::getResourceGuidFromName(_str));
	}
}
