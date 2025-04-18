#version 430 core
#extension GL_ARB_bindless_texture : enable

// Input vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

// Output to fragment shader
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

// Uniforms
uniform mat4 model;        // Model matrix para la instancia actual
uniform mat4 view;         // View matrix
uniform mat4 projection;   // Projection matrix

void main()
{
    // Calcular posición en espacio mundo
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    // Pasar coordenadas de textura al fragment shader
    TexCoords = aTexCoords;
    
    // Transformar normal a espacio mundo
    // La matriz inverse transpose asegura que las normales se escalen correctamente
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Calcular posición final en espacio clip
    gl_Position = projection * view * worldPos;
}