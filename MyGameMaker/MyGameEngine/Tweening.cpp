#include "Tweening.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <functional>
#include <algorithm>
#include "MyUIEngine/UITransformComponent.h"

std::vector<Tweening::Tween> Tweening::tweens;

Tweening::Tween Tweening::CreateTween(GameObject* object, float duration, Modes mode) {
	Tween tween;
	tween.object = object;
	tween.duration = duration;
	tween.elapsedTime = 0.0f;
	tween.mode = mode;
	tween.onComplete = nullptr;
	tween.onUpdate = nullptr;
	return tween;
}

void Tweening::Move(GameObject* object, const glm::dvec3& targetPosition, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	tween.startPosition = object->GetTransform()->GetPosition();
	tween.targetPosition = targetPosition;
	tween.tweenType = TweenType::POSITION;

	tweens.push_back(tween);
}

void Tweening::UIMove(GameObject* object, const glm::dvec3& targetPosition, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startPosition = uiTransform->GetPosition();
		tween.targetPosition = targetPosition;
		tween.tweenType = TweenType::UIPOSITION;
		tweens.push_back(tween);
	}
}

void Tweening::MoveX(GameObject* object, float targetX, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentPos = object->GetTransform()->GetPosition();
	tween.startPosition = currentPos;
	tween.targetPosition = glm::dvec3(targetX, currentPos.y, currentPos.z);
	tween.tweenType = TweenType::POSITION_X;

	tweens.push_back(tween);
}

void Tweening::UIMoveX(GameObject* object, float targetX, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startPosition = uiTransform->GetPosition();
		tween.targetPosition = glm::dvec3(targetX, uiTransform->GetPosition().y, uiTransform->GetPosition().z);
		tween.tweenType = TweenType::UIPOSITION_X;
		tweens.push_back(tween);
	}
}

void Tweening::MoveY(GameObject* object, float targetY, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentPos = object->GetTransform()->GetPosition();
	tween.startPosition = currentPos;
	tween.targetPosition = glm::dvec3(currentPos.x, targetY, currentPos.z);
	tween.tweenType = TweenType::POSITION_Y;

	tweens.push_back(tween);
}

void Tweening::UIMoveY(GameObject* object, float targetY, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startPosition = uiTransform->GetPosition();
		tween.targetPosition = glm::dvec3(uiTransform->GetPosition().x, targetY, uiTransform->GetPosition().z);
		tween.tweenType = TweenType::UIPOSITION_Y;
		tweens.push_back(tween);
	}
}

void Tweening::MoveZ(GameObject* object, float targetZ, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentPos = object->GetTransform()->GetPosition();
	tween.startPosition = currentPos;
	tween.targetPosition = glm::dvec3(currentPos.x, currentPos.y, targetZ);
	tween.tweenType = TweenType::POSITION_Z;

	tweens.push_back(tween);
}

void Tweening::UIMoveZ(GameObject* object, float targetZ, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startPosition = uiTransform->GetPosition();
		tween.targetPosition = glm::dvec3(uiTransform->GetPosition().x, uiTransform->GetPosition().y, targetZ);
		tween.tweenType = TweenType::UIPOSITION_Z;
		tweens.push_back(tween);
	}
}

void Tweening::Rotate(GameObject* object, const glm::dvec3& targetRotation, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	tween.startRotation = object->GetTransform()->GetEulerAngles();
	tween.targetRotation = targetRotation;
	tween.tweenType = TweenType::ROTATION;

	tweens.push_back(tween);
}

void Tweening::UIRotate(GameObject* object, const glm::dvec3& targetRotation, float duration, Modes mode) {
	if (!object) return;
	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startRotation = uiTransform->GetRotation();
		tween.targetRotation = targetRotation;
		tween.tweenType = TweenType::UIROTATION;
		tweens.push_back(tween);
	}
}

void Tweening::RotateX(GameObject* object, float targetX, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentRot = object->GetTransform()->GetEulerAngles();
	tween.startRotation = currentRot;
	tween.targetRotation = glm::dvec3(targetX, currentRot.y, currentRot.z);
	tween.tweenType = TweenType::ROTATION_X;

	tweens.push_back(tween);
}

