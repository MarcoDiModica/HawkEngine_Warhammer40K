#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <al.h>
#include <alc.h>
#include "AudioFile.h"

namespace MyEngine {

class AudioAsset;

class AudioEngine {
public:
    static AudioEngine& GetInstance();
    
    bool Initialize();
    void Shutdown();
    
    // Asset management
    std::shared_ptr<AudioAsset> LoadAudioAsset(const std::string& filepath);
    void UnloadAudioAsset(const std::string& filepath);
    
    // Background music control
    void PlayMusic(const std::string& filepath, bool loop = true);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    void SetMusicVolume(float volume);
    
    // Global audio control
    void SetMasterVolume(float volume);
    float GetMasterVolume() const;
    
private:
    AudioEngine();
    ~AudioEngine();
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;
    
    ALCdevice* device;
    ALCcontext* context;
    float masterVolume;
    
    std::unordered_map<std::string, std::shared_ptr<AudioAsset>> audioAssets;
    ALuint currentMusicSource;
    bool isMusicPlaying;
};

} // namespace MyEngine 