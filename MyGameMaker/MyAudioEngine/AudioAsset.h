#pragma once

#include <AL/al.h>
#include <string>
#include <vector>
#include <memory>
#include "AudioFile.h"

namespace MyGameEngine {

class AudioAsset {
public:
    AudioAsset();
    ~AudioAsset();

    // Load audio from file
    bool LoadFromFile(const std::string& filePath);
    
    // Get the OpenAL buffer ID
    ALuint GetBuffer() const { return m_Buffer; }
    
    // Get audio properties
    float GetDuration() const { return m_Duration; }
    int GetSampleRate() const { return m_SampleRate; }
    int GetChannels() const { return m_Channels; }
    
    // Check if the asset is valid
    bool IsValid() const { return m_IsValid; }

private:
    bool ProcessAudioFile(AudioFile<float>& audioFile);
    
    ALuint m_Buffer;
    float m_Duration;
    int m_SampleRate;
    int m_Channels;
    bool m_IsValid;
};

} 