void Tweening::UIRotateX(GameObject* object, float targetX, float duration, Modes mode) {
	if (!object) return;
	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startRotation = uiTransform->GetRotation();
		tween.targetRotation = glm::dvec3(targetX, uiTransform->GetRotation().y, uiTransform->GetRotation().z);
		tween.tweenType = TweenType::UIROTATION_X;
		tweens.push_back(tween);
	}
}

void Tweening::RotateY(GameObject* object, float targetY, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentRot = object->GetTransform()->GetEulerAngles();
	tween.startRotation = currentRot;
	tween.targetRotation = glm::dvec3(currentRot.x, targetY, currentRot.z);
	tween.tweenType = TweenType::ROTATION_Y;

	tweens.push_back(tween);
}

void Tweening::UIRotateY(GameObject* object, float targetY, float duration, Modes mode) {
	if (!object) return;
	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startRotation = uiTransform->GetRotation();
		tween.targetRotation = glm::dvec3(uiTransform->GetRotation().x, targetY, uiTransform->GetRotation().z);
		tween.tweenType = TweenType::UIROTATION_Y;
		tweens.push_back(tween);
	}
}

void Tweening::RotateZ(GameObject* object, float targetZ, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentRot = object->GetTransform()->GetEulerAngles();
	tween.startRotation = currentRot;
	tween.targetRotation = glm::dvec3(currentRot.x, currentRot.y, targetZ);
	tween.tweenType = TweenType::ROTATION_Z;

	tweens.push_back(tween);
}

void Tweening::UIRotateZ(GameObject* object, float targetZ, float duration, Modes mode) {
	if (!object) return;
	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startRotation = uiTransform->GetRotation();
		tween.targetRotation = glm::dvec3(uiTransform->GetRotation().x, uiTransform->GetRotation().y, targetZ);
		tween.tweenType = TweenType::UIROTATION_Z;
		tweens.push_back(tween);
	}
}

void Tweening::Scale(GameObject* object, const glm::dvec3& targetScale, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	tween.startScale = object->GetTransform()->GetScale();
	tween.targetScale = targetScale;
	tween.tweenType = TweenType::SCALE;

	tweens.push_back(tween);
}

void Tweening::UIScale(GameObject* object, const glm::dvec3& targetScale, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startScale = uiTransform->GetScale();
		tween.targetScale = targetScale;
		tween.tweenType = TweenType::UISCALE;
		tweens.push_back(tween);
	}
}

void Tweening::ScaleX(GameObject* object, float targetX, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentScale = object->GetTransform()->GetScale();
	tween.startScale = currentScale;
	tween.targetScale = glm::dvec3(targetX, currentScale.y, currentScale.z);
	tween.tweenType = TweenType::SCALE_X;

	tweens.push_back(tween);
}

void Tweening::UIScaleX(GameObject* object, float targetX, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startScale = uiTransform->GetScale();
		tween.targetScale = glm::dvec3(targetX, uiTransform->GetScale().y, uiTransform->GetScale().z);
		tween.tweenType = TweenType::UISCALE_X;
		tweens.push_back(tween);
	}
}

void Tweening::ScaleY(GameObject* object, float targetY, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentScale = object->GetTransform()->GetScale();
	tween.startScale = currentScale;
	tween.targetScale = glm::dvec3(currentScale.x, targetY, currentScale.z);
	tween.tweenType = TweenType::SCALE_Y;

	tweens.push_back(tween);
}

void Tweening::UIScaleY(GameObject* object, float targetY, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startScale = uiTransform->GetScale();
		tween.targetScale = glm::dvec3(uiTransform->GetScale().x, targetY, uiTransform->GetScale().z);
		tween.tweenType = TweenType::UISCALE_Y;
		tweens.push_back(tween);
	}
}

