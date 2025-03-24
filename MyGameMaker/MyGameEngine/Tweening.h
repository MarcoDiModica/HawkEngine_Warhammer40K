#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>

class GameObject;

class Tweening
{
public:
    
    
    static void Move(const glm::vec3& targetPosition, float duration, bool snapping);
    static void MoveX(float targetX, float duration, bool snapping);
    static void MoveY(float targetY, float duration, bool snapping);
    static void MoveZ(float targetZ, float duration, bool snapping);

    static void Rotate(const glm::vec3& targetRotation, float duration,bool rotate);

    static void Scale(const glm::vec3& targetScale, float duration);
	static void ScaleX(float targetX, float duration);
	static void ScaleY(float targetY, float duration);
	static void ScaleZ(float targetZ, float duration);
	
	static void Start();
    static void Update(float deltaTime);

private:
    struct Tween
    {
        GameObject* object;
        glm::vec3 startPosition;
        glm::vec3 targetPosition;
        glm::vec3 startRotation;
        glm::vec3 targetRotation;
        glm::vec3 startScale;
        glm::vec3 targetScale;
        float duration;
        float elapsedTime;
        std::function<void()> onComplete;
    };

    static std::vector<Tween> tweens;
};
