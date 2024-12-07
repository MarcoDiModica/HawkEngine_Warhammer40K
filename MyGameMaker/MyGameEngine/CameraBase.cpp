#include "CameraBase.h"

CameraBase::CameraBase(float fov, double zNear, double zFar, double aspect) :
    fov(fov),
    zNear(zNear),
    zFar(zFar),
    aspect(aspect)
{
}

glm::dmat4 CameraBase::GetProjectionMatrix() const
{
    return glm::perspective(fov, aspect, zNear, zFar);
}

glm::dmat4 CameraBase::GetViewMatrix(const Transform_Component& transform) const
{
    return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), transform.GetUp());
}

void CameraBase::SetFOV(float fov)
{
    this->fov = fov;
}

float CameraBase::GetFOV() const
{
    return fov;
}

void CameraBase::SetNearPlane(double zNear)
{
    this->zNear = zNear;
}

double CameraBase::GetNearPlane() const
{
    return zNear;
}

void CameraBase::SetFarPlane(double zFar)
{
    this->zFar = zFar;
}

double CameraBase::GetFarPlane() const
{
    return zFar;
}

void CameraBase::UpdateAspectRatio(double aspectRatio)
{
    aspect = aspectRatio;
}