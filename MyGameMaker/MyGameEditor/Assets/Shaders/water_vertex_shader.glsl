#version 450 core
layout(location = 0) in vec3 aPos; // Position attribute
layout(location = 1) in vec2 aTexCoord; // Texture coordinate attribute
layout(location = 2) in vec3 aNormal; // Normal attribute

out vec2 TexCoord; // Texture coordinate of the fragment
out vec3 FragPos; // Position of the fragment
out vec3 Normal; // Normal of the fragment
out float offset;

uniform float u_Time;
uniform float u_Amplitude;
uniform float u_Frequency;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	// Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;

    // Pass the fragment position to the fragment shader
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Pass the normal to the fragment shader
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Transform the vertex position to clip space
    vec3 newPos = aPos;
    // Olas en X y Z
    newPos.y += sin(newPos.x * u_Frequency + u_Time) * u_Amplitude; // Olas X
    newPos.y += sin(newPos.z * u_Frequency + u_Time * 1.3) * u_Amplitude; // Olas Z con desfase

    // Olas diagonales
    newPos.y += sin((newPos.x + newPos.z) * u_Frequency + u_Time) * u_Amplitude * 0.5; // ↘
    newPos.y += sin((newPos.x - newPos.z) * u_Frequency + u_Time) * u_Amplitude * 0.5; // ↙
    gl_Position = projection * view * model * vec4(newPos, 1.0);
	offset = aPos.y -  newPos.y;
}