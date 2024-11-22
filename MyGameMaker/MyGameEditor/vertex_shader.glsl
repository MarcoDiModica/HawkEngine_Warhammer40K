#version 450 core

layout(location = 0) in vec3 aPos; // Position attribute
layout(location = 1) in vec3 aColor; // Color attribute

out vec3 ourColor; // Output color to the fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor; // Pass the color to the fragment shader
}