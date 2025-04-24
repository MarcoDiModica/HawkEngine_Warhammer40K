#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : enable

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
flat in uint InstanceIndex;  // Receive instance index from vertex shader

out vec4 FragColor;

// Material data structure - must match CPU-side GPUMaterial
struct GPUMaterial {
    vec4 albedoColor;
    vec4 pbrParams;    // x: metallic, y: roughness, z: ao, w: emissive
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

// Material data SSBO
layout(std430, binding = 3) buffer MaterialBuffer {
    GPUMaterial materials[];
};

// For non-MDI fallback path
uniform uint materialIndex;
uniform bool useBindlessMode = false;
uniform sampler2D albedoTexture;  // Traditional texture unit for non-bindless

// Bindless texture function with fallback for hardware without uint64_t support
vec4 sampleBindlessTexture(uint64_t handle, vec2 uv) {
    if (handle == 0) {
        return vec4(1.0, 0.0, 1.0, 1.0); // Fallback magenta color
    }
    
    // We can't use preprocessor here because the extension check happens at runtime
    // But we handle systems without proper bindless texture support with the useBindlessMode flag
    return texture(sampler2D(handle), uv);
}

void main() {
    // Get the material data based on what's available
    GPUMaterial material;
    
    if (useBindlessMode) {
        // Use the instance index passed from vertex shader
        material = materials[InstanceIndex];
    } else {
        // Fallback path using uniform
        material = materials[materialIndex];
    }
    
    // Base color (either from texture or material color)
    vec4 color = material.albedoColor;
    
    // If has albedo texture
    if ((material.flags & (1 << 0)) != 0) {
        if (useBindlessMode) {
            // Bindless texture path
            vec4 texColor = sampleBindlessTexture(material.albedoTexture, TexCoord);
            color *= texColor;
        } else {
            // Traditional texture path
            color *= texture(albedoTexture, TexCoord);
        }
    }
    
    FragColor = color;
}