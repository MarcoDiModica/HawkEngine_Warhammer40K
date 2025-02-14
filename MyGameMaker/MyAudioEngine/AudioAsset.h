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

    // Asset management methods
    bool LoadFromFile(const std::string& filePath);
    void SaveBinary(const std::string& filename) const;
    static std::shared_ptr<AudioAsset> LoadBinary(const std::string& filename);
    
    // Getters
    ALuint GetBuffer() const { return m_Buffer; }
    float GetDuration() const { return m_Duration; }
    int GetSampleRate() const { return m_SampleRate; }
    int GetChannels() const { return m_Channels; }
    bool IsValid() const { return m_IsValid; }
    
    // Asset path management
    std::string GetSourcePath() const { return m_SourcePath; }
    std::string GetLibraryPath() const { return m_LibraryPath; }
    uint32_t GetID() const { return m_AssetID; }

private:
    bool ProcessAudioFile(AudioFile<float>& audioFile);
    
    ALuint m_Buffer;
    float m_Duration;
    int m_SampleRate;
    int m_Channels;
    bool m_IsValid;

    // Asset management data
    std::string m_SourcePath;    // Original file path in Assets/
    std::string m_LibraryPath;   // Processed file path in Library/
    uint32_t m_AssetID;          // Unique asset identifier
    static uint32_t s_NextAssetID;
};

} 