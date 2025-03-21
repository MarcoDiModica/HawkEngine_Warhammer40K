#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#else
#pragma comment(linker, "/SUBSYSTEM:console /ENTRY:mainCRTStartup")
#endif

#define GLM_ENABLE_EXPERIMENTAL
#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

#include <string>
#include <chrono>
#include <thread>
#include <exception>
#include <iostream>
#include <stack>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#ifdef min
#undef min
#endif
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

#include "imgui.h"
#include <ImGuizmo.h>

#include "MyWindow.h"
#include "MyGUI.h"
#include "EditorCamera.h"
#include "Input.h"
#include "UISceneWindow.h"
#include "App.h"
#include "../MyGameEngine/Mesh.h"
#include "../MyGameEngine/types.h"
#include "../MyGameEngine/CameraBase.h"
#include "../MyGameEngine/BoundingBox.h"
#include "../MyGameEngine/CameraComponent.h"
#include "../MyGameEngine/GameObject.h"
#include "../MyGameEngine/TransformComponent.h"
#include "../MyGameEngine/MeshRendererComponent.h"
#include "../MyGameEngine/LightComponent.h"
#include "../MyGameEngine/Shaders.h"
#include "../MyGameEngine/Material.h"
#include "../MyGameEngine/SceneManager.h"
#include "../MyGameEngine/InputEngine.h"
#include "./MyScriptingEngine/MonoManager.h"
#include "./MyPhysicsEngine/PhysicsModule.h"
#include "../MyUIEngine/UICanvasComponent.h"

#include "MyAudioEngine/SoundComponent.h"
#include "MyGameEngine/ShaderManager.h"
#include "MyParticlesEngine/ParticleFX.h"

using namespace std;

enum MainState
{
	CREATE,
	AWAKE,
	START,
	LOOP,
	FREE,
	FAIL,
	EXIT
};

GLuint textureID;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(1280, 720);
static const auto FPS = 240;
static const auto FRAME_DT = 1.0s / FPS;

int numPointLight = 0;
int numDirLight = 0;

std::list<GameObject*> lights;

static EditorCamera* camera = nullptr;

App* Application = nullptr;
SceneManager* SceneManagement = nullptr;
InputEngine* InputManagement = nullptr;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScaled(1.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 1.0);

	glMatrixMode(GL_MODELVIEW);
}

static void drawFloorGrid(int size, double step) {
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const glm::vec2 cameraPos2D(Application->camera->GetTransform().GetPosition().x,
		Application->camera->GetTransform().GetPosition().z);

	const float fadeRadius = size * 0.8f;
	const int segmentCount = 8;

	auto calculateFade = [&](const glm::vec2& pos) {
		float distance = glm::length(cameraPos2D - pos);
		return glm::clamp(pow(1.0f - (distance / fadeRadius), 0.75f), 0.0f, 1.0f);
		};

	auto drawSegmentedLine = [&](double x1, double z1, double x2, double z2, float baseAlpha, const glm::vec3& color) {
		for (int i = 0; i < segmentCount; i++) {
			float t1 = static_cast<float>(i) / segmentCount;
			float t2 = static_cast<float>(i + 1) / segmentCount;

			glm::vec2 pos1(glm::mix(x1, x2, t1), glm::mix(z1, z2, t1));
			glm::vec2 pos2(glm::mix(x1, x2, t2), glm::mix(z1, z2, t2));

			float fade1 = calculateFade(pos1);
			float fade2 = calculateFade(pos2);

			glColor4f(color.r, color.g, color.b, baseAlpha * fade1);
			glVertex3d(pos1.x, 0, pos1.y);
			glColor4f(color.r, color.g, color.b, baseAlpha * fade2);
			glVertex3d(pos2.x, 0, pos2.y);
		}
		};

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glm::vec3 gridColor(0.5f, 0.5f, 0.5f);
		drawSegmentedLine(i, -size, i, size, 0.4f, gridColor);
		drawSegmentedLine(-size, i, size, i, 0.4f, gridColor);
	}
	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step * 10) {
		glm::vec3 mainGridColor(0.6f, 0.6f, 0.6f);
		drawSegmentedLine(i, -size, i, size, 0.6f, mainGridColor);
		drawSegmentedLine(-size, i, size, i, 0.6f, mainGridColor);
	}
	glEnd();

	glLineWidth(2.5f);
	glBegin(GL_LINES);
	drawSegmentedLine(-size, 0, size, 0, 0.9f, glm::vec3(0.9f, 0.2f, 0.2f));
	drawSegmentedLine(0, -size, 0, size, 0.9f, glm::vec3(0.2f, 0.2f, 0.9f));
	glEnd();

	glPopAttrib();
	glColor3f(1.0f, 1.0f, 1.0f);
}

static void configureCamera() {
	glm::dmat4 projectionMatrix = Application->camera->projection();
	glm::dmat4 viewMatrix = Application->camera->view();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(glm::value_ptr(viewMatrix));

	Application->camera->frustum.Update(projectionMatrix * viewMatrix);
}

