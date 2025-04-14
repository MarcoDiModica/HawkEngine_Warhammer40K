#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require

// Entrada desde vertex shader
in VS_OUT {
    vec2 TexCoord;
    vec3 WorldPos;
    vec3 Normal;
    mat3 TBN;
    flat uint MaterialIndex;
    flat uint ObjectId;
} fs_in;

// Salida
layout(location = 0) out vec4 FragColor;
layout(location = 1) out uint ObjectIdBuffer; // Para picking/post-procesamiento

// Estructura de material en GPU
struct GPUMaterial {
    vec4 albedoColor;
    vec4 pbrParams;    // x: metallic, y: roughness, z: ao, w: emissive
    uint64_t albedoTexture;
    uint64_t normalTexture;
    uint64_t metallicTexture;
    uint64_t roughnessTexture;
    uint64_t aoTexture;
    uint64_t emissiveTexture;
    uint flags;       // Bit flags para configuración
    float padding[3]; // Padding para alineación
};

// Buffer de materiales
layout(std430, binding = 0) readonly buffer MaterialBuffer {
    GPUMaterial materials[];
};

// Estructuras para luces
struct GPUPointLight {
    vec4 position;    // xyz = posición, w = radio
    vec4 color;       // rgb = color, a = intensidad
    vec4 attenuation; // x = constante, y = lineal, z = cuadrática, w = no usado
    uint lightType;   // 0 = point, 1 = spot (futuro)
    uint castShadow;  // 0 = no, 1 = yes
    uint shadowMapIndex; // índice a shadowmap (si usa sombras)
    uint padding;     // alineación
};

struct GPUDirectionalLight {
    vec4 direction;   // xyz = dirección, w = intensidad
    vec4 color;       // rgb = color, a = no usado
    mat4 shadowMatrix;
    uint castShadow;
    uint shadowMapIndex;
    uint useCascades;
    uint numCascades;
};

// Buffer de luces
layout(std430, binding = 3) readonly buffer PointLightBuffer {
    GPUPointLight pointLights[];
};

layout(std430, binding = 4) readonly buffer DirectionalLightBuffer {
    GPUDirectionalLight directionalLight;
};

// Información de tiles para Forward+
layout(std430, binding = 5) readonly buffer LightGridBuffer {
    uvec2 tileData[]; // x = offset, y = count
};

// Índices de luces por tile
layout(std430, binding = 6) readonly buffer LightIndicesBuffer {
    uint indices[];
};

// Uniforms
uniform vec3 viewPos;
uniform vec2 screenSize;
uniform int tileSize;
uniform bool useForwardPlus = true;

// Constantes
const float PI = 3.14159265359;

// Funciones PBR
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Función principal
void main() {
    // Verificar si el material es válido
    if (fs_in.MaterialIndex >= materials.length()) {
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); // Magenta para material inválido
        ObjectIdBuffer = fs_in.ObjectId;
        return;
    }
    
    // Obtener datos del material
    GPUMaterial material = materials[fs_in.MaterialIndex];
    
    // Valores base PBR
    vec4 albedoColor = material.albedoColor;
    float metallic = material.pbrParams.x;
    float roughness = material.pbrParams.y;
    float ao = material.pbrParams.z;
    float emissive = material.pbrParams.w;
    
    // Obtener valores de texturas si están disponibles
    if ((material.flags & (1 << 0)) != 0 && material.albedoTexture != 0) {
        sampler2D albedoMap = sampler2D(material.albedoTexture);
        albedoColor *= texture(albedoMap, fs_in.TexCoord);
    }
    
    // Normal mapping
    vec3 N = normalize(fs_in.Normal);
    if ((material.flags & (1 << 1)) != 0 && material.normalTexture != 0) {
        sampler2D normalMap = sampler2D(material.normalTexture);
        vec3 normalSample = texture(normalMap, fs_in.TexCoord).rgb * 2.0 - 1.0;
        N = normalize(fs_in.TBN * normalSample);
    }
    
    // Otras texturas PBR
    if ((material.flags & (1 << 2)) != 0 && material.metallicTexture != 0) {
        sampler2D metallicMap = sampler2D(material.metallicTexture);
        metallic = texture(metallicMap, fs_in.TexCoord).r;
    }
    
    if ((material.flags & (1 << 3)) != 0 && material.roughnessTexture != 0) {
        sampler2D roughnessMap = sampler2D(material.roughnessTexture);
        roughness = texture(roughnessMap, fs_in.TexCoord).r;
    }
    
    if ((material.flags & (1 << 4)) != 0 && material.aoTexture != 0) {
        sampler2D aoMap = sampler2D(material.aoTexture);
        ao = texture(aoMap, fs_in.TexCoord).r;
    }
    
    // Ajustar valores
    roughness = max(roughness, 0.04);
    
    // Early discard para fragmentos totalmente transparentes
    if (albedoColor.a < 0.01) {
        discard;
    }
    
    // Cálculos PBR
    vec3 V = normalize(viewPos - fs_in.WorldPos);
    vec3 R = reflect(-V, N);
    
    // Reflectividad a incidencia normal
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoColor.rgb, metallic);
    
    // Color final de iluminación
    vec3 Lo = vec3(0.0);
    
    // Luz direccional (siempre presente)
    {
        vec3 L = normalize(-directionalLight.direction.xyz);
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);
        
        vec3 radiance = directionalLight.color.rgb * directionalLight.direction.w;
        
        // BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        Lo += (kD * albedoColor.rgb / PI + specular) * radiance * NdotL;
    }
    
    // Luces puntuales con Forward+
    if (useForwardPlus) {
        // Determinar el tile actual
        ivec2 tilePos = ivec2(gl_FragCoord.xy) / tileSize;
        uint tileIdx = tilePos.y * uint(screenSize.x / tileSize) + tilePos.x;
        
        // Obtener información del tile
        uvec2 tileInfo = tileData[tileIdx];
        uint offset = tileInfo.x;
        uint count = tileInfo.y;
        
        // Procesar las luces asignadas a este tile
        for (uint i = 0; i < count; i++) {
            uint lightIdx = indices[offset + i];
            GPUPointLight light = pointLights[lightIdx];
            
            // Calcular vector hacia la luz
            vec3 L = light.position.xyz - fs_in.WorldPos;
            float distance = length(L);
            
            // Verificar si estamos dentro del radio de la luz
            if (distance > light.position.w) {
                continue;
            }
            
            L = normalize(L);
            vec3 H = normalize(V + L);
            float NdotL = max(dot(N, L), 0.0);
            
            // Calcular atenuación
            float attenuation = 1.0 / (light.attenuation.x + 
                                      light.attenuation.y * distance + 
                                      light.attenuation.z * distance * distance);
            
            // Atenuación adicional por radio
            float radiusAttenuation = 1.0 - smoothstep(0.75 * light.position.w, light.position.w, distance);
            attenuation *= radiusAttenuation;
            
            vec3 radiance = light.color.rgb * light.color.a * attenuation;
            
            // BRDF
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
            
            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
            vec3 specular = numerator / denominator;
            
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;
            
            Lo += (kD * albedoColor.rgb / PI + specular) * radiance * NdotL;
        }
    }
    
    // Cálculo final
    vec3 ambient = vec3(0.03) * albedoColor.rgb * ao;
    vec3 color = ambient + Lo;
    
    // Tone mapping: Reinhard simple por ahora (podría ser mejorado)
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2));
    
    // Salida final
    FragColor = vec4(color, albedoColor.a);
    ObjectIdBuffer = fs_in.ObjectId;
}
