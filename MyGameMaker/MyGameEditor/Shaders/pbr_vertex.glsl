#version 450 core

layout(location = 0) in vec3 aPos;        // Position attribute
layout(location = 1) in vec2 aTexCoord;    // Texture coordinate attribute
layout(location = 2) in vec3 aNormal;      // Normal attribute
layout(location = 3) in vec3 aTangent;     // Tangent attribute
layout(location = 4) in vec3 aBitangent;   // Bitangent attribute

out vec2 TexCoord;   // Texture coordinate of the fragment
out vec3 FragPos;    // Position of the fragment in world space
out vec3 Normal;     // Normal of the fragment in world space
out mat3 TBN;        // TBN matrix for normal mapping

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Pass the texture coordinates to the fragment shader
    TexCoord = aTexCoord;

    // Calculate fragment position in world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Calculate the normal matrix (transpose of inverse of upper-left 3x3 part of model matrix)
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    // Transform normal to world space
    Normal = normalize(normalMatrix * aNormal);
    
    // Calculate TBN matrix for normal mapping
    // Ensure tangent and bitangent are provided and not zero vectors
    if (length(aTangent) > 0.0 && length(aBitangent) > 0.0) {
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 B = normalize(normalMatrix * aBitangent);
        vec3 N = Normal;
        
        // Re-orthogonalize T with respect to N
        T = normalize(T - dot(T, N) * N);
        
        // Reconstruct B to ensure orthogonal basis
        B = cross(N, T);
        
        // TBN matrix for transforming from tangent space to world space
        TBN = mat3(T, B, N);
    } else {
        // Fallback if tangent/bitangent not provided
        vec3 T = vec3(1.0, 0.0, 0.0);
        vec3 B = vec3(0.0, 1.0, 0.0);
        if (abs(dot(Normal, T)) > 0.9) {
            T = vec3(0.0, 1.0, 0.0);
            B = cross(Normal, T);
            T = cross(B, Normal);
        } else {
            B = cross(Normal, T);
        }
        TBN = mat3(normalize(T), normalize(B), Normal);
    }
    
    // Transform vertex position to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}