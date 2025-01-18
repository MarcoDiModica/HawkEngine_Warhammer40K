#include "SoundComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include <glm/glm.hpp>
#include <algorithm>

using namespace MyGameEngine;  // For AudioEngine and AudioAsset

SoundComponent::SoundComponent(GameObject* owner)
    : Component(owner)
    , m_SourceId(0)
    , m_Volume(1.0f)
    , m_IsMusic(false)
    , m_IsSpatial(false)
    , m_AutoPlay(false)
    , m_Loop(false)
{
    name = "SoundComponent";
    m_AudioEngine = std::make_shared<AudioEngine>();
    if (!m_AudioEngine->Initialize()) {
        enabled = false;
    }
}

SoundComponent::~SoundComponent() {
    Destroy();
}

void SoundComponent::Start() {
    if (!m_AudioPath.empty() && m_AutoPlay) {
        LoadAudio(m_AudioPath, m_IsMusic);
        Play(m_Loop);
    }
}

void SoundComponent::Update(float deltaTime) {
    if (!enabled || !m_AudioEngine || !m_AudioEngine->IsInitialized()) return;

    if (m_IsSpatial && m_SourceId != 0) {
        UpdatePosition();
    }
}

void SoundComponent::Destroy() {
    if (m_SourceId != 0) {
        Stop();
    }
    if (m_AudioEngine) {
        m_AudioEngine->Shutdown();
    }
}

std::unique_ptr<Component> SoundComponent::Clone(GameObject* new_owner) {
    auto clone = std::make_unique<SoundComponent>(*this);
    clone->owner = new_owner;
    clone->m_SourceId = 0; // Reset source ID as it needs a new one
    
    // Reload audio if there was any
    if (!m_AudioPath.empty()) {
        clone->LoadAudio(m_AudioPath, m_IsMusic);
        if (m_AutoPlay) {
            clone->Play(m_Loop);
        }
    }
    
    return clone;
}

bool SoundComponent::LoadAudio(const std::string& filePath, bool isMusic) {
    if (!m_AudioEngine || !m_AudioEngine->IsInitialized()) return false;

    m_AudioPath = filePath;
    m_IsMusic = isMusic;
    
    // Stop and clean up any existing audio
    if (m_SourceId != 0) {
        Stop();
    }

    m_AudioAsset = m_AudioEngine->LoadAudioAsset(filePath);
    return m_AudioAsset != nullptr;
}

void SoundComponent::Play(bool loop) {
    if (!enabled || !m_AudioEngine || !m_AudioAsset) return;

    m_Loop = loop;
    
    // If already playing, stop first
    if (m_SourceId != 0) {
        Stop();
    }

    m_SourceId = m_AudioEngine->PlaySound(m_AudioAsset, loop, m_IsMusic);
    
    if (m_SourceId != 0) {
        m_AudioEngine->SetVolume(m_SourceId, m_Volume);
        if (m_IsSpatial) {
            UpdatePosition();
        }
    }
}

void SoundComponent::Stop() {
    if (m_SourceId != 0) {
        m_AudioEngine->StopSound(m_SourceId);
        m_SourceId = 0;
    }
}

void SoundComponent::Pause() {
    if (m_SourceId != 0) {
        m_AudioEngine->PauseSound(m_SourceId);
    }
}

void SoundComponent::Resume() {
    if (m_SourceId != 0) {
        m_AudioEngine->ResumeSound(m_SourceId);
    }
}

void SoundComponent::SetVolume(float volume) {
    m_Volume = std::max(0.0f, std::min(volume, 1.0f));
    if (m_SourceId != 0) {
        m_AudioEngine->SetVolume(m_SourceId, m_Volume);
    }
}

bool SoundComponent::IsPlaying() const {
    if (m_SourceId != 0) {
        return m_AudioEngine->IsPlaying(m_SourceId);
    }
    return false;
}

void SoundComponent::UpdatePosition() {
    if (!owner) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    glm::dvec3 position = transform->GetPosition();
    m_AudioEngine->SetSourcePosition(m_SourceId, 
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        static_cast<float>(position.z));
}
