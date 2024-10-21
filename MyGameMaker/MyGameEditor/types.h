#pragma once

#include <glm/glm.hpp>

using vec2 = glm::dvec2;
using mat4 = glm::dmat4;
using vec4 = glm::dvec4;
using vec3 = glm::dvec3;
using u8vec3 = glm::u8vec3;

namespace Colors {
	const glm::u8vec3 Red(255, 0, 0);
	const glm::u8vec3 Green(0, 255, 0);
	const glm::u8vec3 Blue(0, 0, 255);
};