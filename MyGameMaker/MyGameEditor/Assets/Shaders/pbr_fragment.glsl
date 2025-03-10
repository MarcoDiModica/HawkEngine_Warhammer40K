#version 450 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

out vec4 FragColor;

// Base textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// Flags to control which maps are active
uniform int u_HasAlbedoMap;
uniform int u_HasNormalMap;
uniform int u_HasMetallicMap;
uniform int u_HasRoughnessMap;
uniform int u_HasAoMap;

// PBR material properties (used when maps aren't available)
uniform vec4 albedoColor; // Base color
uniform float metallicFactor;
uniform float roughnessFactor;
uniform float aoFactor;

// Light structures
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float radius;
    float intensity;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

#define MAX_POINT_LIGHTS 100
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirLight dirLight;
uniform vec3 viewPos;
uniform int numPointLights;

// Constants for PBR calculations
const float PI = 3.14159265359;

// Tonemapping control (added)
uniform float tonemapStrength = 1.0;

// PBR functions
vec3 getNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

// Added ACES tonemapping function
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main()
{
    // Sample textures or use uniform values
    vec4 texColor = (u_HasAlbedoMap == 1) ? texture(albedoMap, TexCoord) : vec4(1.0);
    
    // CAMBIO IMPORTANTE: Siempre aplicar el albedoColor, como en el shader unlit
    vec4 albedoTexture = texColor * albedoColor;
    vec3 albedo = albedoTexture.rgb;
    float alpha = albedoTexture.a;
    
    float metallic = (u_HasMetallicMap == 1) ? texture(metallicMap, TexCoord).r : metallicFactor;
    float roughness = (u_HasRoughnessMap == 1) ? texture(roughnessMap, TexCoord).r : roughnessFactor;
    float ao = (u_HasAoMap == 1) ? texture(aoMap, TexCoord).r : aoFactor;
    
    // Make sure roughness is never 0 to avoid division by zero
    roughness = max(roughness, 0.04);
    
    // Use normal map if available, otherwise use interpolated normal
    vec3 N = (u_HasNormalMap == 1) ? getNormalFromMap() : normalize(Normal);
    
    // In case normal computation fails, fall back to vertex normal
    if(length(N) < 0.5) N = normalize(Normal);
    
    vec3 V = normalize(viewPos - FragPos);
    
    // Calculate reflectance at normal incidence (F0)
    // Use 0.04 for non-metals (dialectrics)
    // For metals, use the albedo color (metalness workflow)
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    // Calculate per-light radiance and sum up contributions
    vec3 Lo = vec3(0.0);
    
    // Directional light contribution
    {
        vec3 L = normalize(-dirLight.direction);
        vec3 H = normalize(V + L);
        
        // Calculate light attenuation and radiance
        vec3 radiance = dirLight.diffuse * dirLight.intensity;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        // Add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        
        // Add ambient light from directional light
        vec3 ambient = dirLight.ambient * albedo * ao;
        Lo += ambient;
    }
    
    // Point light contributions
    for (int i = 0; i < numPointLights; ++i) 
    {
        // Skip invalid lights
        if(pointLights[i].intensity <= 0.0) continue;
        
        vec3 L = normalize(pointLights[i].position - FragPos);
        vec3 H = normalize(V + L);
        
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                            pointLights[i].quadratic * (distance * distance));
        attenuation *= smoothstep(pointLights[i].radius, 0.0, distance);
        
        vec3 radiance = pointLights[i].diffuse * pointLights[i].intensity * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        // Add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        
        // Add ambient contribution from point light
        vec3 ambient = pointLights[i].ambient * albedo * ao * attenuation;
        Lo += ambient;
    }
    
    // Add small ambient term to avoid completely black objects when no lights are present
    vec3 fallbackAmbient = vec3(0.03) * albedo * ao;
    Lo += fallbackAmbient;
    
    // Debug mode - uncomment to see just the albedo texture
    //FragColor = vec4(albedo, alpha);
    //return;
    
    // CAMBIO IMPORTANTE: Mejor tonemapping usando ACES
    vec3 color;
    
    // Mezclar entre el resultado crudo (sin tonemapping) y el resultado con tonemapping
    // Para debugging, puedes ajustar tonemapStrength a 0 para ver colores sin procesar
    if (tonemapStrength > 0.0) {
        // Usar ACES tonemapping
        color = mix(Lo, ACESFilm(Lo), tonemapStrength);
    } else {
        // Usar el antiguo Reinhard tonemapping
        color = Lo / (Lo + vec3(1.0));
    }
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 
    
    // Use the alpha from the texture or the albedoColor
    FragColor = vec4(color, alpha);
}

vec3 getNormalFromMap()
{
    // Sample the normal map and transform from [0,1] to [-1,1] range
    vec3 normalSample = texture(normalMap, TexCoord).rgb;
    vec3 normalTangent = normalSample * 2.0 - 1.0;
    
    // Ensure the normal has some length to avoid issues
    if(length(normalTangent) < 0.1) 
        return normalize(Normal);
    
    // Transform normal from tangent space to world space using TBN matrix
    return normalize(TBN * normalTangent);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / max(denom, 0.0001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}