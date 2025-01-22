#include "AudioAsset.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "../MyGameEditor/Log.h"

namespace MyGameEngine {

uint32_t AudioAsset::s_NextAssetID = 1;

AudioAsset::AudioAsset()
    : m_Buffer(0)
    , m_Duration(0.0f)
    , m_SampleRate(0)
    , m_Channels(0)
    , m_IsValid(false)
    , m_AssetID(s_NextAssetID++)
{
    alGenBuffers(1, &m_Buffer);
}

AudioAsset::~AudioAsset() {
    if (m_Buffer) {
        alDeleteBuffers(1, &m_Buffer);
        m_Buffer = 0;
    }
}

bool AudioAsset::LoadFromFile(const std::string& filePath) {
    AudioFile<float> audioFile;
    
    if (!audioFile.load(filePath)) {
        LOG(LogType::LOG_ERROR, "Failed to load audio file: %s", filePath.c_str());
        return false;
    }

    m_SourcePath = filePath;
    m_LibraryPath = "Library/Audio/" + std::filesystem::path(filePath).filename().string();

    return ProcessAudioFile(audioFile);
}

void AudioAsset::SaveBinary(const std::string& filename) const {
    std::string fullPath = "Library/Audio/" + filename;
    
    // Create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(fullPath).parent_path();
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        LOG(LogType::LOG_ERROR, "Failed to save audio asset: %s", fullPath.c_str());
        return;
    }

    // Write header
    file.write(reinterpret_cast<const char*>(&m_AssetID), sizeof(m_AssetID));
    file.write(reinterpret_cast<const char*>(&m_Duration), sizeof(m_Duration));
    file.write(reinterpret_cast<const char*>(&m_SampleRate), sizeof(m_SampleRate));
    file.write(reinterpret_cast<const char*>(&m_Channels), sizeof(m_Channels));

    // Write paths
    size_t sourcePathLength = m_SourcePath.length();
    file.write(reinterpret_cast<const char*>(&sourcePathLength), sizeof(sourcePathLength));
    file.write(m_SourcePath.c_str(), sourcePathLength);

    size_t libraryPathLength = m_LibraryPath.length();
    file.write(reinterpret_cast<const char*>(&libraryPathLength), sizeof(libraryPathLength));
    file.write(m_LibraryPath.c_str(), libraryPathLength);

    LOG(LogType::LOG_OK, "Saved audio asset: %s", fullPath.c_str());
}

std::shared_ptr<AudioAsset> AudioAsset::LoadBinary(const std::string& filename) {
    std::string fullPath = "Library/Audio/" + filename;
    
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        LOG(LogType::LOG_ERROR, "Failed to load audio asset: %s", fullPath.c_str());
        return nullptr;
    }

    auto asset = std::make_shared<AudioAsset>();

    // Read header
    file.read(reinterpret_cast<char*>(&asset->m_AssetID), sizeof(asset->m_AssetID));
    file.read(reinterpret_cast<char*>(&asset->m_Duration), sizeof(asset->m_Duration));
    file.read(reinterpret_cast<char*>(&asset->m_SampleRate), sizeof(asset->m_SampleRate));
    file.read(reinterpret_cast<char*>(&asset->m_Channels), sizeof(asset->m_Channels));

    // Read paths
    size_t sourcePathLength;
    file.read(reinterpret_cast<char*>(&sourcePathLength), sizeof(sourcePathLength));
    asset->m_SourcePath.resize(sourcePathLength);
    file.read(&asset->m_SourcePath[0], sourcePathLength);

    size_t libraryPathLength;
    file.read(reinterpret_cast<char*>(&libraryPathLength), sizeof(libraryPathLength));
    asset->m_LibraryPath.resize(libraryPathLength);
    file.read(&asset->m_LibraryPath[0], libraryPathLength);

    // Load the actual audio data
    if (!asset->LoadFromFile(asset->m_SourcePath)) {
        LOG(LogType::LOG_ERROR, "Failed to load audio data for asset: %s", fullPath.c_str());
        return nullptr;
    }

    LOG(LogType::LOG_OK, "Loaded audio asset: %s", fullPath.c_str());
    return asset;
}

bool AudioAsset::ProcessAudioFile(AudioFile<float>& audioFile) {
    m_SampleRate = audioFile.getSampleRate();
    m_Channels = audioFile.getNumChannels();
    int numSamples = audioFile.getNumSamplesPerChannel();
    m_Duration = static_cast<float>(numSamples) / m_SampleRate;

    // Convert float audio data to 16-bit PCM
    std::vector<int16_t> samples(numSamples * m_Channels);
    for (int channel = 0; channel < m_Channels; channel++) {
        for (int i = 0; i < numSamples; i++) {
            float sample = audioFile.samples[channel][i];
            // Convert float (-1.0 to 1.0) to int16_t
            int16_t pcmSample = static_cast<int16_t>(sample * 32767.0f);
            samples[i * m_Channels + channel] = pcmSample;
        }
    }

    // Determine format based on number of channels
    ALenum format = (m_Channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    // Upload to OpenAL buffer
    alBufferData(m_Buffer, format, samples.data(), 
                static_cast<ALsizei>(samples.size() * sizeof(int16_t)), 
                m_SampleRate);

    // Check for errors
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        LOG(LogType::LOG_ERROR, "OpenAL error while loading audio: %d", error);
        return false;
    }

    m_IsValid = true;
    return true;
}

} 