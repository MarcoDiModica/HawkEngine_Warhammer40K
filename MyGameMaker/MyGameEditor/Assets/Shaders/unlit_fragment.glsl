#version 430 core
#extension GL_ARB_bindless_texture : enable

// Input from vertex shader
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// Output color
out vec4 FragColor;

// Material properties
uniform vec4 albedoColor;
uniform int u_HasTexture;

// Soporte para texturas tradicionales
uniform sampler2D texture1;

// Soporte para texturas bindless
uniform sampler2D albedoTextureHandle;

void main()
{
    // Color base del material
    vec4 finalColor = albedoColor;
    
    // Sample texture if available
    if (u_HasTexture > 0) {
        // Intentar usar texture bindless handle si es soportado
        #if defined(GL_ARB_bindless_texture)
            // Usar el handle de textura bindless
            vec4 texColor = texture(albedoTextureHandle, TexCoords);
        #else
            // Usar el método tradicional
            vec4 texColor = texture(texture1, TexCoords);
        #endif
        
        // Descartar fragmentos con alpha muy bajo
        if (texColor.a < 0.1)
            discard;
            
        // Combinar textura con color del material
        finalColor = texColor * albedoColor;
    }
    
    // // Asegurar que el color es visible
    // if (finalColor.r < 0.01 && finalColor.g < 0.01 && finalColor.b < 0.01) {
    //     finalColor = vec4(1.0, 1.0, 1.0, finalColor.a);
    // }
    
    FragColor = finalColor;
}