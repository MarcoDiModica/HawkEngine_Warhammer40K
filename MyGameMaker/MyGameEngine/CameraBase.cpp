#include "CameraBase.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

CameraBase::CameraBase(float fov, double zNear, double zFar, double aspect) :
	fov(fov),
	zNear(zNear),
	zFar(zFar),
	aspect(aspect),
	orthographic(false),
	orthoSize(5.0f),
	yaw(0.0f),
	pitch(0.0f)
{
}

glm::dmat4 CameraBase::GetProjectionMatrix() const
{
	if (orthographic) {
		return glm::ortho(-orthoSize * (float)aspect, orthoSize * (float)aspect, -orthoSize, orthoSize, (float)zNear, (float)zFar);
	}
	else
	{
		return glm::perspective(fov, aspect, zNear, zFar);
	}
}

glm::dmat4 CameraBase::GetViewMatrix(const Transform_Component& transform) const
{
	return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), transform.GetUp());
}

void CameraBase::SetFOV(float fov)
{
	this->fov = fov;
	frustum.Invalidate(); 
}

float CameraBase::GetFOV() const
{
	return static_cast<float>(fov);
}

void CameraBase::SetNearPlane(double zNear)
{
	this->zNear = zNear;
	frustum.Invalidate(); 
}

double CameraBase::GetNearPlane() const
{
	return zNear;
}

void CameraBase::SetFarPlane(double zFar)
{
	this->zFar = zFar;
	frustum.Invalidate(); 
}

double CameraBase::GetFarPlane() const
{
	return zFar;
}

void CameraBase::UpdateAspectRatio(double aspectRatio)
{
	aspect = aspectRatio;
	frustum.Invalidate(); 
}

void CameraBase::SetOrthographic(bool orthographic)
{
	this->orthographic = orthographic;
	frustum.Invalidate(); 
}

void CameraBase::SetOrthoSize(float size)
{
	orthoSize = size;
	frustum.Invalidate(); 
}

void CameraBase::Frustum::Update(const glm::mat4& vpm)
{
	if (!needsUpdate && vpm == lastVPMatrix)
		return;

	left = Plane::CreatePlaneFromVec4({
		vpm[0][3] + vpm[0][0],
		vpm[1][3] + vpm[1][0],
		vpm[2][3] + vpm[2][0],
		vpm[3][3] + vpm[3][0]
		});

	right = Plane::CreatePlaneFromVec4({
		vpm[0][3] - vpm[0][0],
		vpm[1][3] - vpm[1][0],
		vpm[2][3] - vpm[2][0],
		vpm[3][3] - vpm[3][0]
		});

	bot = Plane::CreatePlaneFromVec4({
		vpm[0][3] + vpm[0][1],
		vpm[1][3] + vpm[1][1],
		vpm[2][3] + vpm[2][1],
		vpm[3][3] + vpm[3][1]
		});

	top = Plane::CreatePlaneFromVec4({
		vpm[0][3] - vpm[0][1],
		vpm[1][3] - vpm[1][1],
		vpm[2][3] - vpm[2][1],
		vpm[3][3] - vpm[3][1]
		});

	_near = Plane::CreatePlaneFromVec4({
		vpm[0][3] + vpm[0][2],
		vpm[1][3] + vpm[1][2],
		vpm[2][3] + vpm[2][2],
		vpm[3][3] + vpm[3][2]
		});

	_far = Plane::CreatePlaneFromVec4({
		vpm[0][3] - vpm[0][2],
		vpm[1][3] - vpm[1][2],
		vpm[2][3] - vpm[2][2],
		vpm[3][3] - vpm[3][2]
		});

	CalculateVertices(vpm);
	lastVPMatrix = vpm;
	needsUpdate = false;
}

void CameraBase::Frustum::CalculateVertices(const glm::mat4& transform)
{
	static const bool zerotoOne = false;
	glm::mat4 transformInv = glm::inverse(transform);

	glm::vec4 clipSpaceCorners[8] = {
		{-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f},
		{ 1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f},
		{ 1.0f,  1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f},
		{-1.0f,  1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f, 1.0f},
		{ 1.0f,  1.0f, 1.0f, 1.0f},
		{-1.0f,  1.0f, 1.0f, 1.0f} 
	};

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 worldSpaceCorner = transformInv * clipSpaceCorners[i];
		vertices[i] = glm::vec3(worldSpaceCorner) / worldSpaceCorner.w;
	}
}

CameraBase::FrustumIntersection CameraBase::TestFrustumAABB(const BoundingBox& bbox) const
{
	const glm::vec3& bMin = bbox.min;
	const glm::vec3& bMax = bbox.max;

	FrustumIntersection result = FrustumIntersection::INSIDE;

	for (int i = 0; i < 6; i++)
	{
		const Plane& plane = frustum.planes[i];

		glm::vec3 p(bMin);
		if (plane.normal.x >= 0) p.x = bMax.x;
		if (plane.normal.y >= 0) p.y = bMax.y;
		if (plane.normal.z >= 0) p.z = bMax.z;

		glm::vec3 n(bMax);
		if (plane.normal.x >= 0) n.x = bMin.x;
		if (plane.normal.y >= 0) n.y = bMin.y;
		if (plane.normal.z >= 0) n.z = bMin.z;

		if (plane.distanceToPoint(p) < 0)
			return FrustumIntersection::OUTSIDE;

		if (plane.distanceToPoint(n) < 0)
			result = FrustumIntersection::INTERSECT;
	}

	return result;
}

bool CameraBase::IsInsideFrustrum(const BoundingBox& bbox) const
{
	return TestFrustumAABB(bbox) != FrustumIntersection::OUTSIDE;
}

void CameraBase::DrawFrustrum()
{
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++) {
		glVertex3fv(glm::value_ptr(frustum.vertices[i]));
		glVertex3fv(glm::value_ptr(frustum.vertices[(i + 1) % 4]));

		glVertex3fv(glm::value_ptr(frustum.vertices[i + 4]));
		glVertex3fv(glm::value_ptr(frustum.vertices[(i + 1) % 4 + 4]));

		glVertex3fv(glm::value_ptr(frustum.vertices[i]));
		glVertex3fv(glm::value_ptr(frustum.vertices[i + 4]));
	}
	glEnd();
}