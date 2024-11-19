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
#include "ImGuizmo.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"

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

void Gizmos::DrawMovementArrows() 
{
    glm::vec3 position = Application->input->GetSelectedGameObject()->GetTransform()->GetPosition();
    glBegin(GL_LINES);

    // X Axis (Red)
    if (selectedAxis == glm::vec3(1, 0, 0)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x + 1.0f, position.y, position.z);

    // Y Axis (Green)
    if (selectedAxis == glm::vec3(0, 1, 0)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y + 1.0f, position.z);

    // Z Axis (Blue)
    if (selectedAxis == glm::vec3(0, 0, 1)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y, position.z + 1.0f);

    glEnd();

    // Draw arrows
    glBegin(GL_TRIANGLES);

    // X Axis Arrow
    if (selectedAxis == glm::vec3(1, 0, 0)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    glVertex3f(position.x + 1.1f, position.y, position.z);
    glVertex3f(position.x + 1.0f, position.y + 0.05f, position.z);
    glVertex3f(position.x + 1.0f, position.y - 0.05f, position.z);

    // Y Axis Arrow
    if (selectedAxis == glm::vec3(0, 1, 0)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    glVertex3f(position.x, position.y + 1.1f, position.z);
    glVertex3f(position.x + 0.05f, position.y + 1.0f, position.z);
    glVertex3f(position.x - 0.05f, position.y + 1.0f, position.z);

    // Z Axis Arrow
    if (selectedAxis == glm::vec3(0, 0, 1)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    glVertex3f(position.x, position.y, position.z + 1.1f);
    glVertex3f(position.x, position.y + 0.05f, position.z + 1.0f);
    glVertex3f(position.x, position.y - 0.05f, position.z + 1.0f);

    glEnd();
}
 
void Gizmos::DrawAxisPlanesMovement() 
{
    glm::vec3 position = Application->input->GetSelectedGameObject()->GetTransform()->GetPosition();
    glBegin(GL_QUADS);

    // XY Plane 
    if (selectedPlane == glm::vec3(1, 0, 0)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x + 0.2f, position.y, position.z);
    glVertex3f(position.x + 0.2f, position.y + 0.2f, position.z);
    glVertex3f(position.x, position.y + 0.2f, position.z);

    // YZ Plane 
    if (selectedPlane == glm::vec3(0, 0, 1)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y + 0.2f, position.z);
    glVertex3f(position.x, position.y + 0.2f, position.z + 0.2f);
    glVertex3f(position.x, position.y, position.z + 0.2f);

    // XZ Plane 
    if (selectedPlane == glm::vec3(0, 1, 0)) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow if selected
    }
    else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x + 0.2f, position.y, position.z);
    glVertex3f(position.x + 0.2f, position.y, position.z + 0.2f);
    glVertex3f(position.x, position.y, position.z + 0.2f);

    glEnd();
}

//(Fornow) it cant go on the update since we need it inside the drawfunction on main
void Gizmos::DrawGizmos() {
    if (Application->input->GetSelectedGameObject() != nullptr) {
        IntersectAxis(rayStartPos, rayDir, selectedAxis);
		IntersectPlanes(rayStartPos, rayDir, selectedPlane);

        //DrawMovementGizmo
        glDisable(GL_DEPTH_TEST);
		DrawMovementArrows();
        DrawAxisPlanesMovement();
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

        //Detect if one of the plane axis is selected
		//if (IntersectPlanes(rayStartPos, rayDir, selectedPlane) && isAxisSelected == false) {
		//	isDragging = true;
		//	isAxisSelected = true;
		//}
		//else {
		//	isAxisSelected = false;
		//}


    }

    if (Application->input->GetMouseButton(1) == KEY_UP) {
        isDragging = false;
        isAxisSelected = false;
    }

    if (isDragging) {
    
        // Mover el objeto a lo largo del eje seleccionado
		if (selectedAxis != glm::vec3(0, 0, 0))
            MoveObjectAlongAxis(rayStartPos, rayDir, selectedAxis);

		//if (selectedPlane != glm::vec3(0, 0, 0))
		//	MoveObjectAlongPlane(rayStartPos, rayDir, selectedPlane);
    }
}

bool RaysIntersect(const glm::vec3& origin1, const glm::vec3& direction1,
    const glm::vec3& origin2, const glm::vec3& direction2, float tolerance = 1e-3f)
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

bool PlaneIntersect(const glm::vec3& origin, const glm::vec3& direction, 
    const glm::vec3& planeOrigin, const glm::vec3& planeNormal, float PlaneSize ,float tolerance = 1e-2f) {
    glm::vec3 d = glm::normalize(direction);
    glm::vec3 n = glm::normalize(planeNormal);
	glm::vec2 planeSize = glm::vec2(PlaneSize, PlaneSize);

    float dot = glm::dot(d, n);

    if (glm::abs(dot) < tolerance) {
        return false;
    }

    float t = glm::dot(planeOrigin - origin, n) / dot;

    if (t < 0) {
        return false;
    }

    glm::vec3 intersectionPoint = origin + t * d;

    // Check if the intersection point is within the bounds of the plane
    glm::vec3 localPoint = intersectionPoint - planeOrigin;
    glm::vec3 planeRight = glm::normalize(glm::cross(n, glm::vec3(0, 1, 0)));
    glm::vec3 planeUp = glm::normalize(glm::cross(planeRight, n));

    float halfWidth = planeSize.x / 2.0f;
    float halfHeight = planeSize.y / 2.0f;

    float localX = glm::dot(localPoint, planeRight);
    float localY = glm::dot(localPoint, planeUp);

    if (glm::abs(localX) > halfWidth || glm::abs(localY) > halfHeight) {
        return false;
    }

    return true;
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

bool Gizmos::IntersectPlanes(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& outAxis) {
	glm::vec3 position = Application->input->GetSelectedGameObject()->GetTransform()->GetPosition();

	// XY Plane
	if (PlaneIntersect(rayOrigin, rayDirection, position, glm::vec3(0, 0, 1),0.2f) && isAxisSelected == false) {
        selectedPlane = glm::vec3(0, 0, 1);
		return true;
	}

	// YZ Plane
	if (PlaneIntersect(rayOrigin, rayDirection, position, glm::vec3(1, 0, 0), 0.2f) && isAxisSelected == false) {
        selectedPlane = glm::vec3(1, 0, 0);
		return true;
	}

	// XZ Plane
	if (PlaneIntersect(rayOrigin, rayDirection, position, glm::vec3(0, 1, 0), 0.2f) && isAxisSelected == false) {
        selectedPlane = glm::vec3(0, 1, 0);
		return true;
	}

	if (isAxisSelected == false)
        selectedPlane = glm::vec3(0, 0, 0);

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

void Gizmos::MoveObjectAlongPlane(const glm::vec3& rayOrigin, glm::vec3& rayDirection, const glm::vec3& axis) {
	Application->input->GetSelectedGameObject()->GetTransform()->SetPosition(GetRayPlainIntersection(rayOrigin, rayDirection, Application->input->GetSelectedGameObject()->GetTransform()->GetPosition(), axis));
	sphere2 = GetRayPlainIntersection(rayOrigin, rayDirection, Application->input->GetSelectedGameObject()->GetTransform()->GetPosition(), axis);
}