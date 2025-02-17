#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define GLM_ENABLE_EXPERIMENTAL
#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

#include <string>
#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <iostream>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "MyGUI.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "assimp/cimport.h" 
#include "assimp/scene.h" 
#include "assimp/postprocess.h"
#include "../MyGameEngine/Mesh.h"
#include "EditorCamera.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <stack>
#include <IL/ilut.h>
#include "Input.h"
#include "MyGUI.h"
#include <ImGuizmo.h>
#include "UISceneWindow.h"
#include "MyGameEngine/types.h"
#include "MyGameEngine/CameraBase.h"
#include "MyGameEngine/BoundingBox.h"
#include "MyGameEngine/CameraComponent.h"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "./MyScriptingEngine/MonoManager.h"
#include "./MyPhysicsEngine/PhysicsModule.h"


#include "MyGameEngine/LightComponent.h"
#include "MyGameEngine/Shaders.h"
#include "MyGameEngine/Material.h"
#include "MyGameEngine/SceneManager.h"
#include "MyGameEngine/InputEngine.h"
#include "App.h"

//#include <mono/metadata/assembly.h>
//#include <mono/jit/jit.h>

//#include "../MyScriptingEngine/MonoEnvironment.h"


//TODO BALDAN : xcopy mono / place manually in x64/Debug folder

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
static const auto FPS = 120;
static const auto FRAME_DT = 1.0s / FPS;

int numPointLight = 0;
int numDirLight = 0;

std::list<GameObject*> lights;

static EditorCamera* camera = nullptr;

Shaders mainShader;

App* Application = NULL;
SceneManager* SceneManagement = NULL;
InputEngine* InputManagement = NULL;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.2, 0.2, 0.2, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScaled(1.0, (double)WINDOW_SIZE.x/WINDOW_SIZE.y, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
}

static void drawFloorGrid(int size, double step) {
	glColor3f(0.5f, 0.5f, 0.5f); // Gray color

	glBegin(GL_LINES);
	// CHANGE Application->root->currentScene->DebugDrawTree();

	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

// Initializes camera
void configureCamera() {
	glm::dmat4 projectionMatrix = Application->camera->projection();
	glm::dmat4 viewMatrix = Application->camera->view();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(glm::value_ptr(viewMatrix));

	Application->camera->frustum.Update(projectionMatrix * viewMatrix);
}

void configureGameCamera()
{
	if (Application->root->mainCamera != nullptr)
	{
		glm::dmat4 projectionMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->projection();
		glm::dmat4 viewMatrix = Application->root->mainCamera->GetComponent<CameraComponent>()->view();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(glm::value_ptr(projectionMatrix));

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(glm::value_ptr(viewMatrix));

		Application->root->mainCamera->GetComponent<CameraComponent>()->frustum.Update(projectionMatrix * viewMatrix);
	}
}

void drawFrustum(const CameraBase& camera)
{
	//const auto& frustum = camera.frustum;

	//glBegin(GL_LINES);
	//for (int i = 0; i < 4; i++) {
	//	glVertex3fv(glm::value_ptr(frustum.vertices[i]));
	//	glVertex3fv(glm::value_ptr(frustum.vertices[(i + 1) % 4]));

	//	glVertex3fv(glm::value_ptr(frustum.vertices[i + 4]));
	//	glVertex3fv(glm::value_ptr(frustum.vertices[(i + 1) % 4 + 4]));

	//	glVertex3fv(glm::value_ptr(frustum.vertices[i]));
	//	glVertex3fv(glm::value_ptr(frustum.vertices[i + 4]));
	//}
	//glEnd();
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

void ObjectToEditorCamera() 
{
	if (!Application->input->GetSelectedGameObjects().empty() && Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && Application->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && Application->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Application->input->GetSelectedGameObjects().at(0)->GetTransform()->SetPosition(Application->camera->GetTransform().GetPosition());
		Application->input->GetSelectedGameObjects().at(0)->GetTransform()->SetRotationQuat(Application->camera->GetTransform().GetRotation());
	}
}

void MousePickingCheck(std::vector<GameObject*> objects)
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
			if (objects[i]->HasComponent<MeshRenderer>()) 
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

void RenderOutline(GameObject* object) {
	if (!object->isSelected || !object->HasComponent<MeshRenderer>()) return;
	
	glm::mat4 modelMatrix = object->GetTransform()->GetMatrix();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); 
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-5.0f, -5.0f);
	
	glColor3f(1.0f, 0.5f, 0.0f); // Red outline

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(modelMatrix));
	object->GetComponent<MeshRenderer>()->Render();
	glPopMatrix();


	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glColor3f(1.0f, 1.0f, 1.0f);
}

static void display_func() {
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);
	glViewport(0, 0, Application->window->width(), Application->window->height());
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	configureCamera();

	drawFloorGrid(128, 4);

	std::vector<GameObject*> objects;	

	//no me gusta como esta hecho pero me encuentro fatal pensar de como cambiarlo ma�ana
	for (size_t i = 0; i < Application->root->GetActiveScene()->children().size(); ++i)
	{
		GameObject* object = Application->root->GetActiveScene()->children()[i].get();

		if (object->HasComponent<LightComponent>()) {
			auto it = std::find(lights.begin(), lights.end(), object);
			if (it == lights.end()) {
				lights.push_back(object);
			}
		}
	}

	for (size_t i = 0; i < Application->root->GetActiveScene()->children().size(); ++i)
	{
		GameObject* object = Application->root->GetActiveScene()->children()[i].get();
		
		objects.push_back(object);

		RenderOutline(object);

		object->ShaderUniforms(Application->camera->view(), Application->camera->projection(), Application->camera->GetTransform().GetPosition(), lights,mainShader);
		
		object->Update(static_cast<float>(Application->GetDt()));

	
		for (size_t j = 0; j < object->GetChildren().size(); ++j)
		{
			GameObject* child = object->GetChildren()[j].get();
			objects.push_back(child);
			RenderOutline(child);
			child->ShaderUniforms(Application->camera->view(), Application->camera->projection(), Application->camera->GetTransform().GetPosition(), lights, mainShader);
		
		}
	}

	Application->physicsModule->Update(Application->GetDt());
	MousePickingCheck(objects);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void display_func2() {

	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fboCamera);

	glViewport(0, 0, Application->window->width(), Application->window->height());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	configureGameCamera();
	//drawFrustum(*Application->root->mainCamera->GetComponent<CameraComponent>());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EditorRenderer(MyGUI* gui) {

	if (Application->window->IsOpen()) {

		const auto t0 = hrclock::now();
		display_func();
		display_func2();
		gui->Render();

		/*move_camera();*/
		Application->window->SwapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}
}

int main(int argc, char** argv) {

	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT) 
	{
		switch (state)
		{

		case CREATE:

			// The application is created
			Application = new App();
			Application->physicsModule->Awake();
			Application->physicsModule->Start();
			// MonoEnvironment* mono = new MonoEnvironment();
			//	MonoEnvironment* monoEnvironmanet = new MonoEnvironment();
			MonoManager::Init();

			//initialize devil
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

			if (Application->Awake()) { state = START; }
			else { printf("Failed on Awake"); state = FAIL; }
			break;

		case START:

			if (Application->Start()) { state = LOOP; }
			else { state = FAIL; printf("Failed on START"); }
			break;

		case LOOP:

			EditorRenderer(Application->gui);
			UndoRedo();
			ObjectToEditorCamera();
			if (!Application->Update()) { state = FREE; }
			break;

		case FREE:

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

	MonoManager::Shutdown();

	return result;
}