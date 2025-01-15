#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <memory>

namespace MyGameEngine {

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    bool Initialize();
    void Shutdown();

    // Returns true if the audio engine is properly initialized
    bool IsInitialized() const { return m_Initialized; }

private:
    ALCdevice* m_Device;
    ALCcontext* m_Context;
    bool m_Initialized;
};

}