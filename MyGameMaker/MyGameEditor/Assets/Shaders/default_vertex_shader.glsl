#version 450 core

layout(location = 0) in vec3 aPos; // Position attribute
layout(location = 1) in vec2 aTexCoord; // Texture coordinate attribute
layout(location = 2) in vec3 aNormal; // Normal attribute
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

out vec2 TexCoord; // Texture coordinate of the fragment
out vec3 Normal; // Normal of the fragment

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 20;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
       vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    }
		

    gl_Position = projection * view * model * totalPosition;
    TexCoord = aTexCoord;
};