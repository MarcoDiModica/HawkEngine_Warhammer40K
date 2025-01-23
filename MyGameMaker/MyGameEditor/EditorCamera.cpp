#include "EditorCamera.h"

#include "App.h"
#include "Input.h"
#include "MyGameEngine/TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include <SDL2/SDL.h>
#include "MyWindow.h"
#include "MyGUI.h"
#include "UISceneWindow.h"

EditorCamera::EditorCamera(App* app) : Module(app), CameraBase(), transform(Transform_Component(nullptr))
{
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
	move_camera(cameraSpeed, static_cast<float>(dt));

	return true;
}

bool EditorCamera::PostUpdate()
{
	return true;
}

bool EditorCamera::CleanUp()
{
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
			transform.Translate(glm::vec3(delta.x, delta.y, 0) * 0.01f * deltaTime);
		}
	}
	else 
	{
		isPanning = false;
	}

	if (Application->input->GetMouseButton(3) == KEY_REPEAT) 
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
		transform.Rotate(glm::radians(-delta.x * sensitivity * deltaTime), glm::vec3(0, 1, 0));
		transform.Rotate(glm::radians(delta.y * sensitivity * deltaTime), glm::vec3(1, 0, 0));
		transform.AlignToGlobalUp();

		if (Application->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed *= 1.5f;
		if (Application->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) transform.Translate(glm::vec3(0, 0, speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) transform.Translate(glm::vec3(0, 0, -speed * deltaTime));
		if (Application->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) transform.Translate(glm::vec3(speed * deltaTime, 0, 0));
		if (Application->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) transform.Translate(glm::vec3(-speed * deltaTime, 0, 0));
		if (Application->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) transform.Translate(glm::vec3(0, -speed * deltaTime, 0));
		if (Application->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) transform.Translate(glm::vec3(0, speed * deltaTime, 0));

	}

	if (Application->input->GetMouseZ() > 0) transform.Translate(glm::vec3(0, 0, zoomSpeed * deltaTime));
	if (Application->input->GetMouseZ() < 0) transform.Translate(glm::vec3(0, 0, -zoomSpeed * deltaTime));

	/*float wheel = Application->input->GetMouseZ();
	if (wheel != 0) {
		glm::vec3 zoomDirection = transform.GetForward();
		transform.Translate(zoomDirection * wheel * zoomSpeed * deltaTime);
	}*/

	if (Application->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !Application->input->GetSelectedGameObjects().empty()) 
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

		transform.Rotate(glm::radians(-delta.y * 0.1), glm::dvec3(1, 0, 0));
		transform.Rotate(glm::radians(-delta.x * 0.1), glm::dvec3(0, 1, 0));
		transform.AlignToGlobalUp();
	}
	else 
	{
		lastMousePos = glm::dvec2(Application->input->GetMouseX(), Application->input->GetMouseY());
	}
}

void EditorCamera::UpdateCameraView(double windowWidth, double windowHeight, double imageWidth, double imageHeight)
{
	double windowAspect = windowWidth / windowHeight;
	double imageAspect = imageWidth / imageHeight;

	UpdateAspectRatio(windowAspect);

	if (windowAspect > imageAspect) 
	{
		SetFOV(2.0f * atan(tan(glm::radians(60.0f) / 2.0f) * static_cast<float>((imageAspect / windowAspect))));
	}
	else 
	{
		SetFOV(glm::radians(60.0f));
	}
}