#version 460 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
uniform bool useTexture = false;

void main() {
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoord);
        FragColor = texColor * color;
    } else {
        FragColor = color;
    }
}