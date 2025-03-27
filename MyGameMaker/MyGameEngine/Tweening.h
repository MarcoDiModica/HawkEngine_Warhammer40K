#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include <vector>

class GameObject;

enum class Modes {
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    LINEAR
};

class Tweening
{
public:
    static void Move(const glm::dvec3& targetPosition, float duration, Modes mode);
    static void MoveX(float targetX, float duration, Modes mode);
    static void MoveY(float targetY, float duration, Modes mode);
    static void MoveZ(float targetZ, float duration, Modes mode);

    static void Rotate(const glm::dvec3& targetRotation, float duration, bool rotate);

    static void Scale(const glm::dvec3& targetScale, float duration);
    static void ScaleX(float targetX, float duration);
    static void ScaleY(float targetY, float duration);
    static void ScaleZ(float targetZ, float duration);

    static glm::dvec3 CalculatePosition(const glm::dvec3& startPos, const glm::dvec3& targetPos, float t, Modes mode);

    static void Start();
    static void Update(float deltaTime);

private:
    struct Tween
    {
        GameObject* object;
        glm::dvec3 startPosition;
        glm::dvec3 targetPosition;
        glm::dvec3 startRotation;
        glm::dvec3 targetRotation;
        glm::dvec3 startScale;
        glm::dvec3 targetScale;
		Modes mode;
        float duration;
        float elapsedTime;
        std::function<void()> onComplete;
    };

    static std::vector<Tween> tweens;
};