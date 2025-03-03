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

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

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
#include "MyWindow.h"

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

Shaders mainShader;

App* Application = NULL;
SceneManager* SceneManagement = NULL;
InputEngine* InputManagement = NULL;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScaled(1.0, (double)WINDOW_SIZE.x / WINDOW_SIZE.y, 1.0);

	glMatrixMode(GL_MODELVIEW);
}

static void RenderObjectAndChildren(GameObject* object) {
	if (object->HasComponent<MeshRenderer>()) {
		object->GetComponent<MeshRenderer>()->RenderMainCamera();
	}

	for (const auto& child : object->GetChildren()) {
		RenderObjectAndChildren(child.get());
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

	glViewport(0, 0, Application->window->width(), Application->window->height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	for (size_t i = 0; i < Application->root->GetActiveScene()->children().size(); ++i) {
		GameObject* object = Application->root->GetActiveScene()->children()[i].get();
		RenderObjectAndChildren(object);
	}

	//for all obj in scene and call update
	Application->root->GetActiveScene()->Update(Application->GetDt());
	Application->root->Update(Application->GetDt());
	Application->physicsModule->Update(Application->GetDt());

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

GLuint uiTextureID = 0;

bool LoadUITexture(const char* filepath) {
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	bool success = ilLoadImage(filepath);
	if (!success) {
		printf("Error cargando la imagen UI: %s\n", filepath);
		ilDeleteImages(1, &imageID);
		return false;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	glGenTextures(1, &uiTextureID);
	glBindTexture(GL_TEXTURE_2D, uiTextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	ilDeleteImages(1, &imageID);
	return true;
}

void RenderUI() {
	if (uiTextureID == 0) return;

	int screenWidth = Application->window->width();
	int screenHeight = Application->window->height();

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, screenWidth, screenHeight, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uiTextureID);

	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(screenWidth, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(screenWidth, screenHeight);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, screenHeight);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}

static void GameRenderer() {
	if (Application->window->IsOpen()) {
		const auto t0 = hrclock::now();

		RenderGameView();
		RenderUI();

		Application->window->SwapBuffers();

		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}
}

int main(int argc, char** argv) {
	MainState state = CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT) {
		switch (state) {
		case CREATE:
			Application = new App();
			Application->physicsModule->Awake();
			Application->physicsModule->Start();
			MonoManager::GetInstance().Initialize();

			ilInit();
			iluInit();
			ilutInit();

			init_openGL();

			if (Application) {
				state = AWAKE;
			}
			else {
				state = FAIL;
				printf("Failed on Create");
			}
			break;

		case AWAKE:
			LoadUITexture("Assets/Textures/UI_Final.png");
			if (Application->Awake()) {
				state = START;
			}
			else {
				printf("Failed on Awake");
				state = FAIL;
			}
			break;

		case START:
			if (Application->Start()) {
				state = LOOP;
			}
			else {
				state = FAIL;
				printf("Failed on START");
			}
			break;

		case LOOP:
			GameRenderer();

			if (!Application->Update()) {
				state = FREE;
			}
			break;

		case FREE:
			if (Application->CleanUP()) {
				state = EXIT;
				result = EXIT_SUCCESS;
			}
			else {
				state = FAIL;
				printf("Failed on FREE");
			}
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

	MonoManager::GetInstance().Shutdown();

	return result;
}