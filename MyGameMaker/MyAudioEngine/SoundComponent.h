#pragma once

#include "../MyGameEngine/Component.h"
#include "AudioEngine.h"
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>

// Forward declarations
class GameObject;
class Component;

class SoundComponent : public Component {
public:
    explicit SoundComponent(GameObject* owner);
    ~SoundComponent() override;

    static void InitSharedAudioEngine();
    static void ShutdownSharedAudioEngine();
    static std::shared_ptr<MyGameEngine::AudioEngine>& GetSharedAudioEngine() { return s_SharedAudioEngine; }

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;

    ComponentType GetType() const override { return ComponentType::AUDIO; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

    // Audio control methods
    bool LoadAudio(const std::string& filePath, bool isMusic = false);
    void Play(bool loop = false);
    void Stop();
    void Pause() const;
    void Resume() const;

    // Audio properties
    void SetVolume(float volume);  // 0.0f to 1.0f
    float GetVolume() const { return m_Volume; }
    
    void SetSpatial(bool spatial) { m_IsSpatial = spatial; }
    bool IsSpatial() const { return m_IsSpatial; }
    
    bool IsPlaying() const;
    bool IsMusic() const { return m_IsMusic; }

    // Additional getters/setters for inspector
    const std::string& GetAudioPath() const { return m_AudioPath; }
    void SetAutoPlay(bool autoPlay) { m_AutoPlay = autoPlay; }
    bool GetAutoPlay() const { return m_AutoPlay; }
    void SetLoop(bool loop) { m_Loop = loop; }
    bool GetLoop() const { return m_Loop; }

	MonoObject* CsharpReference = nullptr;
	MonoObject* GetSharp() override;

    void DetachFromEngine();

private:
    void UpdatePosition();

protected:
    friend class SceneSerializer;

    YAML::Node encode() override {
        YAML::Node node = Component::encode();
        
        node["audio_path"] = m_AudioPath;
        node["volume"] = m_Volume;
        node["is_music"] = m_IsMusic;
        node["is_spatial"] = m_IsSpatial;
        node["auto_play"] = m_AutoPlay;
        node["loop"] = m_Loop;
        
        return node;
    }

    bool decode(const YAML::Node& node) override {
        if (!Component::decode(node)) return false;
        
        if (!node["audio_path"] || !node["volume"] || !node["is_music"] || 
            !node["is_spatial"] || !node["auto_play"] || !node["loop"])
            return false;

        m_AudioPath = node["audio_path"].as<std::string>();
        m_Volume = node["volume"].as<float>();
        m_IsMusic = node["is_music"].as<bool>();
        m_IsSpatial = node["is_spatial"].as<bool>();
        m_AutoPlay = node["auto_play"].as<bool>();
        m_Loop = node["loop"].as<bool>();

        if (!m_AudioPath.empty()) {
            LoadAudio(m_AudioPath, m_IsMusic);
            if (m_AutoPlay) {
                Play(m_Loop);
            }
        }

        return true;
    }

private:
    static std::shared_ptr<MyGameEngine::AudioEngine> s_SharedAudioEngine;
    std::shared_ptr<MyGameEngine::AudioAsset> m_AudioAsset;
    ALuint m_SourceId;
    
    std::string m_AudioPath;
    float m_Volume;
    bool m_IsMusic;
    bool m_IsSpatial;
    bool m_AutoPlay;
    bool m_Loop;
};
