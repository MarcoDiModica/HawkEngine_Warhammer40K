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
uniform vec3 cameraPos;
uniform int instanceOffset;
uniform float heightScale;
uniform int u_HasHeightMap;
uniform sampler2D heightMap;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    mat3 TBN;
    vec3 CameraPos;
    vec4 FragPosLightSpace;
} vs_out;

void main() {
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    vec4 tPos = vec4(position, 1.0);
    vec3 tNormal = normal;
    vec3 tTangent = tangent;
    vec3 tBitangent = bitangent;

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
            
            if (BoneTransform[3][3] == 0.0) {
                BoneTransform = mat4(1.0);
            }
            
            tPos = BoneTransform * vec4(position, 1.0);
            
            mat3 normalTransform = mat3(BoneTransform);
            tNormal = normalTransform * normal;
            tTangent = normalTransform * tangent;
            tBitangent = normalTransform * bitangent;
        }
    }
    
    vs_out.TexCoord = texCoord;
    
    vs_out.Normal = normalize(normalMatrix * tNormal);
    vs_out.Tangent = normalize(normalMatrix * tTangent);
    vs_out.Bitangent = normalize(normalMatrix * tBitangent);
    vs_out.TBN = mat3(vs_out.Tangent, vs_out.Bitangent, vs_out.Normal);

    vs_out.CameraPos = cameraPos;
    
    vec3 positionOffset = tPos.xyz;
    
    if (u_HasHeightMap == 1 && heightScale > 0.0) {
        float height = texture(heightMap, texCoord).r;
        positionOffset += tNormal * (height * heightScale);
    }
    
    vs_out.FragPos = vec3(model * vec4(positionOffset, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}