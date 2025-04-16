#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

// Atributos de vértice originales
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

// Salida al fragment shader
out vec2 TexCoord;
out vec3 Normal;
out flat uint MaterialID;
out flat uint InstanceID;

// Matrices de la cámara
uniform mat4 view;
uniform mat4 projection;

// Estructuras para modo bindless
struct GPUInstance {
    mat4 modelMatrix;
    mat4 prevModelMatrix; // Para motion blur
    vec4 objectData;      // Datos adicionales del objeto
    uint meshIndex;       // Índice a los datos de malla
    uint materialIndex;   // Índice a los datos de material
    uint objectId;        // ID único del objeto
    uint flags;           // Flags de configuración
};

// Buffer de instancias (solo usado en modo bindless)
layout(std430, binding = 2) readonly buffer InstanceBuffer {
    GPUInstance instances[];
};

// Matriz de modelo (solo usada en modo no-bindless)
uniform mat4 model;

// Flag para determinar si estamos en modo bindless
uniform int useBindlessMode = 0;

void main() {
    // Inicialización de las variables de salida
    TexCoord = aTexCoord;
    
    if (useBindlessMode == 1) {
        // Modo bindless: obtener datos de la instancia
        uint instanceID = gl_BaseInstance + gl_InstanceID;
        InstanceID = instanceID;
        
        GPUInstance instance = instances[instanceID];
        MaterialID = instance.materialIndex;
        
        // Usar matriz del modelo de la instancia
        mat4 modelMatrix = instance.modelMatrix;
        
        // Transformar posición y normal
        gl_Position = projection * view * modelMatrix * vec4(aPos, 1.0);
        mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
        Normal = normalMatrix * aNormal;
    }
    else {
        // Modo clásico: usar uniforms
        InstanceID = 0;
        MaterialID = 0;
        
        // Usar matriz del modelo proporcionada por uniform
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        mat3 normalMatrix = mat3(transpose(inverse(model)));
        Normal = normalMatrix * aNormal;
    }
}