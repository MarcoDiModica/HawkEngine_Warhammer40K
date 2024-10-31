#include "Camera.h"
#include "App.h"
#include "imgui.h"
#include "Input.h"
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>

Camera::Camera(App* app) : Module(app) { ; }

glm::dmat4 Camera::projection() const {
	return glm::perspective(fov, aspect, zNear, zFar);
}

glm::dmat4 Camera::view() const {
	return glm::lookAt(_transform.pos(), _transform.pos() + _transform.fwd(), _transform.up());
}

bool Camera::FixedUpdate() {

	move_camera();

	return true;
}

void Camera::move_camera()
{
	
}



bool Camera::Start(){ return true;}

bool Camera::Awake() { return true; }

bool Camera::Update(double dt) {


	float speed = 0.1f;
	static bool isPanning = false;
	static glm::dvec2 lastMousePos = glm::dvec2(0.0, 0.0);

	if (Application->input->GetMouseButton(2) == KEY_REPEAT)
	{
		if (!isPanning) {
			lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
			isPanning = true;
		}
		else {
			glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
			glm::dvec2 delta = currentMousePos - lastMousePos;
			lastMousePos = currentMousePos;
			transform().translate(glm::vec3(delta.x, delta.y, 0) * 0.01f);
		}
	}
	else {
		isPanning = false;
	}

	if (Application->input->GetMouseButton(3) == KEY_REPEAT) {

		if (Application->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) 	speed = 0.3f;

		if (Application->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) transform().translate(glm::vec3(0, 0, speed));
		if (Application->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) transform().translate(glm::vec3(0, 0, -speed));
		if (Application->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) transform().translate(glm::vec3(speed, 0, 0));
		if (Application->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) transform().translate(glm::vec3(-speed, 0, 0));
		if (Application->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) transform().translate(glm::vec3(0, -speed, 0));
		if (Application->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) transform().translate(glm::vec3(0, speed, 0));

		//FreeLook
		glm::dvec2 mousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
		glm::dvec2 delta = mousePos - lastMousePos;
		lastMousePos = mousePos;

		const double sensitivity = 0.1;
		static double yaw = 0.0;
		static double pitch = 0.0;
		const double MAX_PITCH = glm::radians(89.0);

		yaw += delta.x * sensitivity;
		pitch -= delta.y * sensitivity;

		if (pitch > MAX_PITCH) pitch = MAX_PITCH;
		if (pitch < -MAX_PITCH) pitch = -MAX_PITCH;

		transform().rotate(glm::radians(-delta.x * sensitivity), glm::vec3(0, 1, 0));
		transform().rotate(glm::radians(delta.y * sensitivity), glm::vec3(1, 0, 0));
		transform().alignToGlobalUp();
	}
	if (Application->input->GetMouseZ() > 0) transform().translate(glm::vec3(0, 0, 0.5));
	if (Application->input->GetMouseZ() < 0) transform().translate(glm::vec3(0, 0, -0.5));

	if (Application->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && Application->input->GetSelectedGameObject() != NULL) transform().LookAt(Application->input->GetSelectedGameObject()->GetTransform()->GetPosition());



	if (Application->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && Application->input->GetMouseButton(1) == KEY_REPEAT && Application->input->GetSelectedGameObject() != NULL)
	{
		glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
		glm::dvec2 delta = currentMousePos - lastMousePos;
		lastMousePos = currentMousePos;

		angleHorizontal += delta.x * orbitSpeed;
		angleVertical += delta.y * orbitSpeed;

		if (angleVertical < -glm::half_pi<double>()) angleVertical = -glm::half_pi<double>();
		if (angleVertical > glm::half_pi<double>()) angleVertical = glm::half_pi<double>();

		vec3 targetPosition = Application->input->GetSelectedGameObject()->GetTransform()->GetPosition();

		transform().pos().x = targetPosition.x + orbitRadius * cos(angleVertical) * cos(angleHorizontal);
		transform().pos().y = targetPosition.y + orbitRadius * sin(angleVertical);
		transform().pos().z = targetPosition.z + orbitRadius * cos(angleVertical) * sin(angleHorizontal);

		transform().LookAt(targetPosition);
		transform().alignToGlobalUp();
	}
	else
	{
		lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
	}


	return true;
}