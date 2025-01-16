#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "AudioAsset.h"

namespace MyGameEngine {

class AudioEngine {
public:
    // Represents a playable sound source
    struct AudioSource {
        ALuint sourceId;
        std::shared_ptr<AudioAsset> asset;
        bool isLooping;
        bool isMusic;  // true for background music, false for sound effects
    };

    AudioEngine();
    ~AudioEngine();

    bool Initialize();
    void Shutdown();

    // Asset management
    std::shared_ptr<AudioAsset> LoadAudioAsset(const std::string& filePath);
    
    // Playback control
    ALuint PlaySound(std::shared_ptr<AudioAsset> asset, bool isLooping = false, bool isMusic = false);
    void StopSound(ALuint sourceId);
    void PauseSound(ALuint sourceId);
    void ResumeSound(ALuint sourceId);
    
    // Volume control
    void SetVolume(ALuint sourceId, float volume);  // volume range: 0.0f to 1.0f
    void SetMasterVolume(float volume);
    
    // Spatial audio
    void SetSourcePosition(ALuint sourceId, float x, float y, float z);
    void SetListenerPosition(float x, float y, float z);
    
    // State queries
    bool IsPlaying(ALuint sourceId) const;
    bool IsInitialized() const { return m_Initialized; }

private:
    ALuint CreateAudioSource();
    void DestroyAudioSource(ALuint sourceId);
    void CleanupStoppedSources();

    ALCdevice* m_Device;
    ALCcontext* m_Context;
    bool m_Initialized;
    float m_MasterVolume;
    
    std::unordered_map<ALuint, AudioSource> m_ActiveSources;
    std::unordered_map<std::string, std::weak_ptr<AudioAsset>> m_AudioAssets;
};

}