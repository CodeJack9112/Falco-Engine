#include "stdafx.h"
#include "API_Time.h"
#include "Engine.h"

int API_Time::getFrameRate()
{
	return 1.0 / GetEngine->GetDeltaTime();
}

float API_Time::getDeltaTime()
{
	return GetEngine->GetDeltaTime();;
}

float API_Time::getTimeScale()
{
	return GetEngine->getTimeScale();
}

void API_Time::setTimeScale(float value)
{
	GetEngine->setTimeScale(value);
}