void Tweening::ScaleZ(GameObject* object, float targetZ, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	glm::dvec3 currentScale = object->GetTransform()->GetScale();
	tween.startScale = currentScale;
	tween.targetScale = glm::dvec3(currentScale.x, currentScale.y, targetZ);
	tween.tweenType = TweenType::SCALE_Z;

	tweens.push_back(tween);
}

void Tweening::UIScaleZ(GameObject* object, float targetZ, float duration, Modes mode) {
	if (!object) return;

	auto tween = CreateTween(object, duration, mode);
	auto uiTransform = object->GetComponent<UITransformComponent>();
	if (uiTransform) {
		tween.startScale = uiTransform->GetScale();
		tween.targetScale = glm::dvec3(uiTransform->GetScale().x, uiTransform->GetScale().y, targetZ);
		tween.tweenType = TweenType::UISCALE_Z;
		tweens.push_back(tween);
	}
}
Tweening::TweenHandle Tweening::TweenValue(float* value, float start, float target, float duration, Modes mode) {
	Tween tween;
	tween.floatPtr = value;
	tween.startFloat = start;
	tween.targetFloat = target;
	tween.duration = duration;
	tween.elapsedTime = 0.0f;
	tween.mode = mode;
	tween.tweenType = TweenType::FLOAT_VALUE;

	static int nextHandle = 1;
	tween.handle = nextHandle++;

	tweens.push_back(tween);
	return tween.handle;
}

Tweening::TweenHandle Tweening::TweenColor(glm::vec4* color, const glm::vec4& startColor, const glm::vec4& targetColor, float duration, Modes mode) {
	Tween tween;
	tween.colorPtr = color;
	tween.startColor = startColor;
	tween.targetColor = targetColor;
	tween.duration = duration;
	tween.elapsedTime = 0.0f;
	tween.mode = mode;
	tween.tweenType = TweenType::COLOR;

	static int nextHandle = 1;
	tween.handle = nextHandle++;

	tweens.push_back(tween);
	return tween.handle;
}

Tweening::TweenHandle Tweening::TweenVec3(glm::vec3* vec, const glm::vec3& start, const glm::vec3& target, float duration, Modes mode) {
	Tween tween;
	tween.vec3Ptr = vec;
	tween.startVec3 = start;
	tween.targetVec3 = target;
	tween.duration = duration;
	tween.elapsedTime = 0.0f;
	tween.mode = mode;
	tween.tweenType = TweenType::VEC3;

	static int nextHandle = 1;
	tween.handle = nextHandle++;

	tweens.push_back(tween);
	return tween.handle;
}

void Tweening::Cancel(TweenHandle handle) {
	tweens.erase(std::remove_if(tweens.begin(), tweens.end(),
		[handle](const Tween& tween) { return tween.handle == handle; }),
		tweens.end());
}

void Tweening::CancelAll(GameObject* object) {
	if (!object) return;

	tweens.erase(std::remove_if(tweens.begin(), tweens.end(),
		[object](const Tween& tween) { return tween.object == object; }),
		tweens.end());
}

void Tweening::SetOnComplete(TweenHandle handle, std::function<void()> callback) {
	for (auto& tween : tweens) {
		if (tween.handle == handle) {
			tween.onComplete = callback;
			break;
		}
	}
}

void Tweening::SetOnUpdate(TweenHandle handle, std::function<void(float)> callback) {
	for (auto& tween : tweens) {
		if (tween.handle == handle) {
			tween.onUpdate = callback;
			break;
		}
	}
}

float Tweening::CalculateT(float normalizedTime, Modes mode) {
	switch (mode) {
	case Modes::LINEAR:
		return normalizedTime;
	case Modes::EASE_IN:
		return normalizedTime * normalizedTime;
	case Modes::EASE_OUT:
		return 1.0f - (1.0f - normalizedTime) * (1.0f - normalizedTime);
	case Modes::EASE_IN_OUT:
		return normalizedTime < 0.5f ?
			2.0f * normalizedTime * normalizedTime :
			1.0f - pow(-2.0f * normalizedTime + 2.0f, 2) / 2.0f;
	default:
		return normalizedTime;
	}
}

