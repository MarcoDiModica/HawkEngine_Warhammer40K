#include "AudioAsset.h"
#include <iostream>

namespace MyGameEngine {

AudioAsset::AudioAsset()
    : m_Buffer(0)
    , m_Duration(0.0f)
    , m_SampleRate(0)
    , m_Channels(0)
    , m_IsValid(false)
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
        std::cout << "Failed to load audio file: " << filePath << std::endl;
        return false;
    }

    return ProcessAudioFile(audioFile);
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
        std::cout << "OpenAL error while loading audio: " << error << std::endl;
        return false;
    }

    m_IsValid = true;
    return true;
}

} 