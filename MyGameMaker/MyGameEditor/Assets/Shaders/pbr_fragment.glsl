#version 450 core

in vec2 TexCoord;    // Texture coordinate of the fragment
in vec3 FragPos;     // Position of the fragment in world space
in vec3 Normal;      // Normal of the fragment in world space
in mat3 TBN;         // TBN matrix for normal mapping

out vec4 FragColor;  // Final color of the fragment

// Material properties
struct Material {
    sampler2D albedoMap;
    vec4 albedoColor;
    
    bool useNormalMap;
    sampler2D normalMap;
    
    bool useRoughnessMap;
    sampler2D roughnessMap;
    float roughnessValue;
    
    bool useMetalnessMap;
    sampler2D metalnessMap;
    float metalnessValue;
    
    bool useAOMap;
    sampler2D aoMap;
    float aoValue;
    
    bool useEmissiveMap;
    sampler2D emissiveMap;
    vec3 emissiveColor;
    float emissiveIntensity;
};

// Point lights
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

// Directional light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
};

#define MAX_POINT_LIGHTS 100
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirLight dirLight;
uniform vec3 viewPos;
uniform Material material;
uniform bool u_HasTexture;
uniform sampler2D texture1;  // For compatibility with other shaders
uniform vec4 modColor;      // For compatibility with other shaders

const float PI = 3.14159265359;

// PBR functions
// Distribution function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, 0.0000001);
}

// Geometry function (Smith's method with Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, 0.0000001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Fresnel function (Schlick's approximation)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main() {
    // Get material properties
    // Albedo (base color)
    vec4 albedo;
    if (u_HasTexture) {
        albedo = texture(texture1, TexCoord) * material.albedoColor;
    } else {
        albedo = material.albedoColor;
    }
    
    // Apply modColor for consistency with other shaders
    albedo *= modColor;
    
    // Discard transparent fragments
    if (albedo.a < 0.05)
        discard;
    
    // Get normal from normal map if available
    vec3 N = normalize(Normal);
    if (material.useNormalMap) {
        vec3 normalMap = texture(material.normalMap, TexCoord).rgb;
        normalMap = normalMap * 2.0 - 1.0;   
        N = normalize(TBN * normalMap);
    }
    
    // Get roughness
    float roughness;
    if (material.useRoughnessMap)
        roughness = texture(material.roughnessMap, TexCoord).r;
    else
        roughness = material.roughnessValue;
        
    // Get metalness
    float metalness;
    if (material.useMetalnessMap)
        metalness = texture(material.metalnessMap, TexCoord).r;
    else
        metalness = material.metalnessValue;
    
    // Ambient occlusion
    float ao;
    if (material.useAOMap)
        ao = texture(material.aoMap, TexCoord).r;
    else
        ao = material.aoValue;
    
    // Get emissive value
    vec3 emission = vec3(0.0);
    if (material.useEmissiveMap)
        emission = texture(material.emissiveMap, TexCoord).rgb * material.emissiveIntensity;
    else
        emission = material.emissiveColor * material.emissiveIntensity;

    // PBR calculations
    vec3 V = normalize(viewPos - FragPos);

    // Normal incidence reflectance (F0)
    // For dielectrics (non-metals) F0 = 0.04
    // For metals, use the base color as F0
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo.rgb, metalness);
    
    // Reflectance equation
    vec3 Lo = vec3(0.0);
    
    // Directional light
    {
        vec3 L = normalize(-dirLight.direction);
        vec3 H = normalize(V + L);
        
        // BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // For energy conservation, diffuse + specular cannot exceed 1.0
        vec3 kD = vec3(1.0) - kS;
        // No diffuse for metals
        kD *= 1.0 - metalness;
        
        // Scale by incident light
        float NdotL = max(dot(N, L), 0.0);        
        
        // Dirlight using intensity weighting
        vec3 radiance = dirLight.diffuse * dirLight.intensity;
        
        // Add to outgoing radiance
        Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
    }
    
    // Point lights
    for(int i = 0; i < numPointLights; i++) {
        vec3 L = normalize(pointLights[i].position - FragPos);
        vec3 H = normalize(V + L);
        
        // Calculate distance and attenuation
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (
            pointLights[i].constant + 
            pointLights[i].linear * distance + 
            pointLights[i].quadratic * distance * distance
        );
        
        // Apply radius cutoff using smoothstep for a gradual falloff at the edges
        attenuation *= smoothstep(pointLights[i].radius, pointLights[i].radius * 0.8, distance);
        
        // Apply intensity
        attenuation *= pointLights[i].intensity;
        
        // Radiance
        vec3 radiance = pointLights[i].diffuse * attenuation;
        
        // BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // For energy conservation, diffuse + specular cannot exceed 1.0
        vec3 kD = vec3(1.0) - kS;
        // No diffuse for metals
        kD *= 1.0 - metalness;
        
        // Scale by incident light
        float NdotL = max(dot(N, L), 0.0);
        
        // Add to outgoing radiance
        Lo += (kD * albedo.rgb / PI + specular) * radiance * NdotL;
        
        // Add ambient contribution from each light
        Lo += pointLights[i].ambient * albedo.rgb * ao * attenuation;
    }
    
    // Ambient light (simple approximation)
    vec3 ambient = dirLight.ambient * albedo.rgb * ao;
    
    // Final result = ambient + emission + direct light
    vec3 color = ambient + emission + Lo;
    
    // Tone mapping (HDR to LDR)
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 
    
    // Output final color with original alpha
    FragColor = vec4(color, albedo.a);
}