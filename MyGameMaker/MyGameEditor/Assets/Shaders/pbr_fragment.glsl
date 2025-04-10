#version 450 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
in vec4 vInstanceColor;

out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform int u_HasAlbedoMap;
uniform int u_HasNormalMap;
uniform int u_HasMetallicMap;
uniform int u_HasRoughnessMap;
uniform int u_HasAoMap;

uniform vec4 albedoColor;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform float aoFactor;

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

const float PI = 3.14159265359;
uniform float tonemapStrength = 1.0;

vec3 getNormalFromMap()
{
    vec3 normalSample = texture(normalMap, TexCoord).rgb;
    vec3 normalTangent = normalSample * 2.0 - 1.0;
    
    if(length(normalTangent) < 0.1) 
        return normalize(Normal);
    
    return normalize(TBN * normalTangent);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom = NdotV;
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
    vec4 texColor = (u_HasAlbedoMap == 1) ? texture(albedoMap, TexCoord) : vec4(1.0);
    vec4 albedoTexture = texColor * albedoColor * vInstanceColor;
    vec3 albedo = albedoTexture.rgb;
    float alpha = albedoTexture.a;
    
    float metallic = (u_HasMetallicMap == 1) ? texture(metallicMap, TexCoord).r : metallicFactor;
    float roughness = (u_HasRoughnessMap == 1) ? texture(roughnessMap, TexCoord).r : roughnessFactor;
    float ao = (u_HasAoMap == 1) ? texture(aoMap, TexCoord).r : aoFactor;
    
    roughness = max(roughness, 0.08);
    
    vec3 N = (u_HasNormalMap == 1) ? getNormalFromMap() : normalize(Normal);
    if(length(N) < 0.5) N = normalize(Normal);
    
    vec3 V = normalize(viewPos - FragPos);
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    vec3 Lo = vec3(0.0);
    
    // Directional light calculation
    {
        vec3 L = normalize(-dirLight.direction);
        vec3 H = normalize(V + L);
        
        vec3 radiance = dirLight.diffuse * dirLight.intensity;
        
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        
        vec3 ambient = dirLight.ambient * albedo * ao;
        Lo += ambient;
    }
    
    // Point lights calculation
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; ++i) 
    {
        if(pointLights[i].intensity <= 0.0) continue;
        
        vec3 L = normalize(pointLights[i].position - FragPos);
        vec3 H = normalize(V + L);
        
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                            pointLights[i].quadratic * (distance * distance));
        attenuation *= smoothstep(pointLights[i].radius, 0.0, distance);
        
        vec3 radiance = pointLights[i].diffuse * pointLights[i].intensity * attenuation;
        
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        
        vec3 ambient = pointLights[i].ambient * albedo * ao * attenuation;
        Lo += ambient;
    }
    
    vec3 fallbackAmbient = vec3(0.03) * albedo * ao;
    Lo += fallbackAmbient;
    
    vec3 color = ACESFilm(Lo);
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 
    
    FragColor = vec4(color, alpha);
}