#include "Tweening.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <functional>

std::vector<Tweening::Tween> Tweening::tweens;

void Tweening::Move(const glm::dvec3& targetPosition, float duration, Modes mode) {
    
	auto tween = Tween();
    tween.object->GetTransform()->GetPosition() == tween.startPosition;
	tween.targetPosition = targetPosition;
	tween.duration = duration;
	tween.elapsedTime = 0.0f;
	tween.onComplete = nullptr;
	tween.mode = mode;

	tweens.push_back(tween);

}

void Tweening::MoveX(float targetX, float duration, Modes mode) {
    
}

void Tweening::MoveY(float targetY, float duration, Modes mode) {
   
}

void Tweening::MoveZ(float targetZ, float duration, Modes mode) {
    
}

void Tweening::Start() {
    
}

glm::dvec3 Tweening::CalculatePosition(const glm::dvec3& startPos, const glm::dvec3& targetPos, float t, Modes mode) {
    switch (mode) {
        case Modes::LINEAR:
            return glm::mix(startPos, targetPos, t);
        case Modes::EASE_IN:
            return glm::mix(startPos, targetPos, t * t);
        case Modes::EASE_OUT:
            return glm::mix(startPos, targetPos, 1.0f - (1.0f - t) * (1.0f - t));
        case Modes::EASE_IN_OUT:
            return glm::mix(startPos, targetPos, t < 0.5f ? 2.0f * t * t : 1.0f - pow(-2.0f * t + 2.0f, 2) / 2.0f);
        default:
            return glm::mix(startPos, targetPos, t);
    }
}
void Tweening::Update(float deltaTime) {
    
    for (auto& tween : tweens) {

        tween.elapsedTime += deltaTime;

        float t = glm::clamp(tween.elapsedTime / tween.duration, 0.0f, 1.0f);

        glm::dvec3 currentPosition = CalculatePosition(tween.startPosition, tween.targetPosition, t, tween.mode);

		tween.object->GetTransform()->SetPosition(currentPosition);

        if (tween.elapsedTime >= tween.duration && tween.onComplete != nullptr) {
            tween.onComplete();
        }
    }

    
    tweens.erase(std::remove_if(tweens.begin(), tweens.end(), [](const Tween& tween) {
        return tween.elapsedTime >= tween.duration;
        }), tweens.end());
}
