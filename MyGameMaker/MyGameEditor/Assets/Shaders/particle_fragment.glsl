#version 450 core

in vec2 TexCoord;
in vec4 ParticleColor;
in float Lifetime;
in vec4 EndColor;  // End color for gradient

in vec2 SpriteOffset;  
in vec2 SpriteSize; 
in vec2 SheetSize;

out vec4 FragColor;

uniform sampler2D particleTexture;
uniform sampler2D colorGradient;  // Optional color gradient based on lifetime
uniform int useColorGradient;     // Whether to use gradient (0: no, 1: yes)
uniform int particleType;         // Different particle types (0: default, 1: smoke, 2: fire, 3: muzzle)
uniform float softness;           // Edge softness factor
//uniform vec4 startColor;          // Color inicial
//uniform vec4 endColor;            // Color final

void main() {
 
// Calculate the UV offsets for the sprite
vec2 uv_min = SpriteOffset / SheetSize;
vec2 uv_max = (SpriteOffset + SpriteSize) / SheetSize;


    // Remap texture coordinates to the sub-region
    vec2 croppedTexCoords = uv_min + TexCoord * (uv_max - uv_min);

    vec4 texColor = texture(particleTexture, croppedTexCoords);
    
    // Si la textura es completamente transparente, descartarla inmediatamente
    if (texColor.a < 0.01) {
        discard;
        return;
    }
    
    if (useColorGradient == 1) {
        // Use color gradient based on lifetime
        vec4 gradientColor = texture(colorGradient, vec2(Lifetime, 0.5));
        
        // Preservar el alfa de la textura mientras usamos los colores del gradiente
        FragColor = vec4(texColor.rgb * gradientColor.rgb * ParticleColor.rgb, texColor.a * ParticleColor.a);
    } else {
        // Apply different effects based on particle type
            // Default particles
            // Aplicar el color de la partícula pero preservar el alfa de la textura
            vec4 color = mix(ParticleColor, EndColor, Lifetime);
            FragColor = vec4(texColor.rgba * color.rgba);
            // FragColor = vec4(texColor.rgb * ParticleColor.rgb, texColor.a * ParticleColor.a);
        
    }
    
    // Apply softness to edges for more natural look
    if (softness > 0.0) {
        float distance = length(TexCoord - vec2(0.5, 0.5)) * 2.0;
        float edgeFade = 1.0 - smoothstep(1.0 - softness, 1.0, distance);
        FragColor.a *= edgeFade;
    }
    
    // Discard fully transparent fragments
    if (FragColor.a < 0.01) {
        discard;
    }
}