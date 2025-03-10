#version 450 core

layout(location = 0) in vec3 aPos; // Position attribute
layout(location = 1) in vec2 aTexCoord; // Texture coordinate attribute
layout(location = 2) in vec3 aNormal; // Normal attribute
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

out vec2 TexCoord; // Texture coordinate of the fragment
out vec3 FragPos; // Position of the fragment
out vec3 Normal; // Normal of the fragment
out vec4 ExtraColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 aColor;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];


void main()
{
    // Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;

    // Pass the fragment position to the fragment shader
    FragPos = vec3(model * vec4(aPos, 1.0));

    ExtraColor = aColor;

    // Pass the normal to the fragment shader
    Normal = mat3(transpose(inverse(model))) * aNormal;

         mat4 BoneTransform = finalBonesMatrices[ boneIds[0] ] * weights[0];
    BoneTransform += finalBonesMatrices[ boneIds[1] ] * weights[1];
    BoneTransform += finalBonesMatrices[ boneIds[2] ] * weights[2];
    BoneTransform += finalBonesMatrices[ boneIds[3] ] * weights[3];

	vec4 tPos = BoneTransform * vec4(aPos, 1.0);

    // Transform the vertex position to clip space
    gl_Position = (projection * view * model) * tPos;
    //Normal = normalize(totalNormal);
}