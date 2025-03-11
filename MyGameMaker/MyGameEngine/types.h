#ifndef TYPES_H
#define TYPES_H

#pragma once

#include <glm/glm.hpp>
#ifdef YAML_CPP_DLL_EXPORTS
#define YAML_CPP_API __declspec(dllexport)
#else
#define YAML_CPP_API __declspec(dllimport)
#endif
#include <yaml-cpp/yaml.h>


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
using mat4 = glm::dmat4;
using vec2 = glm::dvec2;
using vec3 = glm::dvec3;
using vec4 = glm::dvec4;
using ivec2 = glm::ivec2;
using u8vec3 = glm::u8vec3;
using u8vec4 = glm::u8vec4;

using color1 = glm::u8vec1;
using color2 = glm::u8vec2;
using color3 = glm::u8vec3;
using color4 = glm::u8vec4;

// Specialize YAML::convert for glm::dvec3
namespace YAML {

    template<>
    struct convert<glm::dvec3> {
        static Node encode(const glm::dvec3& rhs) {
            Node node;
            node["x"] = (rhs.x);
            node["y"] = (rhs.y);
            node["z"] = (rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::dvec3& rhs) {

            if (!node["x"] || !node["y"] || !node["z"]) {
                return false;
            }

            rhs.x = node["x"].as<double>();
            rhs.y = node["y"].as<double>();
            rhs.z = node["z"].as<double>();
            return true;
        }
    };
}

enum ShaderType {
	UNLIT,
    PBR
};

namespace YAML {
    template<>
    struct convert<glm::dmat4> {
        static inline Node encode(const glm::dmat4& rhs) {
            Node node;
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    node["row" + std::to_string(i)]["col" + std::to_string(j)] = rhs[i][j];
                }
            }
            return node;
        }

        static inline bool decode(const Node& node, glm::dmat4& rhs) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    std::string rowKey = "row" + std::to_string(i);
                    std::string colKey = "col" + std::to_string(j);

                    if (!node[rowKey] || !node[rowKey][colKey]) {
                        return false;
                    }
                    rhs[i][j] = node[rowKey][colKey].as<double>();
                }
            }
            return true;
        }
    };
}

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

// Directories
#define ASSETS_PATH "Assets\\"
#define LIBRARY_PATH "Library\\"

// Config
#define TITLE "HawkEngine"
#define VERSION "v0.1"
#define ORGANIZATION "UPC-CITM"

#endif