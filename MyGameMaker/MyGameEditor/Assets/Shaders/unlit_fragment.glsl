#version 430 core

// Input from vertex shader
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// Output color
out vec4 FragColor;

// Material properties
uniform vec4 albedoColor;
uniform int u_HasTexture;
uniform sampler2D texture1;

// NO usar uniform sampler2D albedoTexture; -- eliminar esta referencia

void main()
{
    // Color por defecto (blanco)
    vec4 texColor = vec4(1.0);
    
    // Debug: usar un color fijo para verificar que está funcionando
    vec4 debugColor = vec4(1.0, 0.0, 1.0, 1.0); // Magenta, muy visible
    
    // Sample texture if available
    if (u_HasTexture > 0) {
        texColor = texture(texture1, TexCoords);
        
        // Discard fragments with very low alpha
        if (texColor.a < 0.1)
            discard;
    }
    
    // Final color: combinar textura con color del material
    // Para depuración, puedes descomentar debugColor para forzar un color fijo
    FragColor = texColor * albedoColor;
    // FragColor = debugColor; // Descomentar para depuración
}