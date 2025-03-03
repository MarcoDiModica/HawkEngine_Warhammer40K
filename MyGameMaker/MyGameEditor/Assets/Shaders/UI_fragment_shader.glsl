#version 450 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    // Sample the texture color
    vec4 texColor = texture(texture1, TexCoord);

    // Set the fragment color
    FragColor = texColor;
}