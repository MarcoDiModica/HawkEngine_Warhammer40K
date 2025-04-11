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

#define MAX_POINT_LIGHTS 16
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirLight dirLight;
uniform vec3 viewPos;
uniform int numPointLights;

const float PI = 3.14159265359;
uniform float tonemapStrength = 1.0;

vec3 getNormalFromMap() {
    vec3 normalSample = texture(normalMap, TexCoord).rgb;
    vec3 normalTangent = normalSample * 2.0 - 1.0;
    return normalize(TBN * normalTangent);
}

float DistributionGGX(float NdotH, float roughness) {
    float a2 = roughness * roughness;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float k = roughness * 0.5;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float roughness) {
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 ACESFilm(vec3 x) {
    return clamp((x * (2.51 * x + 0.03)) / (x * (2.43 * x + 0.59) + 0.14), 0.0, 1.0);
}

void main() {
    vec4 albedoTexture = albedoColor * vInstanceColor;
    if (u_HasAlbedoMap == 1) 
        albedoTexture *= texture(albedoMap, TexCoord);
    
    vec3 albedo = albedoTexture.rgb;
    float alpha = albedoTexture.a;
    
    float metallic = metallicFactor;
    if (u_HasMetallicMap == 1) 
        metallic = texture(metallicMap, TexCoord).r;
    
    float roughness = max(roughnessFactor, 0.08);
    if (u_HasRoughnessMap == 1) 
        roughness = max(texture(roughnessMap, TexCoord).r, 0.08);
    
    float ao = aoFactor;
    if (u_HasAoMap == 1) 
        ao = texture(aoMap, TexCoord).r;
    
    vec3 N = normalize(Normal);
    if (u_HasNormalMap == 1)
        N = getNormalFromMap();
    
    vec3 V = normalize(viewPos - FragPos);
    float NdotV = max(dot(N, V), 0.0);
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 Lo = vec3(0.0);
    vec3 kD = vec3(1.0 - metallic);
    
    vec3 dirL = normalize(-dirLight.direction);
    vec3 dirH = normalize(V + dirL);
    float dirNdotL = max(dot(N, dirL), 0.0);
    float dirNdotH = max(dot(N, dirH), 0.0);
    float dirHdotV = max(dot(dirH, V), 0.0);
    
    float dirNDF = DistributionGGX(dirNdotH, roughness);
    float dirG = GeometrySmith(NdotV, dirNdotL, roughness);
    vec3 dirF = fresnelSchlick(dirHdotV, F0);
    
    vec3 dirKS = dirF;
    vec3 dirKD = (1.0 - dirKS) * kD;
    
    vec3 dirNumerator = dirNDF * dirG * dirF;
    float dirDenominator = 4.0 * NdotV * dirNdotL + 0.001;
    vec3 dirSpecular = dirNumerator / dirDenominator;
    
    vec3 dirRadiance = dirLight.diffuse * dirLight.intensity;
    Lo += (dirKD * albedo / PI + dirSpecular) * dirRadiance * dirNdotL;
    Lo += dirLight.ambient * albedo * ao;
    
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; ++i) {
        if(pointLights[i].intensity <= 0.0) continue;
        
        vec3 L = pointLights[i].position - FragPos;
        float distance = length(L);
        
        if(distance > pointLights[i].radius) continue;
        
        L = normalize(L);
        vec3 H = normalize(V + L);
        
        float radiusFactor = 1.0 - distance/pointLights[i].radius;
        radiusFactor = radiusFactor * radiusFactor;
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + 
                            pointLights[i].quadratic * (distance * distance));
        attenuation *= radiusFactor;
        
        vec3 radiance = pointLights[i].diffuse * pointLights[i].intensity * attenuation;
        
        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);
        
        float NDF = DistributionGGX(NdotH, roughness);
        float G = GeometrySmith(NdotV, NdotL, roughness);
        vec3 F = fresnelSchlick(HdotV, F0);
        
        vec3 kS = F;
        vec3 kD = (1.0 - kS) * kD;
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * NdotV * NdotL + 0.001;
        vec3 specular = numerator / denominator;
        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        Lo += pointLights[i].ambient * albedo * ao * attenuation;
    }
    
    Lo += vec3(0.03) * albedo * ao;
    vec3 color = ACESFilm(Lo);
    
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(color, alpha);
}