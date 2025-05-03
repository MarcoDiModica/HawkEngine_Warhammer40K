#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec4 albedoColor;
uniform sampler2D albedoTexture;
uniform int u_HasTexture;

layout(location = 0) out vec4 FragColor;

void main() {
    vec4 baseColor = albedoColor;
    
    if (u_HasTexture == 1) {
        baseColor *= texture(albedoTexture, TexCoord); 
    }
    
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(Normal, lightDir), 0.2);
    
    FragColor = vec4(baseColor.rgb * diff, baseColor.a);
}