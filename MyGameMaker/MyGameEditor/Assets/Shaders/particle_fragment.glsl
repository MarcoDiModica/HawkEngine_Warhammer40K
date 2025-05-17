#version 450 core

in vec2 TexCoord;
in vec4 ParticleColor;
in float Lifetime;
in vec4 EndColor;

in vec2 SpriteOffset;  
in vec2 SpriteSize; 
in vec2 SheetSize;

out vec4 FragColor;

uniform sampler2D particleTexture;
uniform sampler2D colorGradient;
uniform int useColorGradient;
uniform int particleType;
uniform float softness;

void main() {
    vec2 uv_min = SpriteOffset / SheetSize;
    vec2 uv_max = (SpriteOffset + SpriteSize) / SheetSize;

    vec2 croppedTexCoords = uv_min + TexCoord * (uv_max - uv_min);

    vec4 texColor = texture(particleTexture, croppedTexCoords);
    
    if (texColor.a < 0.01) {
        discard;
        return;
    }
    
    if (useColorGradient == 1) {
        vec4 gradientColor = texture(colorGradient, vec2(Lifetime, 0.5));
        FragColor = vec4(texColor.rgb * gradientColor.rgb * ParticleColor.rgb, texColor.a * ParticleColor.a);
    } else {
        vec4 color = mix(ParticleColor, EndColor, Lifetime);
        FragColor = vec4(texColor.rgba * color.rgba);
    }
    
    if (softness > 0.0) {
        float distance = length(TexCoord - vec2(0.5, 0.5)) * 2.0;
        float edgeFade = 1.0 - smoothstep(1.0 - softness, 1.0, distance);
        FragColor.a *= edgeFade;
    }
    
    if (FragColor.a < 0.01) {
        discard;
    }
}