#version 450 core

in vec2 TexCoord;  // Texture coordinate of the fragment

out vec4 FragColor;  // Final color of the fragment

uniform sampler2D texture1;  // Texture sampler
uniform vec4 modColor;       // Color modifier
uniform bool u_HasTexture;   // Whether the material has a texture

void main() {
    // Simple material with texture or solid color
    if (u_HasTexture) {
        vec4 texColor = texture(texture1, TexCoord);
        FragColor = texColor * modColor;
    } else {
        FragColor = modColor;
    }
    
    // Discard fragments with very low alpha
    if (FragColor.a < 0.05)
        discard;
}