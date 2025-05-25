#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

struct InstanceData {
    mat4 modelMatrix;
    mat4 prevModelMatrix;
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
    mat4 boneMatrices[200];
};

layout(std430, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
};

uniform mat4 view;
uniform mat4 projection;
uniform int instanceOffset;

out vec2 TexCoord;
out vec3 Normal;

void main()
{
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    Normal = aNormal;
}