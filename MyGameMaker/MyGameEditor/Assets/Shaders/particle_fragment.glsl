#version 450 core

in vec2 TexCoord;
in vec4 ParticleColor;
in float Lifetime;

out vec4 FragColor;

uniform sampler2D particleTexture;
uniform sampler2D colorGradient;  // Optional color gradient based on lifetime
uniform int useColorGradient;     // Whether to use gradient (0: no, 1: yes)
uniform int particleType;         // Different particle types (0: default, 1: smoke, 2: fire, 3: muzzle)
uniform float softness;           // Edge softness factor

vec4 applySmoke() {
    vec4 texColor = texture(particleTexture, TexCoord);
    
    // Darken particles as they age
    float darkening = mix(1.0, 0.2, Lifetime);
    
    // Make particles more transparent as they age
    float alpha = texColor.a * (1.0 - Lifetime) * ParticleColor.a;
    
    return vec4(texColor.rgb * ParticleColor.rgb * darkening, alpha);
}

vec4 applyFire() {
    vec4 texColor = texture(particleTexture, TexCoord);
    
    // Fire color gradient: red/yellow to black
    vec3 startColor = ParticleColor.rgb;
    vec3 endColor = vec3(0.0, 0.0, 0.0);
    vec3 fireColor = mix(startColor, endColor, Lifetime);
    
    // Make particles more transparent as they age
    float alpha = texColor.a * pow(1.0 - Lifetime, 0.5) * ParticleColor.a;
    
    return vec4(texColor.rgb * fireColor, alpha);
}

vec4 applyMuzzleFlash() {
    vec4 texColor = texture(particleTexture, TexCoord);
    
    // Bright flash that quickly fades
    float intensity = pow(1.0 - Lifetime, 2.0) * 1.5;
    
    // Make flash quickly transparent
    float alpha = texColor.a * pow(1.0 - Lifetime, 1.5) * ParticleColor.a;
    
    return vec4(texColor.rgb * ParticleColor.rgb * intensity, alpha);
}

void main() {
    if (useColorGradient == 1) {
        // Use color gradient based on lifetime
        vec4 gradientColor = texture(colorGradient, vec2(Lifetime, 0.5));
        vec4 texColor = texture(particleTexture, TexCoord);
        FragColor = texColor * gradientColor * ParticleColor;
    } else {
        // Apply different effects based on particle type
        if (particleType == 1) {
            // Smoke
            FragColor = applySmoke();
        } else if (particleType == 2) {
            // Fire
            FragColor = applyFire();
        } else if (particleType == 3) {
            // Muzzle flash
            FragColor = applyMuzzleFlash();
        } else {
            // Default particles
            vec4 texColor = texture(particleTexture, TexCoord);
            FragColor = texColor * ParticleColor;
        }
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