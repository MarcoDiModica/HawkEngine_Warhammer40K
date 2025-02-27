#ifndef __UI_AUDIO_TEST_H__
#define __UI_AUDIO_TEST_H__
#pragma once

#include <string>
#include <memory>

#include "UIElement.h"
#include "../MyAudioEngine/AudioEngine.h"

class UIAudioTest : public UIElement
{
public:
    UIAudioTest(UIType type, std::string name);
    ~UIAudioTest();

    bool Draw() override;

private:
    std::shared_ptr<MyGameEngine::AudioEngine> m_AudioEngine;
    char m_MusicPath[256] = "EngineAssets/Sounds/music.wav";
    char m_SoundEffectPath[256] = "EngineAssets/Sounds/effect.wav";
    float m_Position[3] = {0.0f, 0.0f, 0.0f};
    float m_ListenerPosition[3] = {0.0f, 0.0f, 0.0f};
    float m_Volume = 1.0f;
    float m_MasterVolume = 1.0f;
    ALuint m_CurrentMusicSource = 0;
    ALuint m_CurrentEffectSource = 0;
};

#endif // !__UI_AUDIO_TEST_H__ 