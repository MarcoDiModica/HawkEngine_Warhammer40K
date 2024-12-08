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
#include <IL/ilut.h>
#include "Input.h"
#include "MyGUI.h"
#include "UISceneWindow.h"


//pruebas de include "StaticLibEngineIncludes"
#include "MyGameEngine/GameObject.h"
#include "MyGameEngine/TransformComponent.h"
#include "MyGameEngine/MeshRendererComponent.h"
#include "App.h"


#include "Gizmos.h"

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

static EditorCamera* camera = nullptr;

App* Application = NULL;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScaled(1.0, (double)WINDOW_SIZE.x/WINDOW_SIZE.y, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
}

struct Triangle {
	vec3 v0, v1, v2;
	void draw_triangle(const u8vec4& color) {
		//glColor4ub(color.r, color.g, color.b, color.a);
		glBegin(GL_TRIANGLES);
		glVertex3d(v0.x, v0.y, v0.z);
		glVertex3d(v1.x, v1.y, v1.z);
		glVertex3d(v2.x, v2.y, v2.z);
		//glColor4ub(255.0f, 255.0f, 255.0f, 0.0f);
		glEnd();
	}
};

static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
	glColor4ub(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLES);
	glVertex3d(center.x, center.y + size, center.z);
	glVertex3d(center.x - size, center.y - size, center.z);
	glVertex3d(center.x + size, center.y - size, center.z);
	
	glEnd();
}

static void drawFloorGrid(int size, double step) {
	//glColor3ub(0, 2, 200);
	glBegin(GL_LINES);
	Application->root->currentScene->DebugDrawTree();

	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	
	glEnd();
}



// Initializes camera
void configureCamera() {
	glm::dmat4 projectionMatrix = camera->projection();
	glm::dmat4 viewMatrix = camera->view();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(glm::value_ptr(viewMatrix));
}
#pragma region RayPickingCode(MoveSomewhereElse)

glm::vec3 ConvertMouseToWorldCoords(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x,int window_y)
{
	int adjusted_mouse_x = mouse_x - window_x;
	int adjusted_mouse_y = mouse_y - window_y;

	if (adjusted_mouse_x < 0 || adjusted_mouse_x > screen_width || adjusted_mouse_y < 0 || adjusted_mouse_y > screen_height) {
		// if the mouse is outside the window make it so it doesnt detect anything
	}

	float ndc_x = (2.0f * adjusted_mouse_x) / screen_width - 1.0f;
	float ndc_y = 1.0f - (2.0f * adjusted_mouse_y) / screen_height;

	glm::vec4 clip_coords = glm::vec4(ndc_x, ndc_y, -1.0f, 1.0f);

	glm::mat4 projection_matrix = camera->projection();
	glm::vec4 view_coords = glm::inverse(projection_matrix) * clip_coords;
	view_coords = glm::vec4(view_coords.x, view_coords.y, -1.0f, 0.0f);

	glm::mat4 view_matrix = camera->view();
	glm::vec4 world_coords = glm::inverse(view_matrix) * view_coords;

	return glm::vec3(world_coords.x + camera->GetTransform().GetPosition().x, world_coords.y + camera->GetTransform().GetPosition().y, world_coords.z + camera->GetTransform().GetPosition().z);
}

void DrawRay(const glm::vec3& ray_origin, const glm::vec3& ray_direction)
{
	//draw the ray
	glBegin(GL_LINES);
	glVertex3f(ray_origin.x, ray_origin.y, ray_origin.z);
	glVertex3f(ray_origin.x + ray_direction.x , ray_origin.y + ray_direction.y , ray_origin.z + ray_direction.z);
	glEnd();
}

