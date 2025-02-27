#include "AudioListener.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "SoundComponent.h"
#include "../MyGameEditor/Log.h"
#include <glm/glm.hpp>

AudioListener::AudioListener(GameObject* owner)
    : Component(owner)
{
    name = "AudioListener";
    LOG(LogType::LOG_INFO, "AudioListener created on %s", owner->GetName().c_str());
}

void AudioListener::Start() {
    UpdateListenerPosition();
}

void AudioListener::Update(float deltaTime) {
    if (!enabled) return;
    UpdateListenerPosition();
}

std::unique_ptr<Component> AudioListener::Clone(GameObject* new_owner) {
    auto clone = std::make_unique<AudioListener>(*this);
    clone->owner = new_owner;
    return clone;
}

void AudioListener::UpdateListenerPosition() {
    if (!owner) return;

    Transform_Component* transform = owner->GetTransform();
    if (!transform) return;

    auto& audioEngine = SoundComponent::GetSharedAudioEngine();
    if (!audioEngine || !audioEngine->IsInitialized()) {
        LOG(LogType::LOG_WARNING, "AudioListener on %s: Audio engine not initialized", owner->GetName().c_str());
        return;
    }

    glm::dvec3 position = transform->GetPosition();
    audioEngine->SetListenerPosition(
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        static_cast<float>(position.z)
    );
    
    // Debug log every few frames
    static int frameCount = 0;
    //if (++frameCount % 60 == 0) { // Log every 60 frames
    //    LOG(LogType::LOG_INFO, "AudioListener position updated on %s: (%.2f, %.2f, %.2f)", 
    //        owner->GetName().c_str(), 
    //        static_cast<float>(position.x),
    //        static_cast<float>(position.y),
    //        static_cast<float>(position.z));
    //}
} 