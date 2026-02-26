#include "stdafx.h"
#include "SoundManager.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include <OgreLogManager.h>

//BufferList SoundManager::Buffers;

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

bool SoundManager::Init()
{
	// Позиция слушателя.
	ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

	// Скорость слушателя.
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

	// Ориентация слушателя. (Первые 3 элемента – направление «на», последние 3 – «вверх»)
	ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

	// Открываем заданное по умолчанию устройство
	pDevice = alcOpenDevice("");

	// Проверка на ошибки
	if (!pDevice)
	{
		//MessageBoxA(0, "Default sound device not present", "Error", MB_OK | MB_ICONERROR);
		LogManager::getSingleton().logError("Default sound device not present");
		return false;
	}

	// Создаем контекст рендеринга
	pContext = alcCreateContext(pDevice, NULL);

	if (CheckALCError(pDevice)) return false;

	// Делаем контекст текущим
	alcMakeContextCurrent(pContext);

	// Устанавливаем параметры слушателя
	// Позиция
	alListenerfv(AL_POSITION, ListenerPos);
	// Скорость
	alListenerfv(AL_VELOCITY, ListenerVel);
	// Ориентация
	alListenerfv(AL_ORIENTATION, ListenerOri);
	//Громкость
	if (listener != nullptr)
		alListenerf(AL_GAIN, listener->GetVolume());

	alDistanceModel(AL_LINEAR_DISTANCE);

	return true;
}

void SoundManager::Update()
{
	if (listener != nullptr)
	{
		ALfloat ListenerPos[] = {
			listener->GetParentSceneNode()->_getDerivedPosition().x,
			listener->GetParentSceneNode()->_getDerivedPosition().y,
			listener->GetParentSceneNode()->_getDerivedPosition().z
		};

		Euler rot = Euler(listener->GetParentSceneNode()->_getDerivedOrientation());
		Vector3 up = listener->GetParentSceneNode()->_getDerivedOrientation() * Vector3::UNIT_Y;
		
		ALfloat ListenerOri[] = {
			rot.forward().x,
			rot.forward().y,
			rot.forward().z,
			up.x,
			up.y,
			up.z
		};

		alListenerfv(AL_POSITION, ListenerPos);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}

	for (std::vector<AudioSource*>::iterator it = sources.begin(); it != sources.end(); ++it)
	{
		(*it)->Update();
	}
}

void SoundManager::DeleteSource(AudioSource* src)
{
	sources.erase(std::remove(sources.begin(), sources.end(), src), sources.end());
}

void SoundManager::Destroy()
{
	// Выключаем текущий контекст
	alcMakeContextCurrent(NULL);
	// Уничтожаем контекст
	alcDestroyContext(pContext);
	// Закрываем звуковое устройство
	alcCloseDevice(pDevice);
}

void SoundManager::SetMasterVolume(float volume)
{
	alListenerf(AL_GAIN, volume);
}

ALboolean SoundManager::CheckALCError(ALCdevice * pDevice)
{
	ALenum ErrCode;
	string Err = "ALC error: ";
	if ((ErrCode = alcGetError(pDevice)) != ALC_NO_ERROR)
	{
		Err += (char *)alcGetString(pDevice, ErrCode);
		//MessageBoxA(0, Err.data(), "Error", MB_OK | MB_ICONERROR);
		LogManager::getSingleton().logError(Err.data());
		return AL_TRUE;
	}
	return AL_FALSE;
}

ALboolean SoundManager::CheckALError()
{
	ALenum ErrCode;
	string Err = "OpenAL error: ";
	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		Err += (char *)alGetString(ErrCode);
		//MessageBoxA(0, Err.data(), "Error", MB_OK | MB_ICONERROR);
		LogManager::getSingleton().logError(Err.data());
		return AL_TRUE;
	}

	return AL_FALSE;
}

void SoundManager::SetPaused(bool value)
{
	if (paused != value)
	{
		paused = value;

		if (paused)
		{
			for (auto it = sources.begin(); it != sources.end(); ++it)
			{
				AudioSource* src = *it;
				src->PauseGlobal();
			}
		}
		else
		{
			for (auto it = sources.begin(); it != sources.end(); ++it)
			{
				AudioSource* src = *it;
				src->ResumeGlobal();
			}
		}
	}
}
