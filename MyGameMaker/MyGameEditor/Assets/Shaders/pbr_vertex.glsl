#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

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

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform int instanceOffset;
uniform float heightScale;
uniform int u_HasHeightMap;
uniform sampler2D heightMap;


out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    mat3 TBN;
    vec3 CameraPos;
} vs_out;

void main() {
    InstanceData data = instances[gl_InstanceID + instanceOffset];
    mat4 model = data.modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    vs_out.TexCoord = texCoord;
    
    vec3 N = normalize(normalMatrix * normal);

    vec3 T = vec3(0.0);
    vec3 B = vec3(0.0);

    if (length(tangent) > 0.0) {
        T = normalize(normalMatrix * tangent);
      
        T = normalize(T - dot(T, N) * N);
       
        B = normalize(cross(N, T));
    } else {
      
        T = normalize(cross(N, vec3(0.0, 1.0, 0.0)));
        if (length(T) < 0.01) {
            T = normalize(cross(N, vec3(1.0, 0.0, 0.0)));
        }
        B = normalize(cross(N, T));
    }
    vs_out.Normal = N;
    vs_out.Tangent = T;
    vs_out.Bitangent = B;
    vs_out.TBN = mat3(vs_out.Tangent, vs_out.Bitangent, vs_out.Normal);

    vs_out.CameraPos = cameraPos;
    
    vec3 positionOffset = position;
    
    if (u_HasHeightMap == 1 && heightScale > 0.0) {
        float height = texture(heightMap, texCoord).r;
        vec3 normal = normalize(normal);
        positionOffset += normal * (height * heightScale);
    }
    
    vs_out.FragPos = vec3(model * vec4(positionOffset, 1.0));
    
    gl_Position = projection * view * model * vec4(positionOffset, 1.0);
}