static void RenderObjectAndChildren(std::shared_ptr<GameObject> object) {
	if (object->HasComponent<MeshRenderer>()) {
		GLint lastProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

		glUseProgram(0);
		for (GLenum i = 0; i < 5; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glActiveTexture(GL_TEXTURE0);

		object->GetComponent<MeshRenderer>()->RenderMainCamera();

		glUseProgram(0);
		for (GLenum i = 0; i < 5; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glActiveTexture(GL_TEXTURE0);

		if (lastProgram > 0) {
			glUseProgram(lastProgram);
		}
	}

	if (object->HasComponent<UICanvasComponent>()) {
		object->GetComponent<UICanvasComponent>()->Update(Application->GetDt());
	}

	if (object->HasComponent<ParticleFX>()) {
		object->GetComponent<ParticleFX>()->RenderGameView();
	}

	for (const auto& child : object->GetChildren()) {
		if (child->IsActive()) 
		{
			RenderObjectAndChildren(child);
		}
	}
}

static void RenderGameView() {
	if (Application->root->mainCamera == nullptr) {
		return;
	}

	CameraComponent* gameCamera = Application->root->mainCamera->GetComponent<CameraComponent>();
	if (!gameCamera) {
		return;
	}

	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);

	GLint lastFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFBO);

	GLint lastVP[4];
	glGetIntegerv(GL_VIEWPORT, lastVP);

	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboGame);
	glViewport(0, 0, Application->window->width(), Application->window->height());

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glm::dmat4 projectionMatrix = gameCamera->projection();
	glm::dmat4 viewMatrix = gameCamera->view();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixd(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixd(glm::value_ptr(viewMatrix));

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	for (auto& object : Application->root->GetActiveScene()->children()) 
	{
		if (object->IsActive())
		{
			RenderObjectAndChildren(object);
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (GLenum i = 0; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);
	glViewport(lastVP[0], lastVP[1], lastVP[2], lastVP[3]);

	if (lastProgram > 0) {
		glUseProgram(lastProgram);
	}

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
}

//WIP Undo/Redo for transform actions
#pragma region CTRZ+CTRLY

struct TransformState {
	glm::mat4 transform;
	GameObject* gameObject;
};

std::stack<TransformState> undoStack;
std::stack<TransformState> redoStack;


void SaveState(GameObject* gameObject, const glm::mat4& currentTransform) {
	undoStack.push({ currentTransform, gameObject });

	while (!redoStack.empty()) {
		redoStack.pop();
	}
}

void Undo() {
	if (!undoStack.empty()) {
		TransformState previousState = undoStack.top();
		undoStack.pop();

		// Save the current state of the object for redo
		redoStack.push({ previousState.gameObject->GetTransform()->GetMatrix(), previousState.gameObject });

		// Apply the previous transform to the object
		previousState.gameObject->GetTransform()->SetMatrix(previousState.transform);
	}
}

void Redo() {
	if (!redoStack.empty()) {
		TransformState nextState = redoStack.top();
		redoStack.pop();

		// Save the current state of the object for undo
		undoStack.push({ nextState.gameObject->GetTransform()->GetMatrix(), nextState.gameObject });

		// Apply the next transform to the object
		nextState.gameObject->GetTransform()->SetMatrix(nextState.transform);
	}
}

void UndoRedo()
{
	static bool wasUsingGizmo = false;

	if (ImGuizmo::IsUsing()) {
		wasUsingGizmo = true;
	}
	else if (wasUsingGizmo) {
		SaveState(Application->input->GetSelectedGameObjects().at(0), Application->input->GetSelectedGameObjects().at(0)->GetTransform()->GetMatrix());
		wasUsingGizmo = false;
	}

	// Handle undo/redo input
	if (Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && Application->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
		Undo();
	}

	if (Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && Application->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) {
		Redo();
	}

}

#pragma endregion

static void ObjectToEditorCamera() 
{
	if (!Application->input->GetSelectedGameObjects().empty() && Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && Application->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && Application->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Application->input->GetSelectedGameObjects().at(0)->GetTransform()->SetPosition(Application->camera->GetTransform().GetPosition());
		Application->input->GetSelectedGameObjects().at(0)->GetTransform()->SetRotationQuat(Application->camera->GetTransform().GetRotation());
	}
}

static void MousePickingCheck(std::vector<GameObject*> objects)
{	
	glm::vec3 rayOrigin = glm::vec3(glm::inverse(Application->camera->view()) * glm::vec4(0, 0, 0, 1));
	glm::vec3 rayDirection = Application->input->getMousePickRay();
	GameObject* selectedObject = nullptr;
	bool selecting = false;
	float distance = 0.0f;
	float closestDistance = 0.0f;
	
	if (Application->input->GetMouseButton(1) == KEY_DOWN && Application->gui->UISceneWindowPanel->isFoucused) 
	{

		if (ImGuizmo::IsOver()) {
			return;
		}

		selecting = true;
		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i]->HasComponent<MeshRenderer>() && objects[i]->IsActive())
			{
				BoundingBox bbox = objects[i]->GetComponent<MeshRenderer>()->GetMesh()->boundingBox();

				bbox = objects[i]->GetTransform()->GetMatrix() * bbox;
				glm::vec3 collisionPoint;
				if (Application->gui->UISceneWindowPanel->CheckRayAABBCollision(rayOrigin, rayDirection, bbox, collisionPoint))
				{
                    distance = glm::distance(rayOrigin, collisionPoint);
					if (distance < closestDistance || closestDistance == 0.0f)
					{
						closestDistance = distance;
						selectedObject = objects[i];
					}
				}
			}
		}
	}

	if (selectedObject != nullptr && selecting == true)
	{
		Application->input->ClearSelection();
		Application->input->SetDraggedGameObject(selectedObject);
		Application->input->AddToSelection(selectedObject);
	}
}

