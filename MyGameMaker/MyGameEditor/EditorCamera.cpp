#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>

#include "EditorCamera.h"
#include "App.h"
#include "Input.h"
#include "MyGameEngine/TransformComponent.h"
#include "MyAudioEngine/AudioListener.h"
#include "MyGameEngine/GameObject.h"
#include "MyWindow.h"
#include "MyGUI.h"
#include "UISceneWindow.h"

EditorCamera::EditorCamera(App* app) : Module(app), CameraBase(), transform(Transform_Component(nullptr))
{
	listenerObject = new GameObject("EditorCameraObject");
	listenerObject->AddComponent<AudioListener>();

	transform.SetPosition(glm::dvec3(0, 7, 15));

	glm::dvec3 forward = transform.GetForward();
	pitch = glm::degrees(asin(-forward.y));
	yaw = glm::degrees(atan2(-forward.x, -forward.z));
	orbitYaw = yaw;
	orbitPitch = pitch;
}


bool EditorCamera::Awake()
{
	return true;
}

bool EditorCamera::Start()
{
	UpdateCameraView(Application->window->width(), Application->window->height(), Application->gui->camSize.x, Application->gui->camSize.y);

	return true;
}

bool EditorCamera::FixedUpdate()
{
	return true;
}

bool EditorCamera::Update(double dt)
{
	if (Application->gui->UISceneWindowPanel->IsMouseOverWindow())
	{
		move_camera(cameraSpeed, static_cast<float>(dt));
	}
	
	// Sync listener object position with camera
	if (listenerObject) {
		listenerObject->GetTransform()->SetPosition(transform.GetPosition());
		// For now, let's skip rotation sync until we debug the rotation issue
	}

	return true;
}

bool EditorCamera::PostUpdate()
{
	return true;
}

bool EditorCamera::CleanUp()
{
	if (listenerObject) {
		delete listenerObject;
		listenerObject = nullptr;
	}
	return true;
}

