#version 460 core
#extension GL_ARB_bindless_texture : enable

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec4 albedoColor;
uniform sampler2D albedoTexture;
uniform int u_HasTexture;
uniform bool useBindlessMode;

layout(location = 0) out vec4 FragColor;

void main() {
    vec4 baseColor = albedoColor;
    
    if (u_HasTexture == 1) {
        baseColor *= texture(albedoTexture, TexCoord); 
    }
    
    FragColor = vec4(baseColor.rgb, baseColor.a);
}