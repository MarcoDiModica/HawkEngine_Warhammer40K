#include "CameraComponent.h"
#include "GameObject.h"
#include "../MyGameEditor/Log.h"
#include "MeshRendererComponent.h"
#include "Scene.h"
#include <random>

CameraComponent::CameraComponent(GameObject* owner) : Component(owner), CameraBase()
{
    name = "CameraComponent";
}

CameraComponent::CameraComponent(const CameraComponent& other) :
    Component(other),
    CameraBase(other.GetFOV(), other.GetNearPlane(), other.GetFarPlane(), other.aspect)
{
}

CameraComponent::CameraComponent(CameraComponent&& other) noexcept :
    Component(std::move(other)),
    CameraBase(other.GetFOV(), other.GetNearPlane(), other.GetFarPlane(), other.aspect)
{
}

CameraComponent& CameraComponent::operator=(const CameraComponent& other)
{
    if (this != &other)
    {
        Component::operator=(other);
        SetFOV(other.GetFOV());
        SetNearPlane(other.GetNearPlane());
        SetFarPlane(other.GetFarPlane());
        aspect = other.aspect;
    }
    return *this;
}

CameraComponent& CameraComponent::operator=(CameraComponent&& other) noexcept
{
    if (this != &other)
    {
        Component::operator=(std::move(other));
        SetFOV(other.GetFOV());
        SetNearPlane(other.GetNearPlane());
        SetFarPlane(other.GetFarPlane());
        aspect = other.aspect;
    }
    return *this;
}

void CameraComponent::Start()
{
	UpdateCameraView(1280, 720, 1280, 720);
}

void CameraComponent::Update(float deltaTime)
{
    if (followTarget != nullptr) {
        glm::dvec3 targetPosition = followTarget->GetTransform()->GetPosition();
        glm::dvec3 desiredPosition = targetPosition + followOffset - followTarget->GetTransform()->GetForward() * followDistance;
        glm::dvec3 currentPosition = owner->GetTransform()->GetPosition();

        if (!followX) {
            desiredPosition.x = currentPosition.x;
        }
        if (!followY) {
            desiredPosition.y = currentPosition.y;
        }
        if (!followZ) {
            desiredPosition.z = currentPosition.z;
        }

        double t = glm::clamp(followSmoothness * deltaTime, 0.0, 1.0);
        glm::dvec3 newPosition = glm::mix(currentPosition, desiredPosition, followSmoothness * deltaTime);
        owner->GetTransform()->SetPosition(newPosition);
    }

    if (lookAtTarget != nullptr) {
        glm::dvec3 targetPosition = lookAtTarget->GetTransform()->GetPosition();
        glm::dvec3 lookAtPosition = targetPosition + lookAtOffset;

        glm::dvec3 direction = glm::normalize(lookAtPosition - owner->GetTransform()->GetPosition());
        glm::quat newRotation = glm::quatLookAt(direction, owner->GetTransform()->GetUp());

        glm::quat currentRotation = owner->GetTransform()->GetRotation();
        glm::quat smoothedRotation = glm::slerp(currentRotation, newRotation, lookAtSmoothness * deltaTime);
        owner->GetTransform()->SetRotationQuat(smoothedRotation);
    }

    if (ShakeEnabled)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dist(-1.0f, 1.0f);

        float shakeX = dist(gen) * shakeIntensity;
        float shakeY = dist(gen) * shakeIntensity;
        float shakeZ = dist(gen) * shakeIntensity;

        glm::vec3 currentPosition = owner->GetTransform()->GetPosition();
        owner->GetTransform()->SetPosition(currentPosition + glm::vec3(shakeX, shakeY, shakeZ));

        shakeTimer -= deltaTime;
        if (shakeTimer <= 0.0f)
        {
            ShakeEnabled = false;
        }
    }

    if (frustrumCullingEnabled)
    {
        frustum.Update(GetViewMatrix(*owner->GetTransform()) * GetProjectionMatrix());

        if (frustrumRepresentation)
		{
            DrawFrustrum();
		}

        for (auto& gameObject : owner->GetScene()->children())
		{
            if (gameObject.get() == owner)
            {
                continue;
            }

            if (gameObject->HasComponent<MeshRenderer>())
			{
				if (IsInsideFrustrum(gameObject->GetComponent<MeshRenderer>()->GetMesh()->boundingBox()))
				{
                    LOG(LogType::LOG_INFO, "inside %s",gameObject->GetName().c_str());
                    
                    gameObject->SetActive(true);
				}
				else
				{
                    LOG(LogType::LOG_INFO, "outside %s",gameObject->GetName().c_str());
					gameObject->SetActive(false);
				}
			}
		}
    }

    if (orthographic)
    {
        projectionType = ProjectionType::Orthographic;
    }
    else
    {
        projectionType = ProjectionType::Perspective;
    }
}

std::unique_ptr<Component> CameraComponent::Clone(GameObject* owner)
{
    auto clone = std::make_unique<CameraComponent>(*this);
    clone->owner = owner;
    return clone;
}

void CameraComponent::UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight)
{
    double windowAspect = windowWidth / windowHeight;
    double imageAspect = imageWidth / imageHeight;

    UpdateAspectRatio(windowAspect);

    if (windowAspect > imageAspect) {
        SetFOV(2.0 * atan(tan(glm::radians(60.0) / 2.0) * (imageAspect / windowAspect)));
    }
    else {
        SetFOV(glm::radians(60.0));
    }
}

void CameraComponent::SetPerspective(float fov, float nearPlane, float farPlane)
{
	SetFOV(fov);
	SetNearPlane(nearPlane);
	SetFarPlane(farPlane);
}

void CameraComponent::SetOrthographic(float size, float nearPlane, float farPlane)
{
	orthographic = true;
	orthoSize = size;
	SetNearPlane(nearPlane);
	SetFarPlane(farPlane);
}

void CameraComponent::FollowTarget(GameObject* target, float distance, const glm::vec3& offset, float smoothness, bool followX, bool followY, bool followZ)
{
    followTarget = target;
    followDistance = distance;
    followOffset = offset;
    followSmoothness = smoothness;
    this->followX = followX;
    this->followY = followY;
    this->followZ = followZ;
}

void CameraComponent::LookAtTarget(GameObject* target, const glm::vec3& offset, float smoothness)
{
    lookAtTarget = target;
    lookAtOffset = offset;
    lookAtSmoothness = smoothness;
}

void CameraComponent::Shake(float intensity, float duration, float frequency)
{
	shakeIntensity = intensity;
	shakeDuration = duration;
	shakeFrequency = frequency;
	ShakeEnabled = true;
}