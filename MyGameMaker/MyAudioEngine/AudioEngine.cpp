#include "AudioEngine.h"
#include "AudioAssetProcessor.h"
#include <iostream>
#include <filesystem>

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
    AudioAssetProcessor::ClearCache(); //crahsea en esta funcion
    
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
    std::string libraryPath = "Library/Audio/" + std::filesystem::path(filePath).filename().string();
    auto asset = AudioAssetProcessor::GetCachedAsset(libraryPath);
    if (asset) {
        return asset;
    }

    // Process the audio file if needed
    if (!std::filesystem::exists(libraryPath)) {
        if (!AudioAssetProcessor::ProcessAudioFile(filePath, libraryPath)) {
            return nullptr;
        }
    }

    // Load the processed audio
    return AudioAssetProcessor::LoadProcessedAudio(libraryPath);
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

ALuint AudioEngine::TestPlayMusic(const std::string& filePath, bool autoLoop) {
    if (!m_Initialized) {
        std::cout << "Audio Engine not initialized!" << std::endl;
        return 0;
    }

    std::cout << "Testing music playback: " << filePath << std::endl;
    auto asset = LoadAudioAsset(filePath);
    if (!asset) {
        std::cout << "Failed to load music file!" << std::endl;
        return 0;
    }

    ALuint sourceId = PlaySound(asset, autoLoop, true);
    if (sourceId == 0) {
        std::cout << "Failed to play music!" << std::endl;
        return 0;
    }

    std::cout << "Music playing with source ID: " << sourceId << std::endl;
    std::cout << "Duration: " << asset->GetDuration() << " seconds" << std::endl;
    std::cout << "Channels: " << asset->GetChannels() << std::endl;
    std::cout << "Sample Rate: " << asset->GetSampleRate() << " Hz" << std::endl;
    
    return sourceId;
}

ALuint AudioEngine::TestPlaySoundEffect(const std::string& filePath, float x, float y, float z) {
    if (!m_Initialized) {
        std::cout << "Audio Engine not initialized!" << std::endl;
        return 0;
    }

    std::cout << "Testing sound effect: " << filePath << std::endl;
    std::cout << "Position: (" << x << ", " << y << ", " << z << ")" << std::endl;
    
    auto asset = LoadAudioAsset(filePath);
    if (!asset) {
        std::cout << "Failed to load sound effect file!" << std::endl;
        return 0;
    }

    ALuint sourceId = PlaySound(asset, false, false);
    if (sourceId == 0) {
        std::cout << "Failed to play sound effect!" << std::endl;
        return 0;
    }

    SetSourcePosition(sourceId, x, y, z);
    std::cout << "Sound effect playing with source ID: " << sourceId << std::endl;
    
    return sourceId;
}

void AudioEngine::TestSetListenerPosition(float x, float y, float z) {
    if (!m_Initialized) {
        std::cout << "Audio Engine not initialized!" << std::endl;
        return;
    }

    std::cout << "Setting listener position to: (" << x << ", " << y << ", " << z << ")" << std::endl;
    SetListenerPosition(x, y, z);
}

void AudioEngine::TestPlayAllSupportedFormats() {
    if (!m_Initialized) {
        std::cout << "Audio Engine not initialized!" << std::endl;
        return;
    }

    std::cout << "\n=== Testing Supported Audio Formats ===" << std::endl;
    
    // Test WAV format
    std::cout << "\nTesting WAV format:" << std::endl;
    if (std::filesystem::exists("EngineAssets/Sounds/effect.wav")) {
        TestPlaySoundEffect("EngineAssets/Sounds/effect.wav");
    } else {
        std::cout << "WAV test file not found (EngineAssets/Sounds/effect.wav)" << std::endl;
        std::cout << "Please create the EngineAssets/Sounds directory and add test audio files." << std::endl;
    }

    // Add more format tests as needed
}

void AudioEngine::PrintAudioDeviceInfo() const {
    if (!m_Initialized) {
        std::cout << "Audio Engine not initialized!" << std::endl;
        return;
    }

    std::cout << "\n=== Audio Device Information ===" << std::endl;
    std::cout << "Default Device: " << alcGetString(m_Device, ALC_DEVICE_SPECIFIER) << std::endl;
    
    // Get and display audio device capabilities
    ALCint major, minor;
    alcGetIntegerv(m_Device, ALC_MAJOR_VERSION, 1, &major);
    alcGetIntegerv(m_Device, ALC_MINOR_VERSION, 1, &minor);
    std::cout << "OpenAL Version: " << major << "." << minor << std::endl;

    // Print max sources information
    ALCint maxMono, maxStereo;
    alcGetIntegerv(m_Device, ALC_MONO_SOURCES, 1, &maxMono);
    alcGetIntegerv(m_Device, ALC_STEREO_SOURCES, 1, &maxStereo);
    std::cout << "Maximum Mono Sources: " << maxMono << std::endl;
    std::cout << "Maximum Stereo Sources: " << maxStereo << std::endl;
}

void AudioEngine::PrintActiveSourcesInfo() const {
    if (!m_Initialized) {
        std::cout << "Audio Engine not initialized!" << std::endl;
        return;
    }

    std::cout << "\n=== Active Audio Sources ===" << std::endl;
    std::cout << "Total active sources: " << m_ActiveSources.size() << std::endl;

    for (const auto& [sourceId, source] : m_ActiveSources) {
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        
        float x, y, z;
        alGetSource3f(sourceId, AL_POSITION, &x, &y, &z);
        
        float gain;
        alGetSourcef(sourceId, AL_GAIN, &gain);

        std::cout << "\nSource ID: " << sourceId << std::endl;
        std::cout << "Type: " << (source.isMusic ? "Music" : "Sound Effect") << std::endl;
        std::cout << "State: ";
        switch (state) {
            case AL_INITIAL: std::cout << "Initial"; break;
            case AL_PLAYING: std::cout << "Playing"; break;
            case AL_PAUSED: std::cout << "Paused"; break;
            case AL_STOPPED: std::cout << "Stopped"; break;
            default: std::cout << "Unknown";
        }
        std::cout << std::endl;
        std::cout << "Position: (" << x << ", " << y << ", " << z << ")" << std::endl;
        std::cout << "Volume: " << gain << std::endl;
        std::cout << "Looping: " << (source.isLooping ? "Yes" : "No") << std::endl;
    }
}

}