#pragma once

#include "SoundManager.h"
#include "../Mono/include/mono/metadata/object.h"
#include "../Ogre/source/OgreMain/custom/Component.h"

#include <boost\iostreams\stream.hpp>

#include "Sound/dr_wav.h"

using namespace std;
using namespace Ogre;

class AudioSource : public Component
{
private:
	enum FileFormat { FF_OGG, FF_WAV };

	//Main
	ALfloat mVel[3];
	ALfloat mPos[3];
	bool  mLooped = false;
	ALuint mSourceID = -1;
	string fileName = "";
	string fileGuid = "";
	bool fileValid = false;
	FileFormat fileFormat;

	std::vector<AudioInfo> buffers;

	//Wav
	drwav wav;

	//Ogg
	OggVorbis_File *mVF;
	vorbis_comment *mComment;
	vorbis_info *mInfo;
	std::ifstream AudioFile;
	boost::iostreams::stream<boost::iostreams::array_source>* AudioFileZip = nullptr;
	char* AudioZipData = nullptr;

	bool mStreamed = true;
	int ReadDataBlock(ALuint BufID, size_t Size);

	//Wav callbacks
	static size_t ReadWav(void* pUserData, void* pBufferOut, size_t bytesToRead);
	static drwav_bool32 SeekWav(void* pUserData, int offset, drwav_seek_origin origin);

	//Ogg callbacks
	static size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource);
	static int SeekOgg(void *datasource, ogg_int64_t offset, int whence);
	static long TellOgg(void *datasource);
	static int CloseOgg(void *datasource);

	//Loading files functions
	bool LoadWavFile(const std::string &Filename, bool Streamed);
	bool LoadOggFile(const std::string &Filename, bool Streamed);

	void SetPosition(float X, float Y, float Z);
	bool playOnStart = false;
	bool isPlaying = false;
	float volume = 1.0f;
	float minDistance = 10.0f;
	float maxDistance = 1000.0f;
	bool isPaused = false;
	bool isPausedGlobal = false;
	bool is2D = false;
	int totalSize = 0;

	void prepareBuffers();

public:
	AudioSource(SceneNode * parent);
	virtual ~AudioSource();

	bool Open(bool Streamed);
	void SetFileName(std::string name);
	std::string GetFileName() { return fileName; }
	bool IsStreamed();
	void Play();
	void Resume();
	void ResumeGlobal();
	void Pause();
	void PauseGlobal();
	void Stop();
	void Close();
	void FreeResources();
	void Update();
	bool IsPlaying() { return isPlaying; }
	bool IsPaused() { return isPaused; }
	bool IsPausedGlobal() { return isPausedGlobal; }
	
	void SetPlayOnStart(bool play) { playOnStart = play; }
	void SetLoop(bool loop);

	bool GetPlayOnStart() { return playOnStart; }
	bool GetLooped() { return mLooped; }

	float GetVolume() { return volume; }
	void SetVolume(float value);

	float GetMinDistance() { return minDistance; }
	void SetMinDistance(float value);

	float GetMaxDistance() { return maxDistance; }
	void SetMaxDistance(float value);

	bool GetIs2D() { return is2D; }
	void SetIs2D(bool value);

	string GetFileGuid() { return fileGuid; }
	void SetFileGuid(string guid) { fileGuid = guid; }

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	virtual void SceneLoaded();
	virtual void StateChanged(bool active);
	virtual void NodeStateChanged(bool active);
};

