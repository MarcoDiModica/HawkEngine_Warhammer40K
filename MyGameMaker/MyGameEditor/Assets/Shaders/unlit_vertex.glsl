#version 450 core

layout(location = 0) in vec3 aPos; // Position attribute
layout(location = 1) in vec2 aTexCoord; // Texture coordinate attribute
layout(location = 2) in vec3 aNormal; // Normal attribute
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

out vec2 TexCoord; // Texture coordinate of the fragment
out vec3 Normal; // Normal of the fragment

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	Normal = aNormal;
};