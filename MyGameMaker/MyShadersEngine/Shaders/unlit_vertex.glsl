#version 450 core

layout(location = 0) in vec3 aPos;      // Position attribute
layout(location = 1) in vec2 aTexCoord;  // Texture coordinate attribute
layout(location = 2) in vec3 aNormal;    // Normal attribute (unused but kept for compatibility)

out vec2 TexCoord;  // Texture coordinate of the fragment

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform the vertex position to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;
}