#pragma once
#undef max
#undef T 
#include "fmod.hpp"
#include "fmod_studio.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <glm/glm.hpp>

struct Implementation {
    Implementation();
    ~Implementation();

    void Update();

    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;

    int mnNextChannelId;
    float masterVolume = 1;

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    std::map<int, float> mChannelBaseVolumes; // Channel ID -> Base Volume


    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine();
    static void Init();
    static void Update();
    static void Shutdown();

public :

    void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const std::string& strEventName);
    void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string& strSoundName);
    void Set3dListenerAndOrientation(const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    int PlaySound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    void PlayEvent(const std::string& strEventName);
	int GetChannelId(const std::string& strSoundName);
    void StopSound(int nChannelId);
	void PauseSound(int nChannelId);
	void ResumeSound(int nChannelId);
    void StopEvent(const std::string& strEventName, bool bImmediate = false);
    void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
    void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
    void StopAllChannels();
	void PauseAllChannels();
	void ResumeAllChannels();
    void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);
	void SetMasterVolume(float fVolumedB);
    bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const std::string& strEventName) const;
    static float dbToVolume(float db);
    static float VolumeTodb(float volume);
    FMOD_VECTOR VectorToFmod(const glm::vec3& vPosition);


};

