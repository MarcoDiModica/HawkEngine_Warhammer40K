#version 460 core
#extension GL_ARB_bindless_texture : require

// Entradas de vértices
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIds;
layout(location = 6) in vec4 aBoneWeights;

// Datos de instancia desde buffer de almacenamiento
struct InstanceData {
    mat4 modelMatrix;
    mat4 prevModelMatrix;
    vec4 objectData;
    uint meshIndex;
    uint materialIndex;
    uint objectId;
    uint flags;
};

layout(std430, binding = 2) readonly buffer InstanceBuffer {
    InstanceData instances[];
};

// Salida al fragment shader
out VS_OUT {
    vec2 TexCoord;
    vec3 WorldPos;
    vec3 Normal;
    mat3 TBN;
    flat uint MaterialIndex;
    flat uint ObjectId;
} vs_out;

// Uniforms
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

void main() {
    // Obtener datos de la instancia por índice
    mat4 model = instances[gl_InstanceID].modelMatrix;
    uint materialIndex = instances[gl_InstanceID].materialIndex;
    uint objectId = instances[gl_InstanceID].objectId;
    
    // Calcular posición de vértice
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // Calcular matriz para normales
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    // Calcular normal transformada
    vec3 normal = normalize(normalMatrix * aNormal);
    
    // Calcular tangente y bitangente si existe información
    vec3 T = vec3(0.0), B = vec3(0.0);
    mat3 tbn = mat3(1.0);
    
    if (length(aTangent) > 0.001) {
        // Usando datos de tangente/bitangente del modelo
        T = normalize(normalMatrix * aTangent);
        
        // Aplicar Gram-Schmidt para ortogonalización
        T = normalize(T - dot(T, normal) * normal);
        B = normalize(cross(normal, T));
        
        tbn = mat3(T, B, normal);
    }
    else {
        // Generar base TBN básica si no hay datos de tangente
        vec3 c1 = cross(normal, vec3(0.0, 0.0, 1.0));
        vec3 c2 = cross(normal, vec3(0.0, 1.0, 0.0));
        
        // Usar el eje perpendicular más adecuado
        T = length(c1) > length(c2) ? normalize(c1) : normalize(c2);
        B = normalize(cross(normal, T));
        
        tbn = mat3(T, B, normal);
    }
    
    // Pasar datos al fragment shader
    vs_out.TexCoord = aTexCoord;
    vs_out.WorldPos = worldPos.xyz;
    vs_out.Normal = normal;
    vs_out.TBN = tbn;
    vs_out.MaterialIndex = materialIndex;
    vs_out.ObjectId = objectId;
    
    // Posición final del vértice
    gl_Position = projection * view * worldPos;
}