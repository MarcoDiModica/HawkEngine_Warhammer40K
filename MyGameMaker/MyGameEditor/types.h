#pragma once

#include <glm/glm.hpp>

// Math constants
#define PI 3.14159265358979323846f;
#define DEGTORAD 0.01745329251994329577f;
#define RADTODEG 57.2957795130823208767f;

// Rad to deg
template <class T>
inline T RadToDegrees(const T rad)
{
	return DEGTORAD * rad;
}

// Deg to rad
template <class T>
inline T DegreesToRad(const T deg)
{
	return RADTODEG * deg;
}

// String sizes
#define SString 32
#define MString 255
#define BString 8192

// TypeDefs
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

// Join path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MString];
	sprintf_s(path, MString, "%s/%s", folder, file);
	return path;
}

// Config
#define TITLE "HawkEngine"
#define VERSION "v0.1"
#define ORGANIZATION "UPC-CITM"
