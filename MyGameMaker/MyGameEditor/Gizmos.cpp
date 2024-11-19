#include "Gizmos.h"
#include "App.h"
#include "Input.h"
#include "MyGUI.h"
#include "UISceneWindow.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> 

Gizmos::Gizmos(App* app) : Module(app) { ; }

bool Gizmos::FixedUpdate()
{
	return true;
}



bool Gizmos::Start() { return true; }

bool Gizmos::Awake() { return true; }

bool Gizmos::Update(double dt) {

    rayStartPos = glm::vec3(glm::inverse(Application->camera->view()) * glm::vec4(0, 0, 0, 1));
    rayDir = Application->input->getMousePickRay();

	if (Application->input->GetSelectedGameObject() != nullptr) {
        ProcessMousePicking();
	}


	return true;
}

//(Fornow) it cant go on the update since we need it inside the drawfunction on main
void Gizmos::DrawGizmos() {
 
    if (Application->input->GetSelectedGameObject() != nullptr) 
    {
		IntersectAxis(rayStartPos, rayDir, selectedAxis);
        glm::vec3 position = Application->input->GetSelectedGameObject()->GetTransform()->GetPosition();

        // Deshabilitar la prueba de profundidad para dibujar los gizmos sobre la malla
        glDisable(GL_DEPTH_TEST);

        glBegin(GL_LINES);

        // Dibujar eje X (rojo)
        if (selectedAxis == glm::vec3(1, 0, 0)) {
            glColor3f(1.0f, 1.0f, 0.0f); // Amarillo si está seleccionado
        }
        else {
            glColor3f(1.0f, 0.0f, 0.0f);
        }
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(position.x + 1.0f, position.y, position.z);

        // Dibujar eje Y (verde)
        if (selectedAxis == glm::vec3(0, 1, 0)) {
            glColor3f(1.0f, 1.0f, 0.0f); // Amarillo si está seleccionado
        }
        else {
            glColor3f(0.0f, 1.0f, 0.0f);
        }
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(position.x, position.y + 1.0f, position.z);

        // Dibujar eje Z (azul)
        if (selectedAxis == glm::vec3(0, 0, 1)) {
            glColor3f(1.0f, 1.0f, 0.0f); // Amarillo si está seleccionado
        }
        else {
            glColor3f(0.0f, 0.0f, 1.0f);
        }
        glVertex3f(position.x, position.y, position.z);
        glVertex3f(position.x, position.y, position.z + 1.0f);

        glEnd();

        // Restaurar la prueba de profundidad
        glEnable(GL_DEPTH_TEST);

        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

void Gizmos::ProcessMousePicking() {
    static bool isDragging = false;

    if (Application->input->GetMouseButton(1) == KEY_DOWN) {
    
		IntersectAxis(rayStartPos, rayDir, selectedAxis);
        // Detectar si el rayo intersecta con alguno de los ejes
        if (IntersectAxis(rayStartPos, rayDir, selectedAxis)) {
            isDragging = true;
            isAxisSelected = true;
        } else {
            isAxisSelected = false;
        }
    }

    if (Application->input->GetMouseButton(1) == KEY_UP) {
        isDragging = false;
        isAxisSelected = false;
        Application->input->SetDraggedGameObject(nullptr);
    }

    if (isDragging) {
    
        // Mover el objeto a lo largo del eje seleccionado
        MoveObjectAlongAxis(rayStartPos, rayDir, selectedAxis);
    }
}

bool RaysIntersect(const glm::vec3& origin1, const glm::vec3& direction1,
    const glm::vec3& origin2, const glm::vec3& direction2,
    float tolerance = 1e-2f)
{
    glm::vec3 d1 = glm::normalize(direction1);
    glm::vec3 d2 = glm::normalize(direction2);

    glm::vec3 crossD = glm::cross(d1, d2);

    float crossLen2 = glm::length2(crossD);
    if (crossLen2 < tolerance) {
    
        glm::vec3 diff = origin2 - origin1;
        if (glm::length2(glm::cross(diff, d1)) < tolerance) {
            return true; 
        }
        return false; 
    }

  
    glm::vec3 diff = origin2 - origin1;

   
    float t1 = glm::dot(glm::cross(diff, d2), crossD) / crossLen2;
    float t2 = glm::dot(glm::cross(diff, d1), crossD) / crossLen2;


    glm::vec3 point1 = origin1 + t1 * d1;
    glm::vec3 point2 = origin2 + t2 * d2;

    if (t2 > 1 || t2 < 0) {
		return false;
    }
    
    return glm::length2(point1 - point2) < tolerance;
}

bool Gizmos::IntersectAxis(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& outAxis) {
    glm::vec3 position = Application->input->GetSelectedGameObject()->GetTransform()->GetPosition();

    // Axis X
    if (RaysIntersect(rayOrigin,rayDirection,position,glm::vec3 (1,0,0)) && isAxisSelected == false){
        selectedAxis = glm::vec3(1, 0, 0);
        return true;
    }

    // Axis Y
    if (RaysIntersect(rayOrigin, rayDirection, position, glm::vec3(0, 1, 0)) && isAxisSelected == false) {
        selectedAxis = glm::vec3(0, 1, 0);
        return true;
    }

    // Axis Z
    if (RaysIntersect(rayOrigin, rayDirection, position, glm::vec3(0, 0, 1)) && isAxisSelected == false) {
        selectedAxis = glm::vec3(0, 0, 1);
        return true;
    }

	if (isAxisSelected == false)
    selectedAxis = glm::vec3(0, 0, 0);

    return false;
}

glm::vec3 GetRayAxisIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& axisOrigin, const glm::vec3& axisDirection) {
    glm::vec3 normalizedRayDir = glm::normalize(rayDirection);
    glm::vec3 normalizedAxisDir = glm::normalize(axisDirection);

    // Calculate the vector from the ray origin to the axis origin
    glm::vec3 originToOrigin = axisOrigin - rayOrigin;

    // Calculate the cross product of the ray direction and axis direction
    glm::vec3 crossDir = glm::cross(normalizedRayDir, normalizedAxisDir);

    // If the cross product is zero, the ray and axis are parallel and do not intersect
    if (glm::length(crossDir) < 1e-2) {
        return glm::vec3(0.0f);
    }

    // Calculate the distance along the ray direction to the intersection point
    float t = glm::dot(glm::cross(originToOrigin, normalizedAxisDir), crossDir) / glm::length2(crossDir);

    // Calculate the intersection point
    glm::vec3 intersectionPoint = rayOrigin + t * normalizedRayDir;

    // Project the intersection point onto the axis
    glm::vec3 projectedIntersectionPoint = axisOrigin + glm::dot(intersectionPoint - axisOrigin, normalizedAxisDir) * normalizedAxisDir;

    return projectedIntersectionPoint;
}

//if we want to make another gizmo that moves two axis at the same time we sould use this (still doesnt work as intended)
glm::vec3 GetRayPlainIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& axisOrigin, const glm::vec3& axisDirection) {
    glm::vec3 normalizedRayDir = glm::normalize(rayDirection);
    glm::vec3 normalizedAxisDir = glm::normalize(axisDirection);

    // Calculate the vector from the ray origin to the axis origin
    glm::vec3 originToOrigin = axisOrigin - rayOrigin;

    // Calculate the projection of the originToOrigin vector onto the axis direction
    float t = glm::dot(originToOrigin, normalizedAxisDir) / glm::dot(normalizedRayDir, normalizedAxisDir);

    // Calculate the intersection point
    glm::vec3 intersectionPoint = rayOrigin + t * normalizedRayDir;

    return intersectionPoint;
}

void Gizmos::MoveObjectAlongAxis(const glm::vec3& rayOrigin, glm::vec3& rayDirection, const glm::vec3& axis) {
   
    Application->input->GetSelectedGameObject()->GetTransform()->SetPosition(GetRayAxisIntersection(rayOrigin, rayDirection, Application->input->GetSelectedGameObject()->GetTransform()->GetPosition(), axis));
	sphere3 = GetRayAxisIntersection(rayOrigin,rayDirection, Application->input->GetSelectedGameObject()->GetTransform()->GetPosition(),axis);

}