static void RenderOutline(GameObject* object) {
	if (!object->isSelected || !object->HasComponent<MeshRenderer>()) return;
	
	glm::mat4 modelMatrix = object->GetTransform()->GetMatrix();

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-5.0f, -5.0f);
	
	glColor3f(1.0f, 0.5f, 0.0f); // Red outline

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(modelMatrix));
	object->GetComponent<MeshRenderer>()->Render();
	glPopMatrix();

	glDisable(GL_POLYGON_OFFSET_FILL);

	glColor3f(1.0f, 1.0f, 1.0f);
}

static void RenderEditor() {
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);
	glViewport(0, 0, (int)Application->gui->camSize.x, (int)Application->gui->camSize.y);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	configureCamera();
	drawFloorGrid(256, 4);
	std::vector<GameObject*> objects;
	for (size_t i = 0; i < Application->root->GetActiveScene()->children().size(); ++i) {
		GameObject* object = Application->root->GetActiveScene()->children()[i].get();
		
		objects.push_back(object);

		for (const auto& j : object->GetChildren()) {
			GameObject* child = j.get();
			objects.push_back(child);
			//RenderOutline(child);
		}

		if (object->IsActive()) 
		{
			object->Update(static_cast<float>(Application->GetDt()));

			if (object->HasComponent<LightComponent>()) {
				auto& lights = Application->root->GetActiveScene()->_lights;
				auto it = std::find(lights.begin(), lights.end(), object->shared_from_this());
				if (it == lights.end()) {
					lights.push_back(object->shared_from_this());
				}
			}
		}
	}

	Application->physicsModule->Update(Application->GetDt());
	MousePickingCheck(objects);
}

static void EditorRenderer(MyGUI* gui) {
	if (Application->window->IsOpen()) {
		const auto t0 = hrclock::now();

		RenderEditor();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui->Render();

		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		//if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}
}

static double counterUsingDeltaTime = 0.0;
static double counterUsingChrono = 0.0;
static hrclock::time_point startTime = hrclock::now();

static void PrintCounters() {
	//// Counter using delta time
	//counterUsingDeltaTime += Application->GetDt();
	//std::cout << "Counter using delta time: " << std::fixed << std::setprecision(2) << counterUsingDeltaTime << " seconds" << std::endl;

	//// Counter using chrono
	//auto currentTime = hrclock::now();
	//std::chrono::duration<double> elapsedTime = currentTime - startTime;
	//counterUsingChrono = elapsedTime.count();
	//std::cout << "Counter using chrono: " << std::fixed << std::setprecision(2) << counterUsingChrono << " seconds" << std::endl;

	std::cout << "Fps;  %d" << Application->GetFps() << std::endl;
}

int main(int argc, char** argv) {

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT) 
	{
		switch (state)
		{

		case CREATE:

			Application = new App();
			
			MonoManager::GetInstance().Initialize();
			SoundComponent::InitSharedAudioEngine();

			ilInit();
			iluInit();
			ilutInit();

			init_openGL();

			if (Application) {	
				state = AWAKE;
			}
			else { state = FAIL; printf("Failed on Create"); }
			break;


		case AWAKE:

			Application->physicsModule->Awake();
			if (Application->Awake()) { state = START; }
			else { printf("Failed on Awake"); state = FAIL; }
			break;

		case START:

			Application->physicsModule->Start();
			if (Application->Start()) { state = LOOP; }
			else { state = FAIL; printf("Failed on START"); }
			break;

		case LOOP:

			EditorRenderer(Application->gui);
			
			RenderGameView(); 
			PrintCounters();
			//Application->gui->Render();
			Application->window->SwapBuffers();
            Application->AddLog(LogType::LOG_INFO, std::to_string(Application->GetDt()).c_str());
			UndoRedo();
			ObjectToEditorCamera();
		
		
			if (!Application->Update()) { state = FREE; }
			break;

		case FREE:

			MonoManager::GetInstance().Shutdown();
			ShaderManager::GetInstance().Cleanup();

			if (Application->CleanUP()) {
				state = EXIT;
				result = EXIT_SUCCESS;
			}
			else { state = FAIL; printf("Failed on FREE"); }
			state = EXIT;
			break;

		case FAIL:

			state = EXIT;
			result = EXIT_FAILURE;
			break;

		case EXIT:
			break;
		}
	}

	return result;
}