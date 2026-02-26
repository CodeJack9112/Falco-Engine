#include "stdafx.h"
#include "API_Animation.h"
#include "AnimationList.h"
#include "Engine.h"
#include "StringConverter.h"

void API_Animation::play(MonoObject * this_ptr, MonoString * name)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		std::string animName = (const char*)mono_string_to_utf8((MonoString*)name);
		animName = CP_SYS(animName);

		Engine::AnimationPlay(node, animName, true, 0);
	}
}

void API_Animation::crossFade(MonoObject* this_ptr, MonoString* name, float duration, int playMode, int prevStatePosition, int currentStatePosition)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		std::string animName = (const char*)mono_string_to_utf8((MonoString*)name);
		animName = CP_SYS(animName);

		Engine::AnimationPlay(node, animName, true, duration, playMode, prevStatePosition, currentStatePosition);
	}
}

void API_Animation::stop(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
		{
			Engine::AnimationPlay(node, it->name, false);
		}
	}
}

void API_Animation::setLoop(MonoObject * this_ptr, MonoString * name, bool loop)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		std::string animName = (const char*)mono_string_to_utf8((MonoString*)name);
		animName = CP_SYS(animName);

		Engine::AnimationSetLoop(node, animName, loop);
	}
}

bool API_Animation::getLoop(MonoObject * this_ptr, MonoString * name)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		std::string animName = (const char*)mono_string_to_utf8((MonoString*)name);
		animName = CP_SYS(animName);

		return Engine::AnimationIsLoop(node, animName);
	}

	return false;
}

bool API_Animation::isPlayingByName(MonoObject * this_ptr, MonoString * name)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		std::string animName = (const char*)mono_string_to_utf8((MonoString*)name);
		animName = CP_SYS(animName);

		return Engine::AnimationIsPlaying(node, animName);
	}
	
	return false;
}

bool API_Animation::isPlaying(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_native_ptr_field, reinterpret_cast<void*>(&node));

	AnimationList* animList = nullptr;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->component_this_ptr_field, reinterpret_cast<void*>(&animList));

	if (animList != nullptr)
	{
		for (AnimationList::AnimationDataList::iterator it = animList->GetAnimationDataList().begin(); it != animList->GetAnimationDataList().end(); ++it)
		{
			if (Engine::AnimationIsPlaying(node, it->name))
				return true;
		}
	}

	return false;
}
