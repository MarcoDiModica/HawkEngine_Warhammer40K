#include "AudioEngine.h"
#include <iostream>

namespace MyGameEngine {

AudioEngine::AudioEngine()
    : m_Device(nullptr)
    , m_Context(nullptr)
    , m_Initialized(false)
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

    m_Initialized = true;
    std::cout << "Audio Engine initialized successfully!" << std::endl;
    
    // Print OpenAL version and vendor info
    std::cout << "OpenAL Vendor: " << alGetString(AL_VENDOR) << std::endl;
    std::cout << "OpenAL Version: " << alGetString(AL_VERSION) << std::endl;
    std::cout << "OpenAL Renderer: " << alGetString(AL_RENDERER) << std::endl;

    return true;
}

void AudioEngine::Shutdown() {
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