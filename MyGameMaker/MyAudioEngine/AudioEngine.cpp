#include "AudioEngine.h"
#include <iostream>

namespace MyGameEngine {

AudioEngine::AudioEngine()
    : m_Device(nullptr)
    , m_Context(nullptr)
    , m_Initialized(false)
    , m_MasterVolume(1.0f)
{
}

AudioEngine::~AudioEngine() {
    Shutdown();
}

bool AudioEngine::Initialize() {
    // Open the default device
    m_Device = alcOpenDevice(nullptr);
    if (!m_Device) {
        std::cout << "Failed to open default audio device!" << std::endl;
        return false;
    }

    // Create and set the context
    m_Context = alcCreateContext(m_Device, nullptr);
    if (!m_Context) {
        std::cout << "Failed to create audio context!" << std::endl;
        alcCloseDevice(m_Device);
        m_Device = nullptr;
        return false;
    }

    if (!alcMakeContextCurrent(m_Context)) {
        std::cout << "Failed to make audio context current!" << std::endl;
        alcDestroyContext(m_Context);
        alcCloseDevice(m_Device);
        m_Context = nullptr;
        m_Device = nullptr;
        return false;
    }

    // Set default listener properties
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    float orientation[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    alListenerfv(AL_ORIENTATION, orientation);

    m_Initialized = true;
    std::cout << "Audio Engine initialized successfully!" << std::endl;
    
    // Print OpenAL version and vendor info
    std::cout << "OpenAL Vendor: " << alGetString(AL_VENDOR) << std::endl;
    std::cout << "OpenAL Version: " << alGetString(AL_VERSION) << std::endl;
    std::cout << "OpenAL Renderer: " << alGetString(AL_RENDERER) << std::endl;

    return true;
}

void AudioEngine::Shutdown() {
    // Stop and destroy all active sources
    for (auto& [sourceId, source] : m_ActiveSources) {
        StopSound(sourceId);
        DestroyAudioSource(sourceId);
    }
    m_ActiveSources.clear();
    m_AudioAssets.clear();

    if (m_Context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_Context);
        m_Context = nullptr;
    }
    
    if (m_Device) {
        alcCloseDevice(m_Device);
        m_Device = nullptr;
    }

    m_Initialized = false;
}

std::shared_ptr<AudioAsset> AudioEngine::LoadAudioAsset(const std::string& filePath) {
    // Check if asset is already loaded
    auto it = m_AudioAssets.find(filePath);
    if (it != m_AudioAssets.end()) {
        if (auto asset = it->second.lock()) {
            return asset;
        }
        // If weak_ptr is expired, remove it
        m_AudioAssets.erase(it);
    }

    // Create and load new asset
    auto asset = std::make_shared<AudioAsset>();
    if (!asset->LoadFromFile(filePath)) {
        return nullptr;
    }

    m_AudioAssets[filePath] = asset;
    return asset;
}

ALuint AudioEngine::PlaySound(std::shared_ptr<AudioAsset> asset, bool isLooping, bool isMusic) {
    if (!asset || !asset->IsValid()) {
        return 0;
    }

    ALuint sourceId = CreateAudioSource();
    if (sourceId == 0) {
        return 0;
    }

    // Configure the source
    alSourcei(sourceId, AL_BUFFER, asset->GetBuffer());
    alSourcef(sourceId, AL_GAIN, m_MasterVolume);
    alSourcei(sourceId, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
    
    // Store source information
    AudioSource source{sourceId, asset, isLooping, isMusic};
    m_ActiveSources[sourceId] = source;

    // Start playback
    alSourcePlay(sourceId);
    
    return sourceId;
}

void AudioEngine::StopSound(ALuint sourceId) {
    auto it = m_ActiveSources.find(sourceId);
    if (it != m_ActiveSources.end()) {
        alSourceStop(sourceId);
    }
}

void AudioEngine::PauseSound(ALuint sourceId) {
    auto it = m_ActiveSources.find(sourceId);
    if (it != m_ActiveSources.end()) {
        alSourcePause(sourceId);
    }
}

void AudioEngine::ResumeSound(ALuint sourceId) {
    auto it = m_ActiveSources.find(sourceId);
    if (it != m_ActiveSources.end()) {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        if (state == AL_PAUSED) {
            alSourcePlay(sourceId);
        }
    }
}

void AudioEngine::SetVolume(ALuint sourceId, float volume) {
    auto it = m_ActiveSources.find(sourceId);
    if (it != m_ActiveSources.end()) {
        volume = std::max(0.0f, std::min(volume, 1.0f));
        alSourcef(sourceId, AL_GAIN, volume * m_MasterVolume);
    }
}

void AudioEngine::SetMasterVolume(float volume) {
    m_MasterVolume = std::max(0.0f, std::min(volume, 1.0f));
    
    // Update all active sources
    for (auto& [sourceId, source] : m_ActiveSources) {
        ALfloat currentVolume;
        alGetSourcef(sourceId, AL_GAIN, &currentVolume);
        alSourcef(sourceId, AL_GAIN, (currentVolume / m_MasterVolume) * m_MasterVolume);
    }
}

void AudioEngine::SetSourcePosition(ALuint sourceId, float x, float y, float z) {
    auto it = m_ActiveSources.find(sourceId);
    if (it != m_ActiveSources.end()) {
        alSource3f(sourceId, AL_POSITION, x, y, z);
    }
}

void AudioEngine::SetListenerPosition(float x, float y, float z) {
    alListener3f(AL_POSITION, x, y, z);
}

bool AudioEngine::IsPlaying(ALuint sourceId) const {
    auto it = m_ActiveSources.find(sourceId);
    if (it != m_ActiveSources.end()) {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }
    return false;
}

ALuint AudioEngine::CreateAudioSource() {
    CleanupStoppedSources();

    ALuint sourceId;
    alGenSources(1, &sourceId);
    
    if (alGetError() != AL_NO_ERROR) {
        return 0;
    }
    
    return sourceId;
}

void AudioEngine::DestroyAudioSource(ALuint sourceId) {
    alDeleteSources(1, &sourceId);
}

void AudioEngine::CleanupStoppedSources() {
    for (auto it = m_ActiveSources.begin(); it != m_ActiveSources.end();) {
        ALint state;
        alGetSourcei(it->first, AL_SOURCE_STATE, &state);
        
        if (state == AL_STOPPED) {
            DestroyAudioSource(it->first);
            it = m_ActiveSources.erase(it);
        } else {
            ++it;
        }
    }
}

}