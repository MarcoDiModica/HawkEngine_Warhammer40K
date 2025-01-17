#pragma once
#include "../MyAudioEngine/AudioEngine.h"
#include <memory>
#include <string>

namespace MyGameEngine {

class UIAudioTest {
public:
    UIAudioTest();
    ~UIAudioTest() = default;

    void Initialize();
    void OnImGui();

private:
    void DrawAudioDeviceInfo();
    void DrawMusicControls();
    void DrawSoundEffectControls();
    void Draw3DAudioControls();
    
    std::shared_ptr<AudioEngine> m_AudioEngine;
    
    // Test state
    ALuint m_CurrentMusicSource;
    ALuint m_CurrentSFXSource;
    float m_ListenerPosition[3];
    float m_SFXPosition[3];
    float m_Volume;
    float m_MasterVolume;
    char m_MusicPath[256];
    char m_SFXPath[256];
    bool m_ShowWindow;
};

} 