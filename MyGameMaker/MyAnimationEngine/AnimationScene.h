#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H


#include <glm/glm.hpp>

#include <assimp\Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

#include "glslprogram.h"
#include "SkeletalModel.h"
#include "MyGameEngine/types.h"


class AnimationScene
{
private:
    GLSLProgram* prog; //!< Shader program 

    int width, height;

    glm::dmat4 model; // Model matrix

    SkeletalModel* m_AnimatedModel; //!< The skeletal model 

    void setMatrices(); //Set the camera matrices

    void compileAndLinkShader(); //Compile and link the shader

public:
    AnimationScene(); //Constructor

    void setLightParams(); //Setup the lighting

    void initScene(); //Initialise the scene

    void update(long long f_startTime, float f_Interval); //Update the scene

    void render();	//Render the scene

    void resize(int, int); //Resize
};

#endif
