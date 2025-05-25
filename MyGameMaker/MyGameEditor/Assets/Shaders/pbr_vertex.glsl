#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(location = 7) in ivec4 boneIds;
layout(location = 8) in vec4 weights;

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

    
        // Apply bone transformations to the position
        mat4 BoneTransform = data.boneMatrices[boneIds[0]] * weights[0];
        BoneTransform += data.boneMatrices[boneIds[1]] * weights[1];
        BoneTransform += data.boneMatrices[boneIds[2]] * weights[2];
        BoneTransform += data.boneMatrices[boneIds[3]] * weights[3];
        tPos = BoneTransform * vec4(position, 1.0);

        // Apply bone transformations to the normal
        tNormal = mat3(BoneTransform) * normal;
    


    vs_out.TexCoord = texCoord;
    
    vs_out.Normal = normalize(normalMatrix * normal);
    vs_out.Tangent = normalize(normalMatrix * tangent);
    vs_out.Bitangent = normalize(normalMatrix * bitangent);
    vs_out.TBN = mat3(vs_out.Tangent, vs_out.Bitangent, vs_out.Normal);

    vs_out.CameraPos = cameraPos;
    
    vec3 positionOffset = position;
    
    if (u_HasHeightMap == 1 && heightScale > 0.0) {
        float height = texture(heightMap, texCoord).r;
        vec3 normal = normalize(normal);
        positionOffset += normal * (height * heightScale);
    }
    vs_out.FragPos = vec3(model * vec4(positionOffset, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    
    gl_Position = projection * view * model * vec4(positionOffset, 1.0);
}