glm::vec3 GetMousePickDir(int mouse_x, int mouse_y, int screen_width, int screen_height, int window_x, int window_y)
{
	int adjusted_mouse_x = mouse_x - window_x;
	int adjusted_mouse_y = mouse_y - window_y;

	if (adjusted_mouse_x < 0 || adjusted_mouse_x > screen_width || adjusted_mouse_y < 0 || adjusted_mouse_y > screen_height) {
		 // if the mouse is outside the window make it so it doesnt detect anything
	}

	glm::vec3 window_coords = glm::vec3(adjusted_mouse_x, screen_height - adjusted_mouse_y, 0.0f);

	glm::mat4 view_matrix = camera->view();
	glm::mat4 projection_matrix = camera->projection();

	glm::vec4 viewport = glm::vec4(0, 0, screen_width, screen_height);

	glm::vec3 v0 = glm::unProject(window_coords, view_matrix, projection_matrix, viewport);
	glm::vec3 v1 = glm::unProject(glm::vec3(window_coords.x, window_coords.y, 1.0f), view_matrix, projection_matrix, viewport);

	glm::vec3 world_coords = (v1 - v0);

	return world_coords;
}
struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

bool CheckRayAABBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const BoundingBox& bBox) {
	float tmin = (bBox.min.x - rayOrigin.x) / rayDir.x;
	float tmax = (bBox.max.x - rayOrigin.x) / rayDir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (bBox.min.y - rayOrigin.y) / rayDir.y;
	float tymax = (bBox.max.y - rayOrigin.y) / rayDir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (bBox.min.z - rayOrigin.z) / rayDir.z;
	float tzmax = (bBox.max.z - rayOrigin.z) / rayDir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	return true;
}

#pragma endregion

void Draw3DRectangle(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float width, float height, float depth) {
	glm::vec3 right = glm::normalize(glm::cross(rayDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * width;
	glm::vec3 up = glm::normalize(glm::cross(right, rayDirection)) * height;
	glm::vec3 forward = glm::normalize(rayDirection) * depth;

	glm::vec3 vertices[8];
	vertices[0] = rayOrigin;
	vertices[1] = rayOrigin + right;
	vertices[2] = rayOrigin + right + up;
	vertices[3] = rayOrigin + up;
	vertices[4] = rayOrigin + forward;
	vertices[5] = rayOrigin + forward + right;
	vertices[6] = rayOrigin + forward + right + up;
	vertices[7] = rayOrigin + forward + up;

	glBegin(GL_LINES);
	// Front face
	glVertex3fv(glm::value_ptr(vertices[0]));
	glVertex3fv(glm::value_ptr(vertices[1]));
	glVertex3fv(glm::value_ptr(vertices[1]));
	glVertex3fv(glm::value_ptr(vertices[2]));
	glVertex3fv(glm::value_ptr(vertices[2]));
	glVertex3fv(glm::value_ptr(vertices[3]));
	glVertex3fv(glm::value_ptr(vertices[3]));
	glVertex3fv(glm::value_ptr(vertices[0]));

	// Back face
	glVertex3fv(glm::value_ptr(vertices[4]));
	glVertex3fv(glm::value_ptr(vertices[5]));
	glVertex3fv(glm::value_ptr(vertices[5]));
	glVertex3fv(glm::value_ptr(vertices[6]));
	glVertex3fv(glm::value_ptr(vertices[6]));
	glVertex3fv(glm::value_ptr(vertices[7]));
	glVertex3fv(glm::value_ptr(vertices[7]));
	glVertex3fv(glm::value_ptr(vertices[4]));

	// Connecting lines
	glVertex3fv(glm::value_ptr(vertices[0]));
	glVertex3fv(glm::value_ptr(vertices[4]));
	glVertex3fv(glm::value_ptr(vertices[1]));
	glVertex3fv(glm::value_ptr(vertices[5]));
	glVertex3fv(glm::value_ptr(vertices[2]));
	glVertex3fv(glm::value_ptr(vertices[6]));
	glVertex3fv(glm::value_ptr(vertices[3]));
	glVertex3fv(glm::value_ptr(vertices[7]));
	glEnd();
}

//using it to debug axis movement will be removed
void CreateSphere(const glm::vec3& position, float radius, const glm::vec3& color) {

	glColor3f(color.r, color.g, color.b);
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);

	// Create a sphere using OpenGL
	GLUquadric* quad = gluNewQuadric();
	gluSphere(quad, radius, 20, 20);
	gluDeleteQuadric(quad);

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
}

static void display_func() {
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);
	glViewport(0, 0, Application->window->width(), Application->window->height());
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	configureCamera();
	drawFloorGrid(16, 0.25);



	glm::vec3 rayOrigin = glm::vec3(glm::inverse(camera->view()) * glm::vec4(0, 0, 0, 1));
	glm::vec3 rayDirection = Application->input->getMousePickRay();

	//Debug for mousepicking
	//Draw3DRectangle(rayOrigin, rayDirection, 1.0f, 1.0f, 1.0f);
	

	// TODO cambiar esto de sitio
	for (size_t i = 0; i < Application->root->currentScene->children().size(); ++i)
	{
		GameObject* object = Application->root->currentScene->children()[i].get();
	
		object->Update(0.16f);
		
		if (object->HasComponent<MeshRenderer>()) {
	
			BoundingBox bbox = object->GetComponent<MeshRenderer>()->GetMesh()->boundingBox();
	
			bbox = object->GetTransform()->GetMatrix() * bbox;
	
			if (CheckRayAABBCollision(rayOrigin, rayDirection, bbox))
			{
				Application->input->SetDraggedGameObject(object);
			}
	
			if (Application->input->GetMouseButton(1) == KEY_DOWN)
			if (CheckRayAABBCollision(rayOrigin, rayDirection, bbox) )
			{
				std::cout << "Hit: " << object->GetName();
				Application->input->AddToSelection(object);
			}
		}
	}
	//It has to go AFTER drawing the objects
	//Application->gizmos->DrawGizmos();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Application->root->sceneManagement.Update(0.16f);

}

