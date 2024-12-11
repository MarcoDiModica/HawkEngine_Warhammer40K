#include "CameraBase.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

CameraBase::CameraBase(float fov, double zNear, double zFar, double aspect) :
    fov(fov),
    zNear(zNear),
    zFar(zFar),
    aspect(aspect)
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
    glm::dmat4 rotation = glm::mat4_cast(glm::conjugate(transform.GetRotation()));
    glm::dmat4 translation = glm::translate(glm::dmat4(1.0), -transform.GetPosition());
    return rotation * translation;
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

void CameraBase::SetOrthographic(bool orthographic)
{
	this->orthographic = orthographic;
}

void CameraBase::SetOrthoSize(float size)
{
	orthoSize = size;
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

bool CameraBase::IsInsideFrustrum(const BoundingBox& bbox)
{
    for (const auto& plane : { frustum._near, frustum._far, frustum.left, frustum.right, frustum.top, frustum.bot })
    {
        int outsideCount = 0;
        for (const auto& vertex : { bbox.v000(), bbox.v001(), bbox.v010(), bbox.v011(),
                                    bbox.v100(), bbox.v101(), bbox.v110(), bbox.v111() })
        {
            if (plane.distanceToPoint(vertex) < 0) {
                outsideCount++;
            }
        }
        if (outsideCount == 8) {
            return false;  
        }
    }
    return true;  
}