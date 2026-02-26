#pragma once

#include "API.h"

class API_Time
{
public:
	//Register methods
	static void Register()
	{
		mono_add_internal_call("FalcoEngine.Time::get_frameRate", (void*)getFrameRate);
		mono_add_internal_call("FalcoEngine.Time::get_deltaTime", (void*)getDeltaTime);
		mono_add_internal_call("FalcoEngine.Time::get_timeScale", (void*)getTimeScale);
		mono_add_internal_call("FalcoEngine.Time::set_timeScale", (void*)setTimeScale);
	}

private:
	static int getFrameRate();
	static float getDeltaTime();
	static float getTimeScale();
	static void setTimeScale(float value);
};

