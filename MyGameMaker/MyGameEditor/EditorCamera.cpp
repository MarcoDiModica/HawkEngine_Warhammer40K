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
			transform.Translate(glm::vec3(delta.x, delta.y, 0) * 0.5f * deltaTime);
		}
	}
	else 
	{
		isPanning = false;
	}

	if (Application->input->GetMouseButton(3) == KEY_REPEAT) 
	{

		if (infiniteMouse) 
		{
			glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());

			// Calcular delta del ratón antes de envolver
			glm::dvec2 mouseDelta = currentMousePos - lastMousePos;

			if (currentMousePos.x <= 0) {
				SDL_WarpMouseInWindow(Application->window->windowPtr(), windowWidth - 1, static_cast<int>(currentMousePos.y));
				lastMousePos.x = windowWidth - 1;
				lastMousePos.y = currentMousePos.y;
			}
			else if (currentMousePos.x >= windowWidth - 1) {
				SDL_WarpMouseInWindow(Application->window->windowPtr(), 0, static_cast<int>(currentMousePos.y));
				lastMousePos.x = 0;
				lastMousePos.y = currentMousePos.y;
			}
			else if (currentMousePos.y <= 0) {
				SDL_WarpMouseInWindow(Application->window->windowPtr(), static_cast<int>(currentMousePos.x), windowHeight - 1);
				lastMousePos.x = currentMousePos.x;
				lastMousePos.y = windowHeight - 1;
			}
			else if (currentMousePos.y >= windowHeight - 1) {
				SDL_WarpMouseInWindow(Application->window->windowPtr(), static_cast<int>(currentMousePos.x), 0);
				lastMousePos.x = currentMousePos.x;
				lastMousePos.y = 0;
			}


			// Calcular delta del ratón después de ajustar lastMousePos
			glm::dvec2 delta = currentMousePos - lastMousePos;
	
		}


		glm::dvec2 mousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
		glm::dvec2 delta = mousePos - lastMousePos;
		lastMousePos = mousePos;

		yaw += delta.x * sensitivity * deltaTime;
		pitch -= delta.y * sensitivity * deltaTime;
		if (pitch > MAX_PITCH) pitch = MAX_PITCH;
		if (pitch < -MAX_PITCH) pitch = -MAX_PITCH;

		transform.Rotate(glm::radians(-delta.x * sensitivity * deltaTime), glm::vec3(0, 1, 0));
		transform.Rotate(glm::radians(delta.y * sensitivity * deltaTime), glm::vec3(1, 0, 0));
		transform.AlignToGlobalUp();

	// Get local axes based on current rotation
		glm::dvec3 forward = transform.GetForward();
		glm::dvec3 right = glm::normalize(transform.GetRotation() * glm::dvec3(1, 0, 0));
		glm::dvec3 up = glm::normalize(glm::cross(forward, right)); // Ensure a proper up vector

		// Invert the necessary directions for correct movement
		if (Application->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) transform.Translate(forward * static_cast<double>(speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) transform.Translate(-forward * static_cast<double>(speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) transform.Translate(right * static_cast<double>(speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) transform.Translate(-right * static_cast<double>(speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) transform.Translate(-up * static_cast<double>(speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) transform.Translate(up * static_cast<double>(speed * deltaTime));

	}

	if (Application->input->GetMouseZ() > 0) transform.Translate(glm::vec3(0, 0, zoomSpeed*10 * deltaTime));
	if (Application->input->GetMouseZ() < 0) transform.Translate(glm::vec3(0, 0, -zoomSpeed*10 * deltaTime));

	/*float wheel = Application->input->GetMouseZ();
	if (wheel != 0) {
		glm::vec3 zoomDirection = transform.GetForward();
		transform.Translate(zoomDirection * wheel * zoomSpeed * deltaTime);
	}*/

	if (Application->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !Application->input->GetSelectedGameObjects().empty() && Application->input->GetKey(SDL_SCANCODE_LCTRL) != KEY_REPEAT)
	{
		glm::dvec3 targetPosition = Application->input->GetSelectedGameObjects()[0]->GetTransform()->GetPosition();
		
		transform.LookAt(targetPosition);

		// add ofset
		transform.SetPosition(targetPosition - transform.GetForward() * 7.0 + glm::dvec3(0, 2, 0));
		transform.AlignToGlobalUp();
	}

	if (Application->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && Application->input->GetMouseButton(1) == KEY_REPEAT) 
	{
		glm::dvec2 currentMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
		glm::dvec2 delta = currentMousePos - lastMousePos;
		lastMousePos = currentMousePos;

		// Calculate the right vector manually (camera's right is perpendicular to forward and up)
		glm::dvec3 forward = transform.GetForward();
		glm::dvec3 worldUp = glm::dvec3(0, 1, 0);
		glm::dvec3 right = glm::normalize(glm::cross(worldUp, forward));

		// Calculate yaw (left/right) rotation using world up (0,1,0)
		glm::dquat yawRotation = glm::angleAxis(glm::radians(-delta.x * sensitivity * deltaTime), glm::dvec3(0, 1, 0));

		// Calculate pitch (up/down) rotation using local right vector
		glm::dquat pitchRotation = glm::angleAxis(glm::radians(delta.y * sensitivity * deltaTime), right);

		// Apply the rotations: pitch first, then yaw
		glm::dquat newRotation = yawRotation * transform.GetRotation() * -pitchRotation;
		transform.SetRotationQuat(newRotation);

	}
	else 
	{
		lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
	}
}

void EditorCamera::UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight)
{
	double aspectRatio = windowWidth / windowHeight;

	UpdateAspectRatio(aspectRatio);

	SetFOV(glm::radians(60.0f));
}