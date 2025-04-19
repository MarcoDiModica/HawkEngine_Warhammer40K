#version 430 core

layout(local_size_x = 64) in;

// Estructura para los datos de culling
struct CullData {
    vec4 boundingSphere;  // xyz = centro, w = radio
    uint drawID;          // ID del comando de dibujado
    uint meshIndex;       // Índice de la malla
    uint instanceOffset;  // Offset en el buffer de instancias
    uint instanceCount;   // Número de instancias
    uint materialIndex;   // Índice del material
};

// Estructura para los comandos de dibujo indirecto
struct DrawElementsCommand {
    uint count;           // Número de índices
    uint instanceCount;   // Número de instancias
    uint firstIndex;      // Primer índice
    int baseVertex;       // Base para indexación de vértices
    uint baseInstance;    // Base para indexación de instancias
};

// Estructura para los datos de la malla
struct GPUMesh {
    uint vertexArray;     // VAO (no usado en compute shader)
    uint indexBuffer;     // Buffer de índices (no usado en compute shader)
    uint positionBuffer;  // Buffer de posiciones
    uint texCoordBuffer;  // Buffer de coordenadas de textura
    uint normalBuffer;    // Buffer de normales
    uint tangentBuffer;   // Buffer de tangentes
    uint bitangentBuffer; // Buffer de bitangentes
    uint colorBuffer;     // Buffer de colores
    uint indexCount;      // Número de índices
    uint vertexCount;     // Número de vértices
    uint meshId;          // ID único de la malla
    uint attributeFlags;  // Flags para indicar qué atributos están disponibles
};

// Estructura para las instancias
struct GPUInstance {
    mat4 modelMatrix;
    mat4 prevModelMatrix; // Para motion blur
    vec4 objectData;      // Datos adicionales del objeto
    uint meshIndex;       // Índice a los datos de malla
    uint materialIndex;   // Índice a los datos de material
    uint objectId;        // ID único del objeto
    uint flags;           // Flags de configuración
};

// Frustum definido por 6 planos (left, right, bottom, top, near, far)
struct Frustum {
    vec4 planes[6];       // Planos del frustum (normal, distancia)
};

// Buffers de entrada/salida
layout(std430, binding = 0) readonly buffer CullDataBuffer {
    CullData cullData[];
};

layout(std430, binding = 1) buffer DrawCommandBuffer {
    DrawElementsCommand drawCommands[];
};

layout(std430, binding = 2) buffer VisibleCountBuffer {
    uint visibleCount;
};

layout(std430, binding = 3) readonly buffer MeshBuffer {
    GPUMesh meshes[];
};

layout(std430, binding = 4) readonly buffer InstanceBuffer {
    GPUInstance instances[];
};

// Uniforms
uniform Frustum u_frustum;
uniform mat4 u_viewMatrix;
uniform mat4 u_projMatrix;
uniform vec3 u_cameraPosition;
uniform int u_useFrustumCulling;
uniform int u_useOcclusionCulling;
uniform float u_maxDrawDistance;

// Funciones auxiliares
bool sphereInFrustum(vec3 center, float radius, Frustum frustum) {
    for (int i = 0; i < 6; i++) {
        vec4 plane = frustum.planes[i];
        float distance = dot(vec3(plane), center) + plane.w;
        if (distance < -radius) {
            return false;
        }
    }
    return true;
}

float distanceToCamera(vec3 center) {
    return distance(center, u_cameraPosition);
}

bool isVisible(CullData cullData) {
    vec3 center = cullData.boundingSphere.xyz;
    float radius = cullData.boundingSphere.w;
    
    // Culling por distancia
    if (distanceToCamera(center) > u_maxDrawDistance + radius) {
        return false;
    }
    
    // Culling por frustum
    if (u_useFrustumCulling != 0) {
        if (!sphereInFrustum(center, radius, u_frustum)) {
            return false;
        }
    }
    
    // Aquí se podría implementar occlusion culling
    // (requeriría buffers adicionales para depth testing)
    
    return true;
}

void main() {
    // Obtener el índice global de trabajo
    uint gID = gl_GlobalInvocationID.x;
    
    // Asegurarse de no exceder el número de elementos a procesar
    if (gID >= cullData.length()) {
        return;
    }
    
    // Inicializar comandos con cero instancias
    drawCommands[gID].instanceCount = 0;
    
    // Procesar el cullData actual
    CullData current = cullData[gID];
    
    // Realizar culling
    if (isVisible(current)) {
        // Obtener datos de la malla
        GPUMesh mesh = meshes[current.meshIndex];
        
        // Configurar el comando de dibujo
        drawCommands[gID].count = mesh.indexCount;
        drawCommands[gID].instanceCount = current.instanceCount;
        drawCommands[gID].firstIndex = 0;
        drawCommands[gID].baseVertex = 0;
        drawCommands[gID].baseInstance = current.instanceOffset;
        
        // Incrementar el contador de visibles (de manera atómica)
        atomicAdd(visibleCount, current.instanceCount);
    }
}