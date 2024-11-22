#version 450 core

in vec3 ourColor; // Input color from the vertex shader

out vec4 FragColor; // Output color

void main()
{
    FragColor = vec4(ourColor, 1.0); // Set the output color
}