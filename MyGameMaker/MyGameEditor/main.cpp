#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include "MyGUI.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "assimp/cimport.h" 
#include "assimp/scene.h" 
#include "assimp/postprocess.h"
#include "../MyGameEngine/Mesh.h"
#include "Camera.h"
#include "imgui.h"

using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;
Mesh mesh;

static Camera camera;
static const ivec2 WINDOW_SIZE(1280, 720);
static const auto FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScaled(1.0, (double)WINDOW_SIZE.x/WINDOW_SIZE.y, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
}

static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
	glColor4ub(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLES);
	glVertex3d(center.x, center.y + size, center.z);
	glVertex3d(center.x - size, center.y - size, center.z);
	glVertex3d(center.x + size, center.y - size, center.z);
	glEnd();
}

static void drawFloorGrid(int size, double step) {
	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	for (double i = -size; i <= size; i += step) {
		glVertex3d(i, 0, -size);
		glVertex3d(i, 0, size);
		glVertex3d(-size, 0, i);
		glVertex3d(size, 0, i);
	}
	glEnd();
}
void move_camera() 
{
	//move the transform of the camera
    if (ImGui::IsKeyDown(ImGuiKey_W)) camera.transform().translate(camera.transform().fwd() * 0.1);
	if (ImGui::IsKeyDown(ImGuiKey_S)) camera.transform().translate(-camera.transform().fwd() * 0.1);
	if (ImGui::IsKeyDown(ImGuiKey_A)) camera.transform().translate(-camera.transform().left() * 0.1);
	if (ImGui::IsKeyDown(ImGuiKey_D)) camera.transform().translate(camera.transform().left() * 0.1);
	if (ImGui::IsKeyDown(ImGuiKey_Q)) camera.transform().translate(-camera.transform().up() * 0.1);
	if (ImGui::IsKeyDown(ImGuiKey_E)) camera.transform().translate(camera.transform().up() * 0.1);
	//rotate the transform of the camera
	
	
	
	

}

static void display_func() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadMatrixd(&camera.view()[0][0]);
	drawFloorGrid(16, 0.25);
	mesh.Draw();
}

int main(int argc, char** argv) {
	MyWindow window("ImGUI with SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
	MyGUI gui(window.windowPtr(), window.contextPtr());

	init_openGL();
	camera.transform().pos() = vec3(0, 1, 4);
	camera.transform().rotate(glm::radians(180.0), vec3(0, 1, 0));
	
	mesh.LoadMesh("BakerHouse.fbx");

	while (window.processEvents(&gui) && window.isOpen()) {
		const auto t0 = hrclock::now();
		display_func();
		gui.render();
		move_camera();
		window.swapBuffers();
		const auto t1 = hrclock::now();
		const auto dt = t1 - t0;
		if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
	}

	return 0;
}