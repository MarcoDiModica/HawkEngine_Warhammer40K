#version 460 core

layout (location = 0) in vec3 aPos;

struct InstanceData {
    mat4 modelMatrix;
    mat4 prevModelMatrix;
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
};

layout(std430, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[]; 
};

uniform int instanceOffset;
uniform mat4 lightSpaceMatrix;

void main()
{
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}  