#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// Output to fragment shader
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
flat out uint InstanceIndex;  // Pass the instance index to fragment shader

// Global uniforms
uniform mat4 view;
uniform mat4 projection;
uniform bool useBindlessMode = false;
uniform int baseInstance = 0; // Fallback for systems without gl_BaseInstance

// Per-instance data - for fallback path
uniform mat4 model;
uniform uint materialIndexUniform = 0;

// Instance data structure - must match CPU-side GPUInstance
struct GPUInstance {
    mat4 modelMatrix;
    mat4 prevModelMatrix; 
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
};

// Instance data SSBO
layout(std430, binding = 1) buffer InstanceBuffer {
    GPUInstance instances[];
};

void main() {
    // Default values
    mat4 modelMatrix = model;  // Default to uniform
    InstanceIndex = materialIndexUniform;  // Default to uniform
    
    // Use SSBO approach if bindless mode is enabled
    if (useBindlessMode) {
        // Calculate instance index, preferring gl_BaseInstance if available
        uint instanceIdx = 0;
        
        // This preprocessor check handles hardware that may not support gl_BaseInstance
        #if __VERSION__ >= 460
            instanceIdx = baseInstance + gl_InstanceID;
        #else
            instanceIdx = baseInstance + gl_InstanceID;
        #endif
        
        // Get instance data from SSBO
        GPUInstance instance = instances[instanceIdx];
        modelMatrix = instance.modelMatrix;
        InstanceIndex = instance.materialIndex;
    }
    
    // Apply transformation
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    gl_Position = projection * view * worldPos;
    
    // Pass data to fragment shader
    TexCoord = texCoord;
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    FragPos = worldPos.xyz;
}