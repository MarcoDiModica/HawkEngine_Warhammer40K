#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

in vec2 TexCoord;
in vec3 Normal;
in flat uint MaterialID;
in flat uint InstanceID;

out vec4 FragColor;

// Modo clásico (no-bindless): uniforms
uniform sampler2D texture1;
uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
uniform int u_HasTexture = 0;

// Estructuras para modo bindless
struct GPUMaterial {
    vec4 albedoColor;
    vec4 pbrParams;
    uint64_t albedoTexture;
    uint64_t normalTexture;
    uint64_t metallicTexture;
    uint64_t roughnessTexture;
    uint64_t aoTexture;
    uint64_t emissiveTexture;
    uint flags;
    uint shaderType;
    float padding[2];
};

struct GPUInstance {
    mat4 modelMatrix;
    mat4 prevModelMatrix;
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
};

// Buffers para modo bindless
layout(std430, binding = 1) readonly buffer MaterialBuffer {
    GPUMaterial materials[];
};

layout(std430, binding = 2) readonly buffer InstanceBuffer {
    GPUInstance instances[];
};

// Flag para determinar si estamos en modo bindless
uniform int useBindlessMode = 0;

void main() {
    if (useBindlessMode == 1) {
        // Modo bindless
        GPUMaterial material = materials[MaterialID];
        GPUInstance instance = instances[InstanceID];
        
        // Combinar color del material con color de instancia
        vec4 finalColor = material.albedoColor * instance.objectData;
        
        // Verificar si tiene textura de albedo (flag bit 0)
        if ((material.flags & (1 << 0)) != 0) {
            sampler2D albedoMap = sampler2D(material.albedoTexture);
            vec4 texColor = texture(albedoMap, TexCoord);
            finalColor *= texColor;
        }
        
        FragColor = finalColor;
    }
    else {
        // Modo clásico (no-bindless)
        if (u_HasTexture == 1) {
            vec4 texColor = texture(texture1, TexCoord);
            FragColor = texColor * color;
        } else {
            FragColor = color;
        }
    }
}