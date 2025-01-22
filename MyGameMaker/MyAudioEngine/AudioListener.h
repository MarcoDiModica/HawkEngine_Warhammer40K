#pragma once

#include "../MyGameEngine/Component.h"
#include "AudioEngine.h"
#include <memory>

class GameObject;

class AudioListener : public Component {
public:
    explicit AudioListener(GameObject* owner);
    ~AudioListener() override = default;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override {}

    ComponentType GetType() const override { return ComponentType::AUDIO_LISTENER; }
    std::unique_ptr<Component> Clone(GameObject* new_owner) override;

private:
    void UpdateListenerPosition();
}; 