#pragma once

#include "Engine.h"

//OpenAL
#include "../OpenAL/include/al.h"
#include "../OpenAL/include/alc.h"
#include "../OpenAL/include/alu.h"
#include "../OpenAL/include/alut.h"

//Ogg-Vorbis
#include "../OpenAL/ogg/vorbis/codec.h"
#include "../OpenAL/ogg/vorbis/vorbisfile.h"

//Std
#include <string>
#include <map>

#define NUM_OF_DYNBUF	4		// Num buffers in queue
#define DYNBUF_SIZE		65536	// Buffer size

using namespace std;

struct AudioInfo
{
	unsigned int  ID;
	unsigned int  Rate;
	unsigned int  Format;
};

//typedef std::map<ALuint, AudioInfo> BufferList;

class AudioSource;
class AudioListener;

class SoundManager
{
private:
	ALCdevice * pDevice;
	ALCcontext * pContext;
	std::vector<AudioSource*> sources;
	AudioListener * listener;

	bool paused = false;

public:
	SoundManager();
	~SoundManager();

	//static std::map<ALuint, AudioInfo> Buffers;

	bool Init();
	void Update();
	void AddSource(AudioSource * src) { sources.push_back(src); }
	void DeleteSource(AudioSource* src);
	void Destroy();
	void SetMasterVolume(float volume);

	static ALboolean CheckALCError(ALCdevice * pDevice);
	static ALboolean CheckALError();

	void SetListener(AudioListener * listener) { this->listener = listener; }
	AudioListener * GetListener() { return this->listener; }

	bool GetPaused() { return paused; }
	void SetPaused(bool value);
};