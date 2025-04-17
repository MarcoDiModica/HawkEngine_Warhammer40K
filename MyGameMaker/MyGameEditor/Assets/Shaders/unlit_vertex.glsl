#version 430 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

// Output to fragment shader
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

// Uniforms
uniform mat4 model;        // Model matrix
uniform mat4 view;         // View matrix
uniform mat4 projection;   // Projection matrix

void main()
{
    // Debug: Output vertex position via gl_Position
    // (esto no mostrará nada pero se asegura que la posición se esté procesando)
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // Pasar coordenadas a fragment shader
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Posición final
    gl_Position = projection * view * worldPos;
}