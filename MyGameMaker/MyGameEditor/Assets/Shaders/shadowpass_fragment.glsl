#version 450 core
#extension GL_ARB_bindless_texture : enable

in vec2 TexCoord;
in vec3 FragPos;

uniform vec4 albedoColor;
uniform sampler2D albedoTexture;
uniform int u_HasTexture;

layout(location = 0) out vec4 FragColor;

void main() {
   
}