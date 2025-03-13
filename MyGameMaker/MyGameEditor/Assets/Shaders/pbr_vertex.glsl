#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;
layout(location = 5) in vec3 aTangent;
layout(location = 6) in vec3 aBitangent;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int isAnimated = 0;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    // Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;

    // Calculate world-space fragment position
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Calculate normal matrix (properly handles non-uniform scaling)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    // Transform normal, tangent and bitangent to world space
    vec3 N = normalize(normalMatrix * aNormal);
    
    // Check if we have tangent/bitangent data
    vec3 T = vec3(0.0);
    vec3 B = vec3(0.0);
    
    // Only build TBN if we have valid tangent data
    if(length(aTangent) > 0.0) {
        T = normalize(normalMatrix * aTangent);
        // Re-orthogonalize T with respect to N
        T = normalize(T - dot(T, N) * N);
        // Calculate B from T and N for consistency
        B = normalize(cross(N, T));
    } else {
        // Create arbitrary TBN (less accurate but functional)
        T = normalize(cross(N, vec3(0.0, 1.0, 0.0)));
        if(length(T) < 0.01) {
            T = normalize(cross(N, vec3(1.0, 0.0, 0.0)));
        }
        B = normalize(cross(N, T));
    }
    
    // Output TBN matrix for normal mapping
    TBN = mat3(T, B, N);
    
    // Output regular normal for fallback
    Normal = N;
    vec4 tPos = vec4(aPos, 1.0);

    if (isAnimated == 1)
    {
    mat4 BoneTransform = finalBonesMatrices[ boneIds[0] ] * weights[0];
    BoneTransform += finalBonesMatrices[ boneIds[1] ] * weights[1];
    BoneTransform += finalBonesMatrices[ boneIds[2] ] * weights[2];
    BoneTransform += finalBonesMatrices[ boneIds[3] ] * weights[3];
    tPos = BoneTransform * vec4(aPos, 1.0);
    }
   



    // Transform vertex position
    gl_Position = projection * view * model * tPos;
}