#pragma once

#include "API.h"

class API_Animation
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Animation::INTERNAL_play", (void*)play);
		mono_add_internal_call("FalcoEngine.Animation::INTERNAL_crossFade", (void*)crossFade);
		mono_add_internal_call("FalcoEngine.Animation::INTERNAL_stop", (void*)stop);
		mono_add_internal_call("FalcoEngine.Animation::INTERNAL_setLoop", (void*)setLoop);
		mono_add_internal_call("FalcoEngine.Animation::INTERNAL_getLoop", (void*)getLoop);
		mono_add_internal_call("FalcoEngine.Animation::INTERNAL_isPlaying", (void*)isPlayingByName);
		mono_add_internal_call("FalcoEngine.Animation::get_isPlaying", (void*)isPlaying);
	}

private:
	//Play
	static void play(MonoObject * this_ptr, MonoString * name);

	//Cross fade
	static void crossFade(MonoObject* this_ptr, MonoString* name, float duration, int playMode, int prevStatePosition, int currentStatePosition);

	//Stop
	static void stop(MonoObject * this_ptr);

	//Set loop
	static void setLoop(MonoObject * this_ptr, MonoString * name, bool loop);

	//Get loop
	static bool getLoop(MonoObject * this_ptr, MonoString * name);

	//Is playing by name
	static bool isPlayingByName(MonoObject * this_ptr, MonoString * name);

	//Is playing global
	static bool isPlaying(MonoObject * this_ptr);
};

