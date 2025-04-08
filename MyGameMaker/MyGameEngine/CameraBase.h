#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "TransformComponent.h"
#include "BoundingBox.h"

class CameraBase
{
public:
	CameraBase(float fov = glm::radians(60.0), double zNear = 0.1, double zFar = 1000, double aspect = 16.0 / 9.0);
	virtual ~CameraBase() = default;

	glm::dmat4 GetProjectionMatrix() const;
	glm::dmat4 GetViewMatrix(const Transform_Component& transform) const;

	void SetFOV(float fov);
	float GetFOV() const;
	void SetNearPlane(double zNear);
	double GetNearPlane() const;
	void SetFarPlane(double zFar);
	double GetFarPlane() const;
	void SetOrthographic(bool orthographic);
	bool IsOrthographic() const { return orthographic; }
	float GetOrthoSize() const { return orthoSize; }
	void SetOrthoSize(float size);
	void UpdateAspectRatio(double aspectRatio);

	struct Plane
	{
		glm::vec3 normal;
		float distance;

		static Plane CreatePlaneFromVec4(const glm::vec4& vec)
		{
			Plane plane;
			float length = glm::length(glm::vec3(vec));
			plane.normal = glm::vec3(vec) / length;
			plane.distance = vec.w / length;
			return plane;
		}

		inline float distanceToPoint(const glm::vec3& point) const
		{
			return glm::dot(normal, point) + distance;
		}
	};

	enum FrustumIntersection { OUTSIDE, INTERSECT, INSIDE };

	struct Frustum
	{
		union {
			struct {
				Plane _near;
				Plane _far;
				Plane left;
				Plane right;
				Plane top;
				Plane bot;
			};
			Plane planes[6];
		};

		glm::vec3 vertices[8];
		glm::mat4 lastVPMatrix;
		bool needsUpdate;

		Frustum() : needsUpdate(true) {}

		void Update(const glm::mat4& vpm);
		void CalculateVertices(const glm::mat4& transform);

		void Invalidate() { needsUpdate = true; }
	};

public:
	double fov;
	double aspect;
	double zNear;
	double zFar;
	bool orthographic;
	float orthoSize;
	float yaw, pitch;
	Frustum frustum;

	void DrawFrustrum();

	FrustumIntersection TestFrustumAABB(const BoundingBox& bbox) const;

	bool IsInsideFrustrum(const BoundingBox& bbox) const;
};