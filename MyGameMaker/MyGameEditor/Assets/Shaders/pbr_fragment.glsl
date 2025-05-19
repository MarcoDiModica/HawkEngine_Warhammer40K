#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_shader_storage_buffer_object : enable

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    mat3 TBN;
    vec3 CameraPos;
} fs_in;

uniform vec4 albedoColor;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform float aoFactor;
uniform vec3 emissiveColor;
uniform float emissiveIntensity;

uniform sampler2D shadowMap;
uniform mat4 depthMVP;
uniform int useShadows;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;

uniform int u_HasAlbedoMap;
uniform int u_HasNormalMap;
uniform int u_HasMetallicMap;
uniform int u_HasRoughnessMap;
uniform int u_HasAoMap;
uniform int u_HasEmissiveMap;

uniform float tonemapStrength;

uniform int tileSize;
uniform ivec2 screenSize;
uniform int useForwardPlus;
uniform int numLights;

struct PointLight {
    vec4 position;
    vec4 color;
    vec4 attenuation;
    uint lightType;
    uint castShadow;
    uint shadowMapIndex;
    uint padding;
};

struct DirectionalLight {
    vec4 direction;
    vec4 color;
    mat4 shadowMatrix;
    uint castShadow;
    uint shadowMapIndex;
    uint useCascades;
    uint numCascades;
    float darknessfallback; 
};

layout (std430, binding = 3) readonly buffer PointLightBuffer {
    PointLight pointLights[];
};

layout (std430, binding = 4) readonly buffer DirectionalLightBuffer {
    DirectionalLight directionalLight;
};

layout (std430, binding = 5) readonly buffer LightGridBuffer {
    uvec2 lightGrid[];
};

layout (std430, binding = 6) readonly buffer LightIndicesBuffer {
    uint lightIndices[];
};

layout(location = 0) out vec4 FragColor;

const float PI = 3.14159265359;

uvec2 getLightGridInfo() {
    ivec2 tileIndex = ivec2(gl_FragCoord.xy) / tileSize;
    uint tileOffset = tileIndex.y * (screenSize.x + tileSize - 1) / tileSize + tileIndex.x;
    return lightGrid[tileOffset];
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / max(denom, 0.001); 
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / max(denom, 0.001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Calculate bias (based on depth map resolution and slope)
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    // PCF filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


void main() {
    vec4 albedo = albedoColor;
    if (u_HasAlbedoMap == 1) {
        albedo *= texture(albedoMap, fs_in.TexCoord);
    }
    
    float metallic = metallicFactor;
    if (u_HasMetallicMap == 1) {
        metallic = texture(metallicMap, fs_in.TexCoord).r;
    }
    
    float roughness = roughnessFactor;
    if (u_HasRoughnessMap == 1) {
        roughness = texture(roughnessMap, fs_in.TexCoord).r;
    }
    
    float ao = aoFactor;
    if (u_HasAoMap == 1) {
        ao = texture(aoMap, fs_in.TexCoord).r;
    }
    
    vec3 emission = emissiveColor * emissiveIntensity;
    if (u_HasEmissiveMap == 1) {
        emission = texture(emissiveMap, fs_in.TexCoord).rgb * emissiveIntensity;
    }
    
    vec3 N = normalize(fs_in.Normal);
    if (u_HasNormalMap == 5) {
        vec3 normalFromMap = texture(normalMap, fs_in.TexCoord).rgb * 2.0 - 1.0;
        N = normalize(fs_in.TBN * normalFromMap);
    }
    
    vec3 V = normalize(fs_in.CameraPos - fs_in.FragPos);
    float NdotV = max(dot(N, V), 0.0); 

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);
    
    vec3 lighting = vec3(0.0);
    vec3 color = vec3(0.0);

    
  vec3 lightDir = normalize(-directionalLight.direction.xyz);
float directionalLightIntensity = max(directionalLight.direction.w, 0.0); 
vec3 radiance = directionalLight.color.rgb * directionalLightIntensity;

vec3 H = normalize(V + lightDir);
float NdotL = max(dot(N, lightDir), 0.0);
float NDF = DistributionGGX(N, H, roughness);   
float G = GeometrySmith(N, V, lightDir, roughness);    
vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

vec3 numerator = NDF * G * F;
float denominator = 4.0 * NdotV * NdotL + 0.0001; 
vec3 specular = numerator / denominator;

vec3 kS = F;
vec3 kD = vec3(1.0) - kS;
kD *= 1.0 - metallic; 

vec3 directLighting = (kD * albedo.rgb / PI + specular) * radiance * NdotL;

float shadow = 0.0;
    if (useShadows == 1) {
        vec4 fragPosLightSpace = depthMVP * vec4(fs_in.FragPos, 1.0);
        shadow = ShadowCalculation(fragPosLightSpace, N, lightDir);
    }
    
    // Apply shadow to lighting
    directLighting *= (1.0 - shadow);

lighting += directLighting;

 float attenuation = 0;
 float constant = 0;
 uint lightCount = 0;
 float lightIntensity = 0;

if (useForwardPlus == 1) {
    uvec2 lightRange = getLightGridInfo();
    uint startIndex = lightRange.x;
    uint lightCount = lightRange.y;

    for (uint i = 0; i < numLights; i++) {
        uint lightIndex = lightIndices[0 + i];
        PointLight light = pointLights[lightIndex];

        vec3 lightPos = light.position.xyz;
        float lightRadius = light.position.w;

        vec3 L = lightPos - fs_in.FragPos;
        float distance = length(L);

        if (distance < lightRadius) 
        {
        L = normalize(L);
        H = normalize(V + L);
        NdotL = max(dot(N, L), 0.0);

        constant = light.attenuation.x;
        float linear = light.attenuation.y;
        float quadratic = light.attenuation.z;
        attenuation = 1.0 / max(constant + linear * distance + quadratic * (distance * distance), 0.001);

        float fade = clamp(1.0 - distance / lightRadius, 0.0, 1.0);
        attenuation *= fade;

        vec3 lightColor = light.color.rgb;
        lightIntensity = max(light.color.a, 0.01);
        vec3 radiance = lightColor * lightIntensity * attenuation;

        NDF = DistributionGGX(N, H, roughness);
        G = GeometrySmith(N, V, L, roughness);
        F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        numerator = NDF * G * F;
        denominator = 4.0 * NdotV * NdotL + 0.0001;
        specular = numerator / denominator;

        kS = F;
        kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        
        vec3 pointLighting = (kD * albedo.rgb / PI + specular) * radiance * NdotL;
        lighting += pointLighting;
        }
    }
}
    
lighting += emission;

vec3 ambientFallback = vec3(directionalLight.darknessfallback) * albedo.rgb;
lighting += ambientFallback;

// Ensure albedo.rgb is factored into the final color
lighting = albedo.rgb * lighting / (lighting + vec3(1.0 - tonemapStrength) + 0.001);

color += pow(lighting, vec3(1.0 / 2.2));

FragColor = vec4(color, albedo.a);
//FragColor = vec4(N * 0.5 + 0.5, 1.0);
//FragColor = vec4(fs_in.TBN[0] * 0.5 + 0.5, 1.0); 
//FragColor = vec4(fs_in.TBN[1] * 0.5 + 0.5, 1.0); 
//FragColor = vec4(fs_in.TBN[2] * 0.5 + 0.5, 1.0); 
//FragColor = vec4(N * 0.5 + 0.5, 1.0); 
}
