#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

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

uniform int instanceOffset;
uniform mat4 lightSpaceMatrix;

void main()
{
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;
    
    vec4 animatedPos = vec4(aPos, 1.0);
    
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
                animatedPos = BoneTransform * vec4(aPos, 1.0);
            }
        }
    }
    
    gl_Position = lightSpaceMatrix * model * animatedPos;
}