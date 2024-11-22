#version 450 core

in vec2 TexCoord; // Texture coordinate of the fragment

out vec4 FragColor;

uniform sampler2D texture1; // Texture sampler

void main()
{
    // Sample the texture using the texture coordinates
    FragColor = texture(texture1, TexCoord);
}