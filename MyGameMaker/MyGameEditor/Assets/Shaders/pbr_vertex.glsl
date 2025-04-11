#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;
layout(location = 5) in vec3 aTangent;
layout(location = 6) in vec3 aBitangent;

layout(location = 7) in vec4 instanceMatrix0;
layout(location = 8) in vec4 instanceMatrix1;
layout(location = 9) in vec4 instanceMatrix2;
layout(location = 10) in vec4 instanceMatrix3;
layout(location = 11) in vec4 instanceColor;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out mat3 TBN;
out vec4 vInstanceColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int isAnimated = 0;
uniform int isInstanced = 0;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    TexCoord = aTexCoord;
    vInstanceColor = isInstanced == 1 ? instanceColor : vec4(1.0);

    vec4 tPos = vec4(aPos, 1.0);
    vec3 tNormal = aNormal;

    if (isAnimated == 1)
    {
        mat4 BoneTransform = weights[0] * finalBonesMatrices[boneIds[0]];
        BoneTransform += weights[1] * finalBonesMatrices[boneIds[1]];
        BoneTransform += weights[2] * finalBonesMatrices[boneIds[2]];
        BoneTransform += weights[3] * finalBonesMatrices[boneIds[3]];
        
        tPos = BoneTransform * tPos;
        tNormal = mat3(BoneTransform) * tNormal;
    }

    mat4 modelMatrix = isInstanced == 1 ? 
        mat4(instanceMatrix0, instanceMatrix1, instanceMatrix2, instanceMatrix3) : 
        model;

    FragPos = vec3(modelMatrix * tPos);
    
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 N = normalize(normalMatrix * tNormal);
    Normal = N;
    
    vec3 T;
    if (length(aTangent) > 0.0) {
        T = normalize(normalMatrix * aTangent);
        T = normalize(T - dot(T, N) * N);
    } else {
        vec3 tempVec = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        T = normalize(cross(tempVec, N));
    }
    
    vec3 B = normalize(cross(N, T));
    
    TBN = mat3(T, B, N);
    gl_Position = projection * view * modelMatrix * tPos;
}