void PauCode2(MyGUI* gui) {

	if (Application->window->IsOpen()) {

		const auto t0 = hrclock::now();
		display_func();
	
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

	// The application is created
	Application = new App();

	/*Application->camera = new Camera(Application);*/
	
	//std::shared_ptr<Camera> cameraPtr = std::make_shared<Camera>(Application);
	//camera = cameraPtr.get();

	//Application->camera = shared_ptr<Camera>
	

	//initialize devil
	ilInit();
	iluInit();
	ilutInit();

	init_openGL();

	camera = Application->camera;

	while (state != EXIT) 
	{
		switch (state)
		{

		case CREATE:

			/*Application = new App();*/

			if (Application) {	
				state = AWAKE;

				/*gui = PauCode();*/
			}
			else { state = FAIL; printf("Failed on Create"); }
			break;


		case AWAKE:
			//Application->AddModule(cameraPtr.get(), true);

			camera->GetTransform().GetPosition() = vec3(0, 1, 4);
			camera->GetTransform().Rotate(glm::radians(180.0), vec3(0, 1, 0));
			if (Application->Awake()) { state = START; }

			else
			{
				printf("Failed on Awake");
				state = FAIL;
			}
			break;

		case START:

			if (Application->Start()) { state = LOOP; }
			else { state = FAIL; printf("Failed on START"); }
			break;

		case LOOP:
			

			PauCode2(Application->gui);

			if (!Application->Update()) {
				state = FREE;
			}
			break;


		case FREE:

			// TODO Free all classes and memory
			state = EXIT;
			break;
		}

	}






	//initialize devil
	//ilInit();
	//iluInit();
	//ilutInit();
	//Window window("ImGUI with SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
	//MyGUI gui(window.windowPtr(), window.contextPtr());

	//init_openGL();
	//camera.transform().pos() = vec3(0, 1, 4);
	//camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));

	//

	//mesh.LoadMesh("BakerHouse.fbx");
	//mesh.LoadTexture("Baker_house.png");
	//mesh.LoadCheckerTexture();

	//while (window.processEvents(&gui) && window.isOpen()) {
	//	const auto t0 = hrclock::now();
	//display_func();
	//	gui.render();
	//	move_camera();
	//	window.swapBuffers();
	//	const auto t1 = hrclock::now();
	//	const auto dt = t1 - t0;
	//	if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	//}

	return 0;
}

