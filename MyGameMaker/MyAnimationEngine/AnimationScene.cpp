#include "AnimationScene.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

//cosa ilegal
#include "MyGameEditor/App.h"
#include "MyGameEditor/MyGui.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Default constructor
/////////////////////////////////////////////////////////////////////////////////////////////
AnimationScene::AnimationScene()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Initialise the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::initScene()
{
	prog = new GLSLProgram();

	//|Compile and link the shader  
	compileAndLinkShader();

	prog->initialiseBoneUniforms();

	glEnable(GL_DEPTH_TEST);

	//Set up the lighting
	setLightParams();

	// Initialise skeletal model. 
	m_AnimatedModel = new SkeletalModel(prog);

	// Load the model from the given path. 
	m_AnimatedModel->LoadMesh("Assets/Meshes/Minotaur@Jump.FBX");
	//m_AnimatedModel->LoadMesh("Assets/astroBoy_walk_Maya.dae");
	//m_AnimatedModel->LoadMesh("Assets/mech.fbx");
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Update
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::update(long long f_StartTime, float f_Interval)
{
	// Vector of bone transformation matrices. 
	std::vector<Matrix4f> Transforms;

	// Obtains newly transformed matrices from the bone hierarchy at the given time. 
	m_AnimatedModel->BoneTransform(f_Interval, Transforms);

	// Passes each new bone transformation into the shader. 
	for (unsigned int i = 0; i < Transforms.size(); i++) {
		m_AnimatedModel->SetBoneTransform(i, Transforms[i]);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Set up the lighting variables in the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::setLightParams()
{
	glm::vec3 worldLight = glm::vec3(10.0f, 10.0f, 10.0f);

	prog->setUniform("lightIntensity", 0.5f, 0.5f, 0.5f);
	prog->setUniform("lightPos", worldLight);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Render the scene
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, Application->gui->fbo);
	glViewport(0, 0, Application->window->width(), Application->window->height());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Model matrix 
	model = mat4(1.0f);
	//model = glm::translate(glm::vec3(0.0, -20.0, 0.0));
	model = glm::scale(glm::vec3(0.1f));

	setMatrices();

	//Set the Teapot material properties in the shader and render
	prog->setUniform("Ka", glm::vec3(0.8f, 0.125f, 0.0f));
	prog->setUniform("Kd", glm::vec3(1.0f, 0.6f, 0.0f));
	prog->setUniform("Ks", glm::vec3(1.0f, 1.0f, 1.0f));
	prog->setUniform("specularShininess", 0.2f);

	m_AnimatedModel->render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Send the MVP matrices to the GPU
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::setMatrices()
{
	glm::dmat4 mv = Application->camera->view() * model;
	prog->setUniform("ModelViewMatrix", mv);

	prog->setUniform("MVP", Application->camera->projection() * mv);

	// the correct matrix to transform the normal is the transpose of the inverse of the M matrix
	glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(model)));

	prog->setUniform("M", model);
	prog->setUniform("NormalMatrix", normMat);
	prog->setUniform("V", Application->camera->view());
	prog->setUniform("P", Application->camera->projection());
}

/////////////////////////////////////////////////////////////////////////////////////////////
// resize the viewport
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	//camera.setAspectRatio((float)width / height);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Compile and link the shader
/////////////////////////////////////////////////////////////////////////////////////////////
void AnimationScene::compileAndLinkShader()
{

	try {
		prog->compileShader("Assets/Shaders/diffuse.vert");
		prog->compileShader("Assets/Shaders/diffuse.frag");
		prog->link();
		prog->validate();
		prog->use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}