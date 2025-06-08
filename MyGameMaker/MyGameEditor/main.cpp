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
#include "UIGameView.h"
#include "External/Optick/include/optick.h"

#include "MyGameEngine/ShaderManager.h"
#include "MyParticlesEngine/ParticleFX.h"
#include "SDL2/SDL_timer.h"
#include "MyAudioEngine/AudioManager.h"

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
	glDepthRange(0.0, 1.0);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.08f, 0.08f, 0.12f, 1.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (GLEW_EXT_texture_filter_anisotropic) {
		GLfloat maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	}

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

#ifdef PROFILE
	OPTICK_EVENT();
#endif // PROFILE

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
	glViewport(0, 0, 1280, 720);

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

#pragma region UNDO_REDO

const int MAX_UNDO_STATES = 100; 

bool MatricesAreEqual(const glm::dmat4& a, const glm::dmat4& b, double epsilon = 0.0000001) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (std::abs(a[i][j] - b[i][j]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

struct TransformState {
	glm::dmat4 transform;
	GameObject* gameObject;
	std::string objectName; 
};

struct TransformCommand {
	std::vector<TransformState> states;
	std::string description;
	Uint32 timestamp;
};

std::stack<TransformCommand> undoStack;
std::stack<TransformCommand> redoStack;
bool wasUsingGizmo = false;
bool transformChanged = false;
std::map<GameObject*, glm::dmat4> initialTransforms;

void BeginTransformAction() {
	if (Application->input->GetSelectedGameObjects().empty()) return;

	initialTransforms.clear();
	for (auto* gameObject : Application->input->GetSelectedGameObjects()) {
		if (gameObject && gameObject->GetTransform()) {
			initialTransforms[gameObject] = gameObject->GetTransform()->GetMatrix();
		}
	}

	transformChanged = false;
}

bool HasTransformationChanged() {
	if (initialTransforms.empty()) return false;

	for (const auto& pair : initialTransforms) {
		if (pair.first && pair.first->GetTransform()) {
			if (!MatricesAreEqual(pair.second, pair.first->GetTransform()->GetMatrix())) {
				return true;
			}
		}
	}
	return false;
}

void CommitTransformAction(const std::string& description = "Transform") {
	if (Application->input->GetSelectedGameObjects().empty() || initialTransforms.empty()) return;

	if (!HasTransformationChanged()) return;

	TransformCommand command;
	command.description = description;
	command.timestamp = SDL_GetTicks();

	for (const auto& pair : initialTransforms) {
		if (pair.first && pair.first->GetTransform()) {
			TransformState initialState;
			initialState.gameObject = pair.first;
			initialState.transform = pair.second;
			initialState.objectName = pair.first->GetName();
			command.states.push_back(initialState);
		}
	}

	while (!redoStack.empty()) {
		redoStack.pop();
	}

	if (undoStack.size() >= MAX_UNDO_STATES) {
		std::stack<TransformCommand> tempStack;
		while (undoStack.size() > MAX_UNDO_STATES - 1) {
			undoStack.pop();
		}
	}

	undoStack.push(command);
	initialTransforms.clear();
	transformChanged = false;
}

void Undo() {
	if (undoStack.empty()) return;

	TransformCommand command = undoStack.top();
	undoStack.pop();

	TransformCommand redoCommand;
	redoCommand.description = "Redo " + command.description;
	redoCommand.timestamp = SDL_GetTicks();

	for (const auto& state : command.states) {
		if (state.gameObject && state.gameObject->GetTransform()) {
			TransformState currentState;
			currentState.gameObject = state.gameObject;
			currentState.transform = state.gameObject->GetTransform()->GetMatrix();
			currentState.objectName = state.gameObject->GetName();
			redoCommand.states.push_back(currentState);

			state.gameObject->GetTransform()->SetMatrix(state.transform);
		}
	}

	redoStack.push(redoCommand);
}

void Redo() {
	if (redoStack.empty()) return;

	TransformCommand command = redoStack.top();
	redoStack.pop();

	TransformCommand undoCommand;
	undoCommand.description = "Undo " + command.description;
	undoCommand.timestamp = SDL_GetTicks();

	for (const auto& state : command.states) {
		if (state.gameObject && state.gameObject->GetTransform()) {
			TransformState currentState;
			currentState.gameObject = state.gameObject;
			currentState.transform = state.gameObject->GetTransform()->GetMatrix();
			currentState.objectName = state.gameObject->GetName();
			undoCommand.states.push_back(currentState);

			state.gameObject->GetTransform()->SetMatrix(state.transform);
		}
	}

	undoStack.push(undoCommand);
}

void UndoRedoSystem() {
	if (ImGuizmo::IsUsing()) {
		if (!wasUsingGizmo) {
			BeginTransformAction();
			wasUsingGizmo = true;
		}
		transformChanged = true;
	}
	else if (wasUsingGizmo) {
		if (transformChanged) {
			CommitTransformAction("Gizmo Transform");
		}
		wasUsingGizmo = false;
	}

	if (Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) {
		if (Application->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
			Undo();
		}
		else if (Application->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) {
			Redo();
		}
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
	if (Application->input->GetMouseButton(1) != KEY_DOWN && !ImGuizmo::IsUsing()) {
		return;
	}

	glm::vec3 rayOrigin = glm::vec3(glm::inverse(Application->camera->view()) * glm::vec4(0, 0, 0, 1));
	glm::vec3 rayDirection = Application->input->getMousePickRay();

	if (rayDirection == glm::vec3(0, 0, -1) && Application->input->GetMouseButton(1) != KEY_DOWN) {
		return;
	}

	bool isCtrlHeld = Application->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT ||
		Application->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT;

	std::vector<std::pair<GameObject*, float>> objectsHit;

	if (Application->input->GetMouseButton(1) == KEY_DOWN && Application->gui->UISceneWindowPanel->isFoucused)
	{
		if (ImGuizmo::IsOver()) {
			return;
		}

		objectsHit.clear();

		
		for (auto& object : objects)
		{
			if (object->HasComponent<MeshRenderer>() && object->IsActive())
			{
				BoundingBox bbox = object->GetComponent<MeshRenderer>()->GetMesh()->boundingBox();
				bbox = object->GetTransform()->GetMatrix() * bbox;
				glm::vec3 collisionPoint;

				if (Application->gui->UISceneWindowPanel->CheckRayAABBCollision(rayOrigin, rayDirection, bbox, collisionPoint))
				{
					float distance = glm::distance(rayOrigin, collisionPoint);
					objectsHit.emplace_back(object, distance);
				}
			}
		}

	
		std::sort(objectsHit.begin(), objectsHit.end(),
			[](const auto& a, const auto& b) {
				return a.second < b.second;
			});

		if (!objectsHit.empty()) {
			GameObject* selectedObject = objectsHit[0].first;


			if (Application->input->IsGameObjectSelected(selectedObject) && objectsHit.size() > 1) {
				selectedObject = objectsHit[1].first;
			}

			if (!isCtrlHeld) {
				Application->input->ClearSelection();
			}

			if (isCtrlHeld && Application->input->IsGameObjectSelected(selectedObject)) {
				Application->input->RemoveFromSelection(selectedObject);
			}
			else {
				Application->input->SetDraggedGameObject(selectedObject);
				Application->input->AddToSelection(selectedObject);
			}
		}
		else if (!isCtrlHeld) {
			Application->input->ClearSelection();
		}
	}
}

static void RenderOutline(GameObject* object) {
	if (!object->isSelected || !object->HasComponent<MeshRenderer>()) return;

	GLint lastProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
	GLboolean depthTestEnabled;
	glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
	GLboolean blendEnabled;
	glGetBooleanv(GL_BLEND, &blendEnabled);
	GLint blendSrcFunc, blendDstFunc;
	glGetIntegerv(GL_BLEND_SRC, &blendSrcFunc);
	glGetIntegerv(GL_BLEND_DST, &blendDstFunc);
	GLfloat lineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glm::mat4 modelMatrix = object->GetTransform()->GetMatrix();
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(modelMatrix));
	object->GetComponent<MeshRenderer>()->Render();
	glPopMatrix();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	float outlineScale = 1.03f;
	glm::mat4 outlineMatrix = glm::scale(modelMatrix, glm::vec3(outlineScale));

	glColor4f(1.0f, 0.5f, 0.0f, 0.8f);

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(outlineMatrix));
	object->GetComponent<MeshRenderer>()->Render();
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);
	if (depthTestEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (blendEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	glBlendFunc(blendSrcFunc, blendDstFunc);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (lastProgram > 0) {
		glUseProgram(lastProgram);
	}
}

static void UpdateLightSystem() {
	auto activeScene = Application->root->GetActiveScene();
	if (!activeScene) return;

	activeScene->_lights.clear();

	auto sceneChildren = activeScene->children();
	for (const auto& objPtr : sceneChildren) {
		if (!objPtr || !objPtr->IsActive()) continue;

		std::function<void(std::shared_ptr<GameObject>)> addLightsRecursive = [&](std::shared_ptr<GameObject> obj) {
			if (!obj || !obj->IsActive()) return;

			if (obj->HasComponent<LightComponent>()) {
				activeScene->_lights.push_back(obj);
			}

			for (const auto& child : obj->GetChildren()) {
				addLightsRecursive(child);
			}
			};

		addLightsRecursive(objPtr);
	}
}

static void RenderEditor() {
	UISceneWindow* sceneWindow = static_cast<UISceneWindow*>(Application->gui->UISceneWindowPanel);
	bool useMSAA = sceneWindow->msaaSamples > 0;

	if (useMSAA) {
		glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->multisampleFBO);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);
	}

	glViewport(0, 0, (int)Application->gui->camSize.x, (int)Application->gui->camSize.y);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	configureCamera();
	drawFloorGrid(256, 4);

	auto activeScene = Application->root->GetActiveScene();
	if (!activeScene) return;

	UpdateLightSystem();

	auto sceneChildrenCopy = activeScene->children();
	std::vector<GameObject*> objects;

	for (const auto& objPtr : sceneChildrenCopy) {
		if (!objPtr) continue;
		GameObject* object = objPtr.get();
		if (!object) continue;
		objects.push_back(object);

		for (const auto& childPtr : object->GetChildren()) {
			if (!childPtr) continue;
			GameObject* child = childPtr.get();
			if (!child) continue;
			objects.push_back(child);
		}

		if (object->IsActive()) {
			object->Update(FIXED_TIME_STEP);

			if (Application->hasChangedScene) {
				Application->hasChangedScene = false;
				return;
			}
		}
	}

	Application->physicsModule->DrawDebugDrawer();

	objects.erase(std::remove(objects.begin(), objects.end(), nullptr), objects.end());

	if (SceneManagement->currentScene->sceneState == Scene::SceneState::PLAY) {
		Application->physicsModule->linkPhysicsToScene = true;
	}

	MousePickingCheck(objects);

	if (useMSAA) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, Application->gui->multisampleFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Application->gui->fbo);
		glBlitFramebuffer(0, 0, (int)Application->gui->camSize.x, (int)Application->gui->camSize.y,
			0, 0, (int)Application->gui->camSize.x, (int)Application->gui->camSize.y,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	glBindTexture(GL_TEXTURE_2D, Application->gui->fboTexture);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void EditorRenderer(MyGUI* gui) {
	if (Application->window->IsOpen()) {

#ifdef PROFILE
		OPTICK_CATEGORY("RenderEditor", Optick::Category::GameLogic);
#endif // PROFILE

		const auto t0 = hrclock::now();

		RenderEditor();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef PROFILE
		OPTICK_CATEGORY("GUIRender", Optick::Category::GameLogic);
#endif // PROFILE

		gui->Render();
	}
}

static double counterUsingDeltaTime = 0.0;
static double counterUsingChrono = 0.0;
static hrclock::time_point startTime = hrclock::now();

static void GameRelease() {

#ifdef PROFILE
	OPTICK_CATEGORY("GameRelease", Optick::Category::GameLogic);
#endif // PROFILE

	if (Application->root->mainCamera == nullptr) {
		return;
	}

	CameraComponent* gameCamera = Application->root->mainCamera->GetComponent<CameraComponent>();
	if (!gameCamera) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Application->window->width(), Application->window->height());

	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto activeScene = Application->root->GetActiveScene();
	if (!activeScene) return;

	activeScene->_lights.clear();

	std::vector<std::shared_ptr<GameObject>> UI;

	for (auto& object : activeScene->children()) {
		if (object->HasComponent<UICanvasComponent>()) {
			UI.push_back(object);
			continue;
		}

		if (object->IsActive()) {
			std::function<void(std::shared_ptr<GameObject>)> addLightsRecursive = [&](std::shared_ptr<GameObject> obj) {
				if (!obj || !obj->IsActive()) return;

				if (obj->HasComponent<LightComponent>()) {
					activeScene->_lights.push_back(obj);
				}

				for (const auto& child : obj->GetChildren()) {
					addLightsRecursive(child);
				}
				};

			addLightsRecursive(object);

			object->Update(static_cast<float>(Application->GetDt()));

			if (Application->hasChangedScene) {
				Application->hasChangedScene = false;
				break;
			}
		}
	}

	if (SceneManagement->currentScene->sceneState == Scene::SceneState::PLAY) {
		Application->physicsModule->linkPhysicsToScene = true;
	}

	for (const auto& i : UI) {
		if (i->IsActive()) {
			i->Update(static_cast<float>(Application->GetDt()));
		}
	}

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
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

			Application->LoadAllParticleTextures();
			AudioManager::Initialize();
			if (Application->Awake()) { state = START; }
			else { printf("Failed on Awake"); state = FAIL; }
			break;

		case START:

			if (Application->Start()) { state = LOOP; }
			else { state = FAIL; printf("Failed on START"); }
			break;

		case LOOP:

#ifndef _BUILD
			lights.clear();
			Application->root->GetActiveScene()->_lights.clear();
			EditorRenderer(Application->gui);
			RenderGameView();

			Application->window->SwapBuffers();

			UndoRedoSystem();
			ObjectToEditorCamera();

			if (MonoManager::GetInstance().IsHotReloadingEnabled()) {
				ScriptHotReloader::GetInstance().Update();
			}
#else
			lights.clear();
			Application->root->GetActiveScene()->_lights.clear();
			GameRelease();
			Application->window->SwapBuffers();
#endif // ENABLE_EDITOR
		
			if (!Application->Update()) { state = FREE; }
			break;

		case FREE:

			MonoManager::GetInstance().Shutdown();
			ShaderManager::GetInstance().Cleanup();
			AudioManager::Shutdown();

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