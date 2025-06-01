#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

struct InstanceData {
    mat4 modelMatrix;
    mat4 prevModelMatrix;
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
    uint boneOffset;
    uint boneCount;
    uint padding[2];
};

layout(std430, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[]; 
};

layout(std430, binding = 7) readonly buffer BoneMatricesBuffer {
    mat4 boneMatrices[];
};

uniform mat4 view;
uniform mat4 projection;
uniform int instanceOffset;

out vec2 TexCoord;
out vec3 FragPos;

void main() {
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;
    
    vec4 animatedPos = vec4(position, 1.0);
    
    if ((data.flags & 1u) == 1u) {
        uint boneBase = data.boneOffset;
        
        vec4 normalizedWeights = weights;
        float weightSum = weights[0] + weights[1] + weights[2] + weights[3];
        
        if (weightSum > 0.0) {
            normalizedWeights /= weightSum;
            
            mat4 BoneTransform = mat4(0.0);
            
            for (int i = 0; i < 4; i++) {
                if (normalizedWeights[i] > 0.001) {
                    uint boneIndex = uint(boneIds[i]);
                    if (boneIndex < data.boneCount) {
                        BoneTransform += boneMatrices[boneBase + boneIndex] * normalizedWeights[i];
                    }
                }
            }
            
            if (BoneTransform[3][3] != 0.0) {
                animatedPos = BoneTransform * vec4(position, 1.0);
            }
        }
    }
    
    gl_Position = projection * view * model * animatedPos;
    TexCoord = texCoord;
    FragPos = vec3(model * animatedPos);
}