void EditorCamera::move_camera(float speed, float deltaTime)
{
	static bool isPanning = false;
	static bool isRotating = false;
	static bool isOrbiting = false;
	int windowWidth = Application->window->width();
	int windowHeight = Application->window->height();

	if (Application->input->GetMouseButton(2) == KEY_REPEAT)
	{
		if (!isPanning)
		{
			lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
			isPanning = true;
		}
		else
		{
			glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
			glm::dvec2 delta = currentMousePos - lastMousePos;
			lastMousePos = currentMousePos;

			glm::dvec3 right = glm::normalize(transform.GetRotation() * glm::dvec3(-1, 0, 0));
			glm::dvec3 up = glm::normalize(transform.GetRotation() * glm::dvec3(0, -1, 0));

			transform.Translate(-right * delta.x * static_cast<double>(deltaTime) - up * delta.y * static_cast<double>(deltaTime));
		}
	}
	else if (Application->input->GetMouseButton(3) == KEY_REPEAT)
	{
		if (!isRotating)
		{
			lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());

			if (isOrbiting) {
				yaw = orbitYaw;
				pitch = orbitPitch;
			}

			if (yaw == 0 && pitch == 0) {
				glm::dvec3 forward = transform.GetForward();
				pitch = glm::degrees(asin(-forward.y));
				yaw = glm::degrees(atan2(-forward.x, -forward.z));
			}

			isRotating = true;
		}
		else
		{
			glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
			glm::dvec2 delta = currentMousePos - lastMousePos;
			lastMousePos = currentMousePos;

			yaw += delta.x * sensitivity * deltaTime;
			pitch = glm::clamp(pitch - delta.y * sensitivity * deltaTime, -MAX_PITCH, MAX_PITCH);

			glm::dquat quatYaw = glm::angleAxis(glm::radians(yaw), glm::dvec3(0, -1, 0));
			glm::dquat quatPitch = glm::angleAxis(glm::radians(pitch), glm::dvec3(-1, 0, 0));

			glm::dquat newRotation = quatYaw * quatPitch;
			transform.SetRotationQuat(newRotation);

			glm::dvec3 forward = transform.GetForward();
			glm::dvec3 right = glm::normalize(glm::cross(glm::dvec3(0, 1, 0), forward));
			glm::dvec3 up = glm::normalize(glm::cross(forward, right));

			if (Application->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) transform.Translate(forward * static_cast<double>(speed * deltaTime));
			if (Application->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) transform.Translate(-forward * static_cast<double>(speed * deltaTime));
			if (Application->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) transform.Translate(right * static_cast<double>(speed * deltaTime));
			if (Application->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) transform.Translate(-right * static_cast<double>(speed * deltaTime));
			if (Application->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) transform.Translate(-up * static_cast<double>(speed * deltaTime));
			if (Application->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) transform.Translate(up * static_cast<double>(speed * deltaTime));
		}
	}
	else if (Application->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && Application->input->GetMouseButton(1) == KEY_REPEAT)
	{		
		if (!isOrbiting)
		{
			lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());

			if (isRotating) {
				orbitYaw = yaw;
				orbitPitch = pitch;
			}

			isOrbiting = true;
		}
		else
		{
			glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
			glm::dvec2 delta = currentMousePos - lastMousePos;
			lastMousePos = currentMousePos;

			glm::dvec3 pivotPoint;
			double orbitDistance = 7.0;

			if (!Application->input->GetSelectedGameObjects().empty()) {
				GameObject* selectedGO = Application->input->GetSelectedGameObjects()[0];
				pivotPoint = selectedGO->GetTransform()->GetPosition();
				orbitDistance = glm::length(transform.GetPosition() - pivotPoint);
			}
			else {
				pivotPoint = transform.GetPosition() + transform.GetForward() * orbitDistance;
			}

			orbitYaw += -delta.x * sensitivity * deltaTime;
			orbitPitch = glm::clamp(orbitPitch - delta.y * sensitivity * deltaTime, -MAX_PITCH, MAX_PITCH);

			glm::dquat quatYaw = glm::angleAxis(glm::radians(orbitYaw), glm::dvec3(0, 1, 0));
			glm::dquat quatPitch = glm::angleAxis(glm::radians(orbitPitch), glm::dvec3(1, 0, 0));

			glm::dquat newRotation = quatYaw * quatPitch;
			transform.SetRotationQuat(newRotation);

			glm::dvec3 offset = -transform.GetForward() * orbitDistance;
			transform.SetPosition(pivotPoint + offset);
		}
	}
	else
	{
		isOrbiting = false;
		isRotating = false;
		isPanning = false;
		lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
	}

	//scroll wheel
	if (Application->input->GetMouseZ() > 0) {
		vec3 direction = transform.GetForward();
		transform.Translate(direction * (double)zoomSpeed * 10.0 * (double)deltaTime);
	}
	else if (Application->input->GetMouseZ() < 0) {
		vec3 direction = -transform.GetForward();
		transform.Translate(direction * (double)zoomSpeed * 10.0 * (double)deltaTime);
	}

	//focus on selected object
	//f key
	if (Application->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !Application->input->GetSelectedGameObjects().empty() && Application->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT)
	{
		glm::dvec3 targetPosition = Application->input->GetSelectedGameObjects()[0]->GetTransform()->GetPosition();

		glm::dquat currentRotation = transform.GetRotation();

		glm::dvec3 forward = transform.GetForward();

		const double focusDistance = 7.0;
		const double heightOffset = 1.0;

		transform.SetPosition(targetPosition - forward * focusDistance + glm::dvec3(0, heightOffset, 0));
	}
}

void EditorCamera::UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight)
{
	double aspectRatio = windowWidth / windowHeight;

	UpdateAspectRatio(aspectRatio);

	SetFOV(glm::radians(60.0f));
}