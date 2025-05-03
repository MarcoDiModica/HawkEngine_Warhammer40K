#version 460 core
#extension GL_ARB_bindless_texture : require // Use require for safety
#extension GL_ARB_gpu_shader_int64 : require   // For GLuint64 handles
#extension GL_ARB_shader_storage_buffer_object : require

// Input from vertex shader
in vec2 TexCoords;
in vec3 FragPos;     // Usually not needed for simple unlit, but kept
in vec3 Normal;      // Usually not needed for simple unlit, but kept
layout(location = 3) flat in uint vs_materialIndex; // Receive material index

// Output color
out vec4 FragColor;

// SSBO Structure (match C++ struct in BindlessManager.h)
// Include fields needed for fragment shading
struct GPUMaterial {
    vec4 albedoColor;
    vec4 pbrParams;         // x: metallic, y: roughness, z: ao, w: emissive
    uint64_t albedoTexture; // Bindless handle for albedo texture
    uint64_t normalTexture; // Bindless handle for normal texture
    uint64_t metallicTexture; // Bindless handle for metallic texture
    uint64_t roughnessTexture; // Bindless handle for roughness texture
    uint64_t aoTexture; // Bindless handle for ambient occlusion texture
    uint64_t emissiveTexture; // Bindless handle for emissive texture
    uint flags;             // Bit flags for material properties
    uint shaderType;        // Shader type (e.g., PBR, UNLIT, etc.)
    float padding[2];       // Padding for alignment
};

// SSBO Binding (Adjust binding point as needed)
layout(std430, binding = 1) buffer MaterialBuffer { GPUMaterial materials[]; }; // Assuming binding = 1 for materials

void main()
{
    // Fetch material data using the index from the vertex shader
    GPUMaterial materialData = materials[vs_materialIndex];

    // Base color from material
    vec4 finalColor = materialData.albedoColor;

    // Check the flag to see if an albedo texture should be used
    // (Assuming bit 0 of flags means "has albedo texture")
    bool hasAlbedoTexture = (materialData.flags & (1 << 0)) != 0;

    if (hasAlbedoTexture) {
        // Get the bindless handle
        uint64_t handle = materialData.albedoTexture;

        // Check if handle is valid before creating sampler (optional safety)
        if (handle != 0) {
             // Construct the sampler using the bindless handle
            sampler2D bindlessAlbedoSampler = sampler2D(handle);

            // Sample the texture
            vec4 texColor = texture(bindlessAlbedoSampler, TexCoords);

            // Discard fragment if alpha is low (optional)
            if (texColor.a < 0.1)
                 discard;

            // Combine texture color with material's base color
            // (Commonly multiplication: texColor * albedoColor)
            finalColor = texColor * materialData.albedoColor;
        }
        // else: Handle case where flag is set but handle is 0? Maybe fallback?
    }
    // else: No texture, just use materialData.albedoColor

    FragColor = finalColor;
}