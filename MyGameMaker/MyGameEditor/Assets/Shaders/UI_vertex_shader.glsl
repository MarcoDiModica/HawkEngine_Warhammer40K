#version 450 core

layout(location = 0) in vec3 aPos; // The position variable has attribute position 0
layout(location = 1) in vec2 aTexCoord; // The texture coordinate variable has attribute position 1

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
      vec4 worldPosition = model * vec4(aPos, 1.0);
    vec4 viewPosition = view * worldPosition;
    vec4 projectedPosition = projection * viewPosition;

    // Set the z coordinate to a small negative value to ensure it is in front of the camera
    gl_Position = vec4(projectedPosition.xy, -0.1, projectedPosition.w);

    // Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;
}