glm::dvec3 Tweening::CalculatePosition(const glm::dvec3& startPos, const glm::dvec3& targetPos, float t, Modes mode) {
	float easedT = CalculateT(t, mode);
	return glm::mix(startPos, targetPos, easedT);
}

glm::dvec3 Tweening::CalculateRotation(const glm::dvec3& startRot, const glm::dvec3& targetRot, float t, Modes mode) {
	float easedT = CalculateT(t, mode);
	return glm::mix(startRot, targetRot, easedT);
}

glm::dvec3 Tweening::CalculateScale(const glm::dvec3& startScale, const glm::dvec3& targetScale, float t, Modes mode) {
	float easedT = CalculateT(t, mode);
	return glm::mix(startScale, targetScale, easedT);
}

float Tweening::CalculateFloat(float start, float target, float t, Modes mode) {
	float easedT = CalculateT(t, mode);
	return start + (target - start) * easedT;
}

glm::vec4 Tweening::CalculateColor(const glm::vec4& startColor, const glm::vec4& targetColor, float t, Modes mode) {
	float easedT = CalculateT(t, mode);
	return glm::mix(startColor, targetColor, easedT);
}

glm::vec3 Tweening::CalculateVec3(const glm::vec3& startVec3, const glm::vec3& targetVec3, float t, Modes mode) {
	float easedT = CalculateT(t, mode);
	return glm::mix(startVec3, targetVec3, easedT);
}

void Tweening::Update(float deltaTime) {
	for (auto& tween : tweens) {
		tween.elapsedTime += deltaTime;
		float t = glm::clamp(tween.elapsedTime / tween.duration, 0.0f, 1.0f);

		if (tween.onUpdate) {
			tween.onUpdate(t);
		}

		if (!tween.object->HasComponent<UITransformComponent>()) {
			continue;
		}

		switch (tween.tweenType) {
		case TweenType::POSITION:
		case TweenType::POSITION_X:
		case TweenType::POSITION_Y:
		case TweenType::POSITION_Z: {
			glm::dvec3 currentPosition = CalculatePosition(tween.startPosition, tween.targetPosition, t, tween.mode);
			if (tween.object && tween.object->GetTransform()) {
				tween.object->GetTransform()->SetPosition(currentPosition);
			}
			break;
		}

		case TweenType::UIPOSITION:
		case TweenType::UIPOSITION_X:
		case TweenType::UIPOSITION_Y:
		case TweenType::UIPOSITION_Z: {
			auto uiTransform = tween.object->GetComponent<UITransformComponent>();
			if (uiTransform) {
				glm::dvec3 currentPosition = CalculatePosition(tween.startPosition, tween.targetPosition, t, tween.mode);
				uiTransform->setPos(currentPosition);
			}
			break;
		}

		case TweenType::ROTATION:
		case TweenType::ROTATION_X:
		case TweenType::ROTATION_Y:
		case TweenType::ROTATION_Z: {
			glm::dvec3 currentRotation = CalculateRotation(tween.startRotation, tween.targetRotation, t, tween.mode);
			if (tween.object && tween.object->GetTransform()) {
				tween.object->GetTransform()->SetRotation(glm::radians(currentRotation));
			}
			break;
		}

		case TweenType::UIROTATION:
		case TweenType::UIROTATION_X:
		case TweenType::UIROTATION_Y:
		case TweenType::UIROTATION_Z: {
			auto uiTransform = tween.object->GetComponent<UITransformComponent>();
			if (uiTransform) {
				glm::dvec3 currentRotation = CalculateRotation(tween.startRotation, tween.targetRotation, t, tween.mode);
				uiTransform->SetRot(glm::radians(currentRotation));
			}
			break;
		}

		case TweenType::SCALE:
		case TweenType::SCALE_X:
		case TweenType::SCALE_Y:
		case TweenType::SCALE_Z: {
			glm::dvec3 currentScale = CalculateScale(tween.startScale, tween.targetScale, t, tween.mode);
			if (tween.object && tween.object->GetTransform()) {
				tween.object->GetTransform()->SetScale(currentScale);
			}
			break;
		}

		case TweenType::UISCALE:
		case TweenType::UISCALE_X:
		case TweenType::UISCALE_Y:
		case TweenType::UISCALE_Z: {
			auto uiTransform = tween.object->GetComponent<UITransformComponent>();
			if (uiTransform) {
				glm::dvec3 currentScale = CalculateScale(tween.startScale, tween.targetScale, t, tween.mode);
				uiTransform->setScale(currentScale);
			}
			break;
		}
		case TweenType::FLOAT_VALUE: {
			if (tween.floatPtr) {
				*tween.floatPtr = CalculateFloat(tween.startFloat, tween.targetFloat, t, tween.mode);
			}
			break;
		}

		case TweenType::COLOR: {
			if (tween.colorPtr) {
				*tween.colorPtr = CalculateColor(tween.startColor, tween.targetColor, t, tween.mode);
			}
			break;
		}
		case TweenType::VEC3: {
			if (tween.vec3Ptr) {
				*tween.vec3Ptr = CalculateVec3(tween.startVec3, tween.targetVec3, t, tween.mode);
			}
			break;
		}

		if (tween.elapsedTime >= tween.duration && tween.onComplete) {
			tween.onComplete();
							}
		}
	}

	tweens.erase(std::remove_if(tweens.begin(), tweens.end(),
		[](const Tween& tween) { return tween.elapsedTime >= tween.duration; }),
		tweens.end());
}

