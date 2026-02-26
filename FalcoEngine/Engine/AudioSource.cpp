#include "stdafx.h"
#include "AudioSource.h"
#include "IO.h"
#include "Engine.h"
#include "ZipHelper.h"
#include <OgreSceneNode.h>

#define DR_WAV_IMPLEMENTATION
#include "Sound/dr_wav.h"

std::string AudioSource::COMPONENT_TYPE = "AudioSource";

AudioSource::AudioSource(SceneNode * parent) : Component(parent, GetEngine->GetMonoRuntime()->audiosource_class)
{
	for (int i = 0; i < 3; i++)
	{
		mPos[i] = mVel[i] = 0.0f;
	}

	mInfo = NULL;
	mVF = NULL;
	mComment = NULL;

	SoundManager * mgr = GetEngine->GetSoundManager();
	if (mgr != nullptr)
	{
		mgr->AddSource(this);
	}

	SceneNode * node = parent;

	//Gen audio source
	alGenSources(1, &mSourceID);

	//Default parameters
	alSourcef(mSourceID, AL_PITCH, GetEngine->getTimeScale());
	alSourcef(mSourceID, AL_GAIN, volume);
	alSourcefv(mSourceID, AL_POSITION, mPos);
	alSourcefv(mSourceID, AL_VELOCITY, mVel);
	alSourcei(mSourceID, AL_LOOPING, mLooped);
	alSourcei(mSourceID, AL_SOURCE_RELATIVE, is2D);
	alSourcei(mSourceID, AL_REFERENCE_DISTANCE, minDistance);
	alSourcei(mSourceID, AL_MAX_DISTANCE, maxDistance);
	alSource3f(mSourceID, AL_DIRECTION, 0.0, 0.0, 0.0);
}

AudioSource::~AudioSource()
{
	Close();
	
	if (alIsSource(mSourceID)) alDeleteSources(1, &mSourceID);

	SoundManager * mgr = GetEngine->GetSoundManager();
	if (mgr != nullptr)
	{
		mgr->DeleteSource(this);
	}
}

void AudioSource::SetFileName(std::string name)
{
	fileName = name;

	if (!fileName.empty())
	{
		if (GetParentSceneNode()->getVisible() && GetEnabled())
		{
			Close();
			Open(true);
		}
	}
}

bool AudioSource::Open(bool Streamed)
{
	if (GetEngine->GetEnvironment() != Engine::Environment::Player)
		return false;

	// Check if file exists
	string fullPath = GetEngine->GetAssetsPath() + fileName;

	bool read = false;

	if (GetEngine->GetUseUnpackedResources())
	{
		if (IO::FileExists(fullPath))
		{
			read = true;
		}
	}
	else
	{
		if (ZipHelper::isFileInZip(GetEngine->GetOpenedZipAssets(), fileName))
		{
			fullPath = fileName;
			read = true;
		}
	}

	if (read)
	{
		mStreamed = Streamed;
		fileValid = true;

		string Ext = IO::GetFileExtension(fileName);
		if (Ext == "ogg")
		{
			fileFormat = FileFormat::FF_OGG;
			return LoadOggFile(fullPath, Streamed);
		}

		if (Ext == "wav")
		{
			fileFormat = FileFormat::FF_WAV;
			return LoadWavFile(fullPath, Streamed);
		}
	}

	return false;
}

bool AudioSource::IsStreamed()
{
	return mStreamed;
}

void AudioSource::Play()
{
	if (isPausedGlobal)
		return;

	if (fileName.empty()) return;
	if (!fileValid) return;
	if (!GetParentSceneNode()->getVisible() || !GetEnabled()) return;

	Stop();

	prepareBuffers();

	alSourcePlay(mSourceID);

	isPlaying = true;
	isPaused = false;
}

void AudioSource::Resume()
{
	if (isPlaying)
	{
		if (isPaused)
		{
			if (!isPausedGlobal)
				alSourcePlay(mSourceID);

			isPaused = false;
		}
	}
}

