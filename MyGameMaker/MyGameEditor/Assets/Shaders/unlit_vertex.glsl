#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 boneWeights;

struct InstanceData {
    mat4 modelMatrix;
    mat4 prevModelMatrix;
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
    //mat4 boneMatrices[200];
};

layout(std430, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[]; 
};

uniform mat4 view;
uniform mat4 projection;
uniform int instanceOffset;

out vec2 TexCoord;
out vec3 FragPos;

void main() {
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;
    
    gl_Position = projection * view * model * vec4(position, 1.0);
    
    TexCoord = texCoord;
    
    FragPos = vec3(model * vec4(position, 1.0));
}