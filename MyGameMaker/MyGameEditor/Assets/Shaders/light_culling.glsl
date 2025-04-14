#version 450 core
layout(local_size_x = 16, local_size_y = 16) in;

// Estructura para luz puntual
struct PointLight {
    vec4 position;    // xyz = posición, w = radio
    vec4 color;       // rgb = color, a = intensidad
    vec4 attenuation; // x = constante, y = lineal, z = cuadrática, w = no usado
    uint lightType;   // 0 = point, 1 = spot (futuro)
    uint castShadow;  // 0 = no, 1 = yes
    uint shadowMapIndex; // índice a shadowmap (si usa sombras)
    uint padding;     // alineación
};

// Buffer de luces puntuales
layout(std430, binding = 0) readonly buffer PointLightBuffer {
    PointLight lights[];
};

// Buffer para grid de tiles
layout(std430, binding = 1) buffer LightGridBuffer {
    uvec2 data[];     // x = offset into light indices, y = light count
} lightGrid;

// Buffer para índices de luces por tile
layout(std430, binding = 2) buffer LightIndicesBuffer {
    uint indices[];
} lightIndices;

// Uniforms
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform int numLights;
uniform int tileSize;
uniform int maxLightsPerTile;
uniform ivec2 screenSize;

// Variables compartidas
shared uint visibleLightCount;
shared uint visibleLightIndices[64]; // Usar maxLightsPerTile pero 64 es un buen valor base

// Estructura para planos de frustum
struct Plane {
    vec3 normal;
    float distance;
};

// Verifica si la esfera está delante del plano
bool sphereInsidePlane(vec4 sphere, Plane plane) {
    return dot(plane.normal, sphere.xyz) - plane.distance > -sphere.w;
}

void main() {
    // Obtener ID de tile
    uvec2 tileID = gl_WorkGroupID.xy;
    uint tileIndex = tileID.y * gl_NumWorkGroups.x + tileID.x;
    
    // Esquinas del tile en espacio de pantalla
    ivec2 tileStart = ivec2(tileID) * tileSize;
    ivec2 tileEnd = min(tileStart + ivec2(tileSize), screenSize);
    
    // Inicializar contador de luces visibles
    if (gl_LocalInvocationIndex == 0) {
        visibleLightCount = 0;
    }
    
    barrier();
    
    // Construir frustum para este tile
    vec2 tileSizeNDC = 2.0 * vec2(tileSize) / vec2(screenSize);
    vec2 tileStartNDC = 2.0 * vec2(tileStart) / vec2(screenSize) - 1.0;
    
    vec4 frustumCorners[8];
    // Esquinas near plane
    frustumCorners[0] = vec4(tileStartNDC.x, tileStartNDC.y, -1.0, 1.0);
    frustumCorners[1] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y, -1.0, 1.0);
    frustumCorners[2] = vec4(tileStartNDC.x, tileStartNDC.y + tileSizeNDC.y, -1.0, 1.0);
    frustumCorners[3] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y + tileSizeNDC.y, -1.0, 1.0);
    // Esquinas far plane
    frustumCorners[4] = vec4(tileStartNDC.x, tileStartNDC.y, 1.0, 1.0);
    frustumCorners[5] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y, 1.0, 1.0);
    frustumCorners[6] = vec4(tileStartNDC.x, tileStartNDC.y + tileSizeNDC.y, 1.0, 1.0);
    frustumCorners[7] = vec4(tileStartNDC.x + tileSizeNDC.x, tileStartNDC.y + tileSizeNDC.y, 1.0, 1.0);
    
    // Transformar a world space
    mat4 invViewProj = inverse(projMatrix * viewMatrix);
    for (int i = 0; i < 8; i++) {
        frustumCorners[i] = invViewProj * frustumCorners[i];
        frustumCorners[i] /= frustumCorners[i].w;
    }
    
    // Calcular planos del frustum (en world space)
    Plane frustumPlanes[6];
    
    // Plano cerca (near)
    vec3 a, b, c, normal;
    float d;
    
    // Plano cerca (near)
    a = frustumCorners[0].xyz;
    b = frustumCorners[1].xyz;
    c = frustumCorners[2].xyz;
    normal = normalize(cross(c - a, b - a));
    d = dot(normal, a);
    frustumPlanes[0] = Plane(normal, d);
    
    // Plano lejos (far)
    a = frustumCorners[4].xyz;
    b = frustumCorners[6].xyz;
    c = frustumCorners[5].xyz;
    normal = normalize(cross(c - a, b - a));
    d = dot(normal, a);
    frustumPlanes[1] = Plane(normal, d);
    
    // Plano izquierda (left)
    a = frustumCorners[0].xyz;
    b = frustumCorners[2].xyz;
    c = frustumCorners[4].xyz;
    normal = normalize(cross(c - a, b - a));
    d = dot(normal, a);
    frustumPlanes[2] = Plane(normal, d);
    
    // Plano derecha (right)
    a = frustumCorners[1].xyz;
    b = frustumCorners[5].xyz;
    c = frustumCorners[3].xyz;
    normal = normalize(cross(c - a, b - a));
    d = dot(normal, a);
    frustumPlanes[3] = Plane(normal, d);
    
    // Plano abajo (bottom)
    a = frustumCorners[0].xyz;
    b = frustumCorners[4].xyz;
    c = frustumCorners[1].xyz;
    normal = normalize(cross(c - a, b - a));
    d = dot(normal, a);
    frustumPlanes[4] = Plane(normal, d);
    
    // Plano arriba (top)
    a = frustumCorners[2].xyz;
    b = frustumCorners[3].xyz;
    c = frustumCorners[6].xyz;
    normal = normalize(cross(c - a, b - a));
    d = dot(normal, a);
    frustumPlanes[5] = Plane(normal, d);
    
    // Recorrer todas las luces y verificar intersección con frustum
    uint lightIndexStart = gl_LocalInvocationIndex;
    uint lightIndexStep = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
    
    for (uint i = lightIndexStart; i < numLights; i += lightIndexStep) {
        vec4 lightSphere = vec4(lights[i].position.xyz, lights[i].position.w);
        
        // Verificar si la luz intersecta con el frustum
        bool isVisible = true;
        for (int j = 0; j < 6; j++) {
            if (!sphereInsidePlane(lightSphere, frustumPlanes[j])) {
                isVisible = false;
                break;
            }
        }
        
        // Si es visible, añadir a la lista compartida
        if (isVisible) {
            uint index = atomicAdd(visibleLightCount, 1);
            if (index < 64) { // Usar maxLightsPerTile en lugar de 64
                visibleLightIndices[index] = i;
            }
        }
    }
    
    barrier();
    
    // Solo el primer thread por grupo escribe los resultados
    if (gl_LocalInvocationIndex == 0) {
        uint count = min(visibleLightCount, 64u); // Usar maxLightsPerTile en lugar de 64
        
        // Escribir información del tile en grid
        uint offset = tileIndex * maxLightsPerTile;
        lightGrid.data[tileIndex] = uvec2(offset, count);
        
        // Escribir índices de luces para este tile
        for (uint i = 0; i < count; i++) {
            lightIndices.indices[offset + i] = visibleLightIndices[i];
        }
    }
}