#pragma once

#include "fmod_studio.hpp"
#include "fmod.hpp"
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

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

class AudioEngine
{
private:
    static void Start();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

public :

    void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const std::string& strEventName);
    void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string& strSoundName);
    void Set3dListenerAndOrientation(const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    int PlaySound(const std::string& strSoundName, const glm::vec3& vPos = glm::vec3{ 0, 0, 0 }, float fVolumedB = 0.0f);
    void PlayEvent(const std::string& strEventName);
    void StopChannel(int nChannelId);
    void StopEvent(const std::string& strEventName, bool bImmediate = false);
    void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter);
    void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue);
    void StopAllChannels();
    void SetChannel3dPosition(int nChannelId, const glm::vec3& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);
    bool IsPlaying(int nChannelId) const;
    bool IsEventPlaying(const std::string& strEventName) const;
    float dbToVolume(float db);
    float VolumeTodb(float volume);
    FMOD_VECTOR VectorToFmod(const glm::vec3& vPosition);


};