//Tweening::Sequence Tweening::CreateSequence() {
//	return Sequence();
//}
//
//Tweening::Sequence::Sequence() : currentIndex(0), isPlaying(false) {}
//
//Tweening::Sequence& Tweening::Sequence::Append(std::function<TweenHandle()> tweenCreator) {
//	steps.push_back({ tweenCreator, 0, StepType::TWEEN });
//	return *this;
//}
//
//Tweening::Sequence& Tweening::Sequence::AppendDelay(float duration) {
//	steps.push_back({ nullptr, duration, StepType::DELAY });
//	return *this;
//}
//
//Tweening::Sequence& Tweening::Sequence::AppendCallback(std::function<void()> callback) {
//	steps.push_back({ [callback]() -> TweenHandle {
//		callback();
//		return 0;
//	}, 0, StepType::CALLBACK });
//	return *this;
//}
//
//void Tweening::Sequence::Play() {
//	if (steps.empty()) return;
//
//	isPlaying = true;
//	currentIndex = 0;
//	PlayCurrentStep();
//}
//
//void Tweening::Sequence::Stop() {
//	isPlaying = false;
//	if (currentTweenHandle != 0) {
//		Tweening::Cancel(currentTweenHandle);
//		currentTweenHandle = 0;
//	}
//}
//
//void Tweening::Sequence::PlayCurrentStep() {
//	if (!isPlaying || currentIndex >= steps.size()) {
//		isPlaying = false;
//		return;
//	}
//
//	auto& step = steps[currentIndex];
//
//	switch (step.type) {
//	case StepType::TWEEN: {
//		if (step.tweenCreator) {
//			currentTweenHandle = step.tweenCreator();
//			Tweening::SetOnComplete(currentTweenHandle, [this]() {
//				currentIndex++;
//				PlayCurrentStep();
//				});
//		}
//		else {
//			currentIndex++;
//			PlayCurrentStep();
//		}
//		break;
//	}
//
//	case StepType::DELAY: {
//		float dummyValue = 0.0f;
//		currentTweenHandle = Tweening::TweenValue(&dummyValue, 0.0f, 1.0f, step.duration, Modes::LINEAR);
//		Tweening::SetOnComplete(currentTweenHandle, [this]() {
//			currentIndex++;
//			PlayCurrentStep();
//			});
//		break;
//	}
//
//	case StepType::CALLBACK: {
//		if (step.tweenCreator) {
//			step.tweenCreator();
//		}
//		currentIndex++;
//		PlayCurrentStep();
//		break;
//	}
//	}
//}