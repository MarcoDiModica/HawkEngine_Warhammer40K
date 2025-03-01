#include "SoundComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEditor/Log.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include "AudioAssetProcessor.h"
#include <filesystem>
#include "MyScriptingEngine/MonoManager.h"
#include "mono/metadata/debug-helpers.h"

using namespace MyGameEngine;  // For AudioEngine and AudioAsset

// Initialize static member
std::shared_ptr<AudioEngine> SoundComponent::s_SharedAudioEngine;

void SoundComponent::InitSharedAudioEngine() {
    if (!s_SharedAudioEngine) {
        s_SharedAudioEngine = std::make_shared<AudioEngine>();
        if (!s_SharedAudioEngine->Initialize()) {
            LOG(LogType::LOG_ERROR, "Failed to initialize shared AudioEngine");
        }
    }
}

void SoundComponent::ShutdownSharedAudioEngine() {
    if (s_SharedAudioEngine) {
        s_SharedAudioEngine->Shutdown();
        s_SharedAudioEngine.reset();
    }
}

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
    if (!s_SharedAudioEngine) {
        InitSharedAudioEngine();
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
    if (!enabled || !s_SharedAudioEngine || !s_SharedAudioEngine->IsInitialized()) return;

    if (m_IsSpatial && m_SourceId != 0) {
        UpdatePosition();
    }
}

void SoundComponent::Destroy() {
    if (m_SourceId != 0) {
        Stop();
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
    if (!s_SharedAudioEngine || !s_SharedAudioEngine->IsInitialized()) return false;

    m_AudioPath = filePath;
    m_IsMusic = isMusic;
    
    // Stop and clean up any existing audio
    if (m_SourceId != 0) {
        Stop();
    }

    // Process the audio file if needed
    std::string libraryPath = "Library/Audio/" + std::filesystem::path(filePath).filename().string();
    if (!std::filesystem::exists(libraryPath)) {
        if (!AudioAssetProcessor::ProcessAudioFile(filePath, libraryPath)) {
            return false;
        }
    }

    // Load the processed audio
    m_AudioAsset = AudioAssetProcessor::LoadProcessedAudio(libraryPath);
    return m_AudioAsset != nullptr;
}

void SoundComponent::Play(bool loop) {
    if (!enabled || !s_SharedAudioEngine || !m_AudioAsset) return;

    m_Loop = loop;
    
    // If already playing, stop first
    if (m_SourceId != 0) {
        Stop();
    }

    m_SourceId = s_SharedAudioEngine->PlaySound(m_AudioAsset, loop, m_IsMusic);
    
    if (m_SourceId != 0) {
        s_SharedAudioEngine->SetVolume(m_SourceId, m_Volume);
        if (m_IsSpatial) {
            UpdatePosition();
        }
    }
}

void SoundComponent::Stop() {
    if (m_SourceId != 0 && s_SharedAudioEngine) {
        s_SharedAudioEngine->StopSound(m_SourceId);
        m_SourceId = 0;
    }
}

void SoundComponent::Pause() {
    if (m_SourceId != 0 && s_SharedAudioEngine) {
        s_SharedAudioEngine->PauseSound(m_SourceId);
    }
}

void SoundComponent::Resume() {
    if (m_SourceId != 0 && s_SharedAudioEngine) {
        s_SharedAudioEngine->ResumeSound(m_SourceId);
    }
}

void SoundComponent::SetVolume(float volume) {
    m_Volume = std::max(0.0f, std::min(volume, 1.0f));
    if (m_SourceId != 0 && s_SharedAudioEngine) {
        s_SharedAudioEngine->SetVolume(m_SourceId, m_Volume);
    }
}

bool SoundComponent::IsPlaying() const {
    if (m_SourceId != 0 && s_SharedAudioEngine) {
        return s_SharedAudioEngine->IsPlaying(m_SourceId);
    }
    return false;
}

MonoObject* SoundComponent::GetSharp()
{
	if (CsharpReference) {
		return CsharpReference;
	}

	MonoClass* klass = MonoManager::GetInstance().GetClass("HawkEngine", "Audio");
	if (!klass) {
		return nullptr;
	}

	MonoObject* monoObject = mono_object_new(MonoManager::GetInstance().GetDomain(), klass);
	if (!monoObject) {
		return nullptr;
	}

	MonoMethodDesc* constructorDesc = mono_method_desc_new("HawkEngine.Audio:.ctor(uintptr,HawkEngine.GameObject)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, klass);
	if (!method)
	{
		return nullptr;
	}

	uintptr_t componentPtr = reinterpret_cast<uintptr_t>(this);
	MonoObject* ownerGo = owner->GetSharp();
	if (!ownerGo)
	{
		return nullptr;
	}

	void* args[2]{};
	args[0] = &componentPtr;
	args[1] = ownerGo;

	mono_runtime_invoke(method, monoObject, args, nullptr);

	CsharpReference = monoObject;
	return CsharpReference;
}

void SoundComponent::UpdatePosition() {
    if (!owner || !s_SharedAudioEngine) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    glm::dvec3 position = transform->GetPosition();
    s_SharedAudioEngine->SetSourcePosition(m_SourceId, 
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        static_cast<float>(position.z));
}
