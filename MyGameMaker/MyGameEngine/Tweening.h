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
	LINEAR,
	
};

enum class StepType {
	TWEEN,
	DELAY,
	TESTCALLBACK
};

class Tweening
{
public:
	typedef int TweenHandle;

	static TweenHandle Move(GameObject* object, const glm::dvec3& targetPosition, float duration, Modes mode = Modes::LINEAR);
	static void MoveX(GameObject* object, float targetX, float duration, Modes mode = Modes::LINEAR);
	static void MoveY(GameObject* object, float targetY, float duration, Modes mode = Modes::LINEAR);
	static void MoveZ(GameObject* object, float targetZ, float duration, Modes mode = Modes::LINEAR);

	static TweenHandle Rotate(GameObject* object, const glm::dvec3& targetRotation, float duration, Modes mode = Modes::LINEAR);
	static void RotateX(GameObject* object, float targetX, float duration, Modes mode = Modes::LINEAR);
	static void RotateY(GameObject* object, float targetY, float duration, Modes mode = Modes::LINEAR);
	static void RotateZ(GameObject* object, float targetZ, float duration, Modes mode = Modes::LINEAR);

	static void Scale(GameObject* object, const glm::dvec3& targetScale, float duration, Modes mode = Modes::LINEAR);
	static void ScaleX(GameObject* object, float targetX, float duration, Modes mode = Modes::LINEAR);
	static void ScaleY(GameObject* object, float targetY, float duration, Modes mode = Modes::LINEAR);
	static void ScaleZ(GameObject* object, float targetZ, float duration, Modes mode = Modes::LINEAR);

	static void UIMove(GameObject* object, const glm::dvec3& targetPosition, float duration, Modes mode = Modes::LINEAR);
	static void UIMoveX(GameObject* object, float targetX, float duration, Modes mode = Modes::LINEAR);
	static void UIMoveY(GameObject* object, float targetY, float duration, Modes mode = Modes::LINEAR);
	static void UIMoveZ(GameObject* object, float targetZ, float duration, Modes mode = Modes::LINEAR);
				
	static void UIRotate(GameObject* object, const glm::dvec3& targetRotation, float duration, Modes mode = Modes::LINEAR);
	static void UIRotateX(GameObject* object, float targetX, float duration, Modes mode = Modes::LINEAR);
	static void UIRotateY(GameObject* object, float targetY, float duration, Modes mode = Modes::LINEAR);
	static void UIRotateZ(GameObject* object, float targetZ, float duration, Modes mode = Modes::LINEAR);
				
	static void UIScale(GameObject* object, const glm::dvec3& targetScale, float duration, Modes mode = Modes::LINEAR);
	static void UIScaleX(GameObject* object, float targetX, float duration, Modes mode = Modes::LINEAR);
	static void UIScaleY(GameObject* object, float targetY, float duration, Modes mode = Modes::LINEAR);
	static void UIScaleZ(GameObject* object, float targetZ, float duration, Modes mode = Modes::LINEAR);

	static TweenHandle TweenValue(float* value, float start, float target, float duration, Modes mode = Modes::LINEAR);
	static TweenHandle TweenColor(glm::vec4* color, const glm::vec4& startColor, const glm::vec4& targetColor, float duration, Modes mode = Modes::LINEAR);
	static TweenHandle TweenVec3(glm::vec3* vec, const glm::vec3& start, const glm::vec3& target, float duration, Modes mode = Modes::LINEAR);

	static void Cancel(TweenHandle handle);
	static void CancelAll(GameObject* object);
	static void SetOnComplete(TweenHandle handle, std::function<void()> callback);
	static void SetOnUpdate(TweenHandle handle, std::function<void(float)> callback);

	static float CalculateT(float normalizedTime, Modes mode);
	static glm::dvec3 CalculatePosition(const glm::dvec3& startPos, const glm::dvec3& targetPos, float t, Modes mode);
	static glm::dvec3 CalculateRotation(const glm::dvec3& startRot, const glm::dvec3& targetRot, float t, Modes mode);
	static glm::dvec3 CalculateScale(const glm::dvec3& startScale, const glm::dvec3& targetScale, float t, Modes mode);
	static float CalculateFloat(float start, float target, float t, Modes mode);
	static glm::vec4 CalculateColor(const glm::vec4& startColor, const glm::vec4& targetColor, float t, Modes mode);
	static glm::vec3 CalculateVec3(const glm::vec3& start, const glm::vec3& target, float t, Modes mode);

	static void CleanAllTweens();

	class Sequence 
	{
	public:
		Sequence();

		Sequence& Append(std::function<Tweening::TweenHandle()> tweenCreator);
		Sequence& AppendDelay(float duration);
		Sequence& AppendCallback(std::function<void()> callback);

		void Play();
		void Stop();

	private:
		
		struct Step {
			std::function<Tweening::TweenHandle()> tweenCreator;
			float duration;
			StepType type;
		};

		std::vector<Step> steps;
		size_t currentIndex;
		bool isPlaying;
		Tweening::TweenHandle currentTweenHandle = 0;

		void PlayCurrentStep();
	};

	static Sequence CreateSequence();
	static void Update(float deltaTime);

private:
	enum class TweenType {
		POSITION,
		POSITION_X,
		POSITION_Y,
		POSITION_Z,
		ROTATION,
		ROTATION_X,
		ROTATION_Y,
		ROTATION_Z,
		SCALE,
		SCALE_X,
		SCALE_Y,
		SCALE_Z,
		UIPOSITION,
		UIPOSITION_X,
		UIPOSITION_Y,
		UIPOSITION_Z,
		UIROTATION,
		UIROTATION_X,
		UIROTATION_Y,
		UIROTATION_Z,
		UISCALE,
		UISCALE_X,
		UISCALE_Y,
		UISCALE_Z,
		FLOAT_VALUE,
		COLOR,
		VEC3
	};

	struct Tween
	{
		GameObject* object = nullptr;
		TweenType tweenType;
		TweenHandle handle = 0;

		glm::dvec3 startPosition;
		glm::dvec3 targetPosition;
		glm::dvec3 startRotation;
		glm::dvec3 targetRotation;
		glm::dvec3 startScale;
		glm::dvec3 targetScale;

		float* floatPtr = nullptr;
		float startFloat = 0.0f;
		float targetFloat = 0.0f;

		glm::vec4* colorPtr = nullptr;
		glm::vec4 startColor;
		glm::vec4 targetColor;

		glm::vec3* vec3Ptr = nullptr;
		glm::vec3 startVec3;
		glm::vec3 targetVec3;

		Modes mode;
		float duration;
		float elapsedTime;

		// Callbacks
		std::function<void()> onComplete;
		std::function<void(float)> onUpdate;
	};

	static std::vector<Tween> tweens;
	static Tween CreateTween(GameObject* object, float duration, Modes mode);
};

