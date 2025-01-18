#pragma once
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
using namespace std;

class Particle
{
public:
	Particle();

	~Particle();

	string name;
	float lifetime;
	float rotation;	
	vector<glm::vec3> position;
	vector<glm::vec3> speed;
	
};
