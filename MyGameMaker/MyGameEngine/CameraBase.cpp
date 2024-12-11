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
    return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), transform.GetUp());
    //glm::dmat4 rotation = glm::mat4_cast(glm::conjugate(transform.GetRotation()));
    //glm::dmat4 translation = glm::translate(glm::dmat4(1.0), -transform.GetPosition());
    //return rotation * translation;
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
    for (int i = 0; i < 8; ++i)
    {
        Log(__FILE__, __LINE__, LogType::LOG_INFO,
            "Frustum vertex %d: (%.2f, %.2f, %.2f)",
            i, frustum.vertices[i].x, frustum.vertices[i].y, frustum.vertices[i].z);
    }

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

bool CameraBase::IsInsideFrustrum(const BoundingBox& bbox) const
{
    for (const auto& plane : { frustum.left, frustum.right, frustum.top, frustum.bot, frustum._near, frustum._far })
    {
        bool isInside = false;
        for (const glm::vec3& vertex : bbox.vertices())
        {
            float distance = plane.distanceToPoint(vertex);
            Log(__FILE__, __LINE__, LogType::LOG_INFO, "Distancia del vértice (%.2f, %.2f, %.2f) al plano: %.2f", vertex.x, vertex.y, vertex.z, distance);
            if (distance >= 0)
            {
                isInside = true;
                break;
            }
        }

        if (!isInside)
        {
            Log(__FILE__, __LINE__, LogType::LOG_WARNING, "BoundingBox fuera del frustum");
            return false;
        }

    }
    Log(__FILE__, __LINE__, LogType::LOG_INFO, "BoundingBox dentro del frustum");
    return true;
}


void CameraBase::RunTests()
{
    // Caso típico
    glm::vec3 v1(3.0f, 4.0f, 0.0f);
    glm::vec3 normalizedV1 = glm::normalize(v1);
    Log(__FILE__, __LINE__, LogType::LOG_INFO,
        "Test Normalize: vector original (%.2f, %.2f, %.2f), vector normalizado (%.2f, %.2f, %.2f)",
        v1.x, v1.y, v1.z, normalizedV1.x, normalizedV1.y, normalizedV1.z);

    // Verificar magnitud
    float magnitude = glm::length(normalizedV1);
    Log(__FILE__, __LINE__, LogType::LOG_INFO,
        "Test Normalize: magnitud del vector normalizado = %.2f (debería ser 1.0)", magnitude);

    // Caso límite: vector cero
    glm::vec3 v2(0.0f, 0.0f, 0.0f);
    glm::vec3 normalizedV2 = glm::normalize(v2);
    Log(__FILE__, __LINE__, LogType::LOG_INFO,
        "Test Normalize: vector original (%.2f, %.2f, %.2f), vector normalizado (%.2f, %.2f, %.2f)",
        v2.x, v2.y, v2.z, normalizedV2.x, normalizedV2.y, normalizedV2.z);

    Plane plane(glm::vec3(0, 1, 0), 0); // Plano horizontal en y=0

    // Punto sobre el plano
    glm::vec3 pointOnPlane(1.0f, 0.0f, 1.0f);
    float distanceOnPlane = plane.distanceToPoint(pointOnPlane);
    Log(__FILE__, __LINE__, LogType::LOG_INFO,
        "Test DistanceToPoint: punto (%.2f, %.2f, %.2f), distancia al plano = %.2f (debería ser 0.0)",
        pointOnPlane.x, pointOnPlane.y, pointOnPlane.z, distanceOnPlane);

    // Punto por encima del plano
    glm::vec3 pointAbove(1.0f, 2.0f, 1.0f);
    float distanceAbove = plane.distanceToPoint(pointAbove);
    Log(__FILE__, __LINE__, LogType::LOG_INFO,
        "Test DistanceToPoint: punto (%.2f, %.2f, %.2f), distancia al plano = %.2f (debería ser 2.0)",
        pointAbove.x, pointAbove.y, pointAbove.z, distanceAbove);

    // Punto por debajo del plano
    glm::vec3 pointBelow(1.0f, -1.0f, 1.0f);
    float distanceBelow = plane.distanceToPoint(pointBelow);
    Log(__FILE__, __LINE__, LogType::LOG_INFO,
        "Test DistanceToPoint: punto (%.2f, %.2f, %.2f), distancia al plano = %.2f (debería ser -1.0)",
        pointBelow.x, pointBelow.y, pointBelow.z, distanceBelow);
}
