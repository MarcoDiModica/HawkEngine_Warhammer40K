#version 450 core

in vec2 TexCoord; // Texture coordinate of the fragment
in vec3 FragPos; // Position of the fragment
in vec3 Normal; // Normal of the fragment

out vec4 FragColor;

uniform vec3 u_ColorLow;
uniform vec3 u_ColorHigh;
uniform sampler2D texture1; // Texture sampler
uniform vec4 modColor; // New uniform for color modifier

void main() {

	// Sample the texture using the texture coordinates
    vec4 texColor = texture(texture1, TexCoord);
	FragColor = texColor;

    //float height = FragPos.y / 100.0; // Escala la altura.
    //vec3 color = mix(u_ColorLow, u_ColorHigh, height);
    //FragColor = vec4(color, 1.0);
}