void AudioSource::ResumeGlobal()
{
	if (isPlaying)
	{
		if (!isPaused)
			alSourcePlay(mSourceID);
	}

	isPausedGlobal = false;
}

void AudioSource::Pause()
{
	if (isPlaying)
	{
		if (!isPausedGlobal)
			alSourcePause(mSourceID);

		isPaused = true;
	}
}

void AudioSource::PauseGlobal()
{
	if (isPlaying)
	{
		if (!isPaused)
			alSourcePause(mSourceID);
	}

	isPausedGlobal = true;
}

void AudioSource::Stop()
{
	if (fileValid)
	{
		if (fileFormat == FF_OGG)
			ov_pcm_seek(mVF, 0);
		if (fileFormat == FF_WAV)
			drwav_seek_to_first_pcm_frame(&wav);

		int Processed = 0;
		alGetSourcei(mSourceID, AL_BUFFERS_PROCESSED, &Processed);
		if (Processed > 0)
		{
			while (Processed--)
			{
				ALuint BufID;
				alSourceUnqueueBuffers(mSourceID, 1, &BufID);
			}
		}

		alSourceStop(mSourceID);
	}

	isPlaying = false;
	isPaused = false;
}

void AudioSource::SetPosition(float X, float Y, float Z)
{
	ALfloat Pos[3] = { X, Y, Z };
	alSourcefv(mSourceID, AL_POSITION, Pos);
}

void AudioSource::SetLoop(bool loop)
{
	mLooped = loop;
	if (mSourceID > -1)
		alSourcei(mSourceID, AL_LOOPING, mLooped);
}

void AudioSource::SetVolume(float value)
{
	volume = value;
	alSourcef(mSourceID, AL_GAIN, volume);
}

void AudioSource::SetMinDistance(float value)
{
	minDistance = value;
	alSourcei(mSourceID, AL_REFERENCE_DISTANCE, minDistance);
}

void AudioSource::SetMaxDistance(float value)
{
	maxDistance = value;
	alSourcei(mSourceID, AL_MAX_DISTANCE, maxDistance);
}

void AudioSource::SetIs2D(bool value)
{
	is2D = value;
	alSourcei(mSourceID, AL_SOURCE_RELATIVE, is2D);

	if (is2D)
	{
		SetPosition(0, 0, 0);
	}
	else
	{
		Vector3 pos = GetParentSceneNode()->_getDerivedPosition();
		SetPosition(pos.x, pos.y, pos.z);
	}
}

void AudioSource::SceneLoaded()
{
	if (playOnStart)
	{
		Play();
	}
}

void AudioSource::StateChanged(bool active)
{
	if (!active)
	{
		if (IsPlaying())
			Stop();
	}
}

void AudioSource::NodeStateChanged(bool active)
{
	if (!active)
	{
		if (IsPlaying())
			Stop();
	}
}

void AudioSource::prepareBuffers()
{
	if (fileFormat == FF_OGG)
		ov_pcm_seek(mVF, 0);
	if (fileFormat == FF_WAV)
		drwav_seek_to_first_pcm_frame(&wav);

	if (mStreamed)
	{
		int Processed = 0;
		alGetSourcei(mSourceID, AL_BUFFERS_PROCESSED, &Processed);
		if (Processed > 0)
		{
			while (Processed--)
			{
				ALuint BufID;
				alSourceUnqueueBuffers(mSourceID, 1, &BufID);
			}
		}

		for (auto it = buffers.begin(); it != buffers.end(); ++it)
		{
			ReadDataBlock(it->ID, DYNBUF_SIZE);
			alSourceQueueBuffers(mSourceID, 1, &it->ID);
		}
	}
	else
	{
		int BlockSize = ov_pcm_total(mVF, -1) * 4;

		for (auto it = buffers.begin(); it != buffers.end(); ++it)
		{
			ReadDataBlock(it->ID, BlockSize);
			alSourcei(mSourceID, AL_BUFFER, it->ID);
		}
	}
}

bool AudioSource::LoadWavFile(const std::string& Filename, bool Streamed)
{
	if (GetEngine->GetUseUnpackedResources())
	{
		AudioFile.open(Filename.c_str(), ios_base::in | ios_base::binary);

		if (!drwav_init(&wav, ReadWav, SeekWav, &AudioFile, NULL))
		{
			return false;
		}
	}
	else
	{
		int sz = 0;
		AudioZipData = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), Filename, sz);
		AudioFileZip = new boost::iostreams::stream<boost::iostreams::array_source>(AudioZipData, sz);

		if (!drwav_init(&wav, ReadWav, SeekWav, AudioFileZip, NULL))
		{
			return false;
		}
	}

	totalSize = wav.dataChunkDataSize;

	if (!Streamed)
	{
		AudioInfo buffer;

		buffer.Rate = wav.sampleRate;
		buffer.Format = (wav.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

		alGenBuffers(1, &buffer.ID);
		buffers.push_back(buffer);
	}
	else
	{
		alSourcei(mSourceID, AL_LOOPING, AL_FALSE);

		for (int i = 0; i < NUM_OF_DYNBUF; i++)
		{
			AudioInfo buffer;

			buffer.Rate = wav.sampleRate;
			buffer.Format = (wav.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

			alGenBuffers(1, &buffer.ID);
			buffers.push_back(buffer);
		}
	}

	return true;
}

bool AudioSource::LoadOggFile(const std::string & Filename, bool Streamed)
{
	// Структура с функциями обратного вызова.
	ov_callbacks cb;

	// Заполняем структуру cb
	cb.close_func = CloseOgg;
	cb.read_func = ReadOgg;
	cb.seek_func = SeekOgg;
	cb.tell_func = TellOgg;

	// Создаем структуру OggVorbis_File
	mVF = new OggVorbis_File;

	if (GetEngine->GetUseUnpackedResources())
	{
		// Открываем OGG файл как бинарный
		AudioFile.open(Filename.c_str(), ios_base::in | ios_base::binary);
		
		// Инициализируем файл средствами vorbisfile
		if (ov_open_callbacks(&AudioFile, mVF, NULL, -1, cb) < 0)
		{
			// Если ошибка, то открываемый файл не является OGG
			return false;
		}
	}
	else
	{
		int sz = 0;
		AudioZipData = ZipHelper::readFileFromZip(GetEngine->GetOpenedZipAssets(), Filename, sz);
		AudioFileZip = new boost::iostreams::stream<boost::iostreams::array_source>(AudioZipData, sz);

		if (ov_open_callbacks(AudioFileZip, mVF, NULL, -1, cb) < 0)
		{
			// Если ошибка, то открываемый файл не является OGG
			return false;
		}
	}

	totalSize = ov_pcm_total(mVF, -1) * 4;

	// Получаем комментарии и информацию о файле
	mComment = ov_comment(mVF, -1);
	mInfo = ov_info(mVF, -1);

	if (!Streamed)
	{
		AudioInfo buffer;

		buffer.Rate = mInfo->rate;
		buffer.Format = (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

		alGenBuffers(1, &buffer.ID);
		buffers.push_back(buffer);
	}
	else
	{
		alSourcei(mSourceID, AL_LOOPING, AL_FALSE);

		for (int i = 0; i < NUM_OF_DYNBUF; i++)
		{
			AudioInfo buffer;

			buffer.Rate = mInfo->rate;
			buffer.Format = (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

			alGenBuffers(1, &buffer.ID);
			buffers.push_back(buffer);
		}
	}

	return true;
}

int AudioSource::ReadDataBlock(ALuint BufID, size_t Size)
{
	int eof = 0;
	long ret = 0;
	int current_section = 0;

	char *PCM;
	char *PCM2;

	if (Size < 1) return 0;
	PCM = new char[Size];

	if (fileFormat == FF_OGG)
	{
		while (ret < Size && eof == 0)
		{
			long result = ov_read(mVF, PCM + ret, Size - ret, 0, 2, 1, &current_section);

			if (result == 0)
			{
				eof = 1;
			}
			else if (result < 0)
			{
			}
			else
			{
				ret += result;
			}
		}
	}
	if (fileFormat == FF_WAV)
	{
		ret = drwav_read_raw(&wav, Size, PCM);
		//if (ret == 0)
		//	eof = 1;
	}

	//Slice zeroes at the end because of read block smaller then dynamic block size
	PCM2 = new char[ret];
	memcpy(reinterpret_cast<char*>(PCM2), reinterpret_cast<char*>(PCM), ret);
	delete[] PCM;
	PCM = PCM2;

	auto it = find_if(buffers.begin(), buffers.end(), [=](const AudioInfo& a) -> bool { return a.ID == BufID; });
	if (it != buffers.end())
	{
		AudioInfo inf = *it;

		if (inf.Format == AL_FORMAT_STEREO16)
		{
			if (!is2D)
			{
				long sz = ret / 2;

				//If the buffer has 2 channels, convert it to single channel to support 3d sound.
				char* pcm = new char[sz];

				int j = 0;
				for (int i = 0; i < ret; i += 2, ++j)
				{
					int16_t r = (static_cast<int16_t>(PCM[i + 1]) + static_cast<int16_t>(PCM[i + 3])) * 0.5;
					pcm[j] = r;
				}

				alBufferData(BufID, AL_FORMAT_MONO16, (void*)pcm, sz, inf.Rate);
				delete[] pcm;
			}
			else
				alBufferData(BufID, inf.Format, (void*)PCM, ret, inf.Rate);
		}
		else
		{
			alBufferData(BufID, inf.Format, (void*)PCM, ret, inf.Rate);
		}
	}

	if (SoundManager::CheckALError())
	{
		ret = -10;
	}

	if (eof == 1)
		ret = -20;

	delete[] PCM;
	return ret;
}

size_t AudioSource::ReadWav(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
	if (GetEngine->GetUseUnpackedResources())
	{
		istream* File = reinterpret_cast<istream*>(pUserData);
		File->read((char*)pBufferOut, bytesToRead);
		int gc = File->gcount();
		return gc;
	}
	else
	{
		boost::iostreams::stream<boost::iostreams::array_source>* File = reinterpret_cast<boost::iostreams::stream<boost::iostreams::array_source>*>(pUserData);
		File->read((char*)pBufferOut, bytesToRead);
		int gc = File->gcount();
		return gc;
	}
}

drwav_bool32 AudioSource::SeekWav(void* pUserData, int offset, drwav_seek_origin origin)
{
	if (GetEngine->GetUseUnpackedResources())
	{
		istream* File = reinterpret_cast<istream*>(pUserData);
		ios_base::seekdir Dir;
		File->clear();
		switch (origin)
		{
			case drwav_seek_origin_start: Dir = ios::beg;  break;
			case drwav_seek_origin_current: Dir = ios::cur;  break;
			default: Dir = ios::end; break;
		}
		File->seekg((streamoff)offset, Dir);
		return (File->fail() ? false : true);
	}
	else
	{
		boost::iostreams::stream<boost::iostreams::array_source>* File = reinterpret_cast<boost::iostreams::stream<boost::iostreams::array_source>*>(pUserData);
		ios_base::seekdir Dir;
		File->clear();
		switch (origin)
		{
			case drwav_seek_origin_start: Dir = ios::beg;  break;
			case drwav_seek_origin_current: Dir = ios::cur;  break;
			default: Dir = ios::end; break;
		}
		File->seekg((streamoff)offset, Dir);
		return (File->fail() ? false : true);
	}
}

size_t AudioSource::ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	if (GetEngine->GetUseUnpackedResources())
	{
		istream* File = reinterpret_cast<istream*>(datasource);
		File->read((char*)ptr, size * nmemb);
		int gc = File->gcount();
		return gc;
	}
	else
	{
		boost::iostreams::stream<boost::iostreams::array_source> * File = reinterpret_cast<boost::iostreams::stream<boost::iostreams::array_source>*>(datasource);
		File->read((char*)ptr, size * nmemb);
		int gc = File->gcount();
		return gc;
	}
}

int AudioSource::SeekOgg(void *datasource, ogg_int64_t offset, int whence)
{
	if (GetEngine->GetUseUnpackedResources())
	{
		istream* File = reinterpret_cast<istream*>(datasource);
		ios_base::seekdir Dir;
		File->clear();
		switch (whence)
		{
			case SEEK_SET: Dir = ios::beg;  break;
			case SEEK_CUR: Dir = ios::cur;  break;
			case SEEK_END: Dir = ios::end;  break;
			default: return -1;
		}
		File->seekg((streamoff)offset, Dir);
		return (File->fail() ? -1 : 0);
	}
	else
	{
		boost::iostreams::stream<boost::iostreams::array_source>* File = reinterpret_cast<boost::iostreams::stream<boost::iostreams::array_source>*>(datasource);
		ios_base::seekdir Dir;
		File->clear();
		switch (whence)
		{
			case SEEK_SET: Dir = ios::beg;  break;
			case SEEK_CUR: Dir = ios::cur;  break;
			case SEEK_END: Dir = ios::end;  break;
			default: return -1;
		}
		File->seekg((streamoff)offset, Dir);
		return (File->fail() ? -1 : 0);
	}
}

long AudioSource::TellOgg(void *datasource)
{
	if (GetEngine->GetUseUnpackedResources())
	{
		istream* File = reinterpret_cast<istream*>(datasource);
		return File->tellg();
	}
	else
	{
		boost::iostreams::stream<boost::iostreams::array_source>* File = reinterpret_cast<boost::iostreams::stream<boost::iostreams::array_source>*>(datasource);
		return File->tellg();
	}
}

int AudioSource::CloseOgg(void *datasource)
{
	return 0;
}

void AudioSource::Update()
{
	if (!fileValid)
		return;

	if (mSourceID == -1)
		return;

	if (!is2D)
	{
		Vector3 pos = GetParentSceneNode()->_getDerivedPosition();
		SetPosition(pos.x, pos.y, pos.z);
	}

	alSourcef(mSourceID, AL_PITCH, GetEngine->getTimeScale());

	if (!mStreamed) return;
	if (!isPlaying) return;

	if (isPaused || isPausedGlobal)
		return;

	ALenum state;
	alGetSourcei(mSourceID, AL_SOURCE_STATE, &state);

	ALint Processed = 0;

	// Получаем количество отработанных буферов
	alGetSourcei(mSourceID, AL_BUFFERS_PROCESSED, &Processed);

	bool shouldStop = false;

	while (Processed--)
	{
		ALuint BufID;

		alSourceUnqueueBuffers(mSourceID, 1, &BufID);
		int ret = ReadDataBlock(BufID, DYNBUF_SIZE);
		alSourceQueueBuffers(mSourceID, 1, &BufID);

		if (ret == -20)
			shouldStop = true;
	}

	if (state == AL_STOPPED || shouldStop)
	{
		if (!mLooped)
		{
			Stop();
		}
		else
		{
			Stop();
			Play();
		}
	}
}

void AudioSource::Close()
{
	Stop();
	FreeResources();
}

void AudioSource::FreeResources()
{
	fileValid = false;

	for (auto i = buffers.begin(); i != buffers.end(); i++)
		alDeleteBuffers(1, &i->ID);

	buffers.clear();

	if (AudioFileZip != nullptr)
	{
		delete AudioFileZip;
	}

	if (AudioZipData != nullptr)
	{
		delete[] AudioZipData;
	}

	if (fileFormat == FF_OGG)
	{
		if (!mVF)
		{
			ov_clear(mVF);
			delete mVF;
			mVF = nullptr;
		}
	}

	if (fileFormat == FF_WAV)
		drwav_uninit(&wav);

	AudioFile.close();
}
