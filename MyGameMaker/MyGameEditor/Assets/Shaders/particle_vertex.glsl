#version 450 core

// Vertex attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aOffset;    // Instance position
layout(location = 3) in vec4 aColor;      // Instance color (with alpha)
layout(location = 4) in vec2 aSize;       // Instance size (x, y)
layout(location = 5) in float aRotation;  // Instance rotation in radians
layout(location = 6) in float aLifetime;  // Current lifetime fraction (0-1)

// Outputs to fragment shader
out vec2 TexCoord;
out vec4 ParticleColor;
out float Lifetime;

// Uniforms
uniform mat4 projection;
uniform mat4 view;
uniform int billboardType;  // 0: screen-aligned, 1: world-aligned, 2: axis-aligned
uniform vec3 cameraPosition;
uniform vec3 cameraUp;
uniform vec3 billboardAxis;

mat4 calculateBillboardMatrix(vec3 position) {
    // Screen-aligned billboard
    if (billboardType == 0) {
        vec3 look = normalize(cameraPosition - position);
        vec3 right = normalize(cross(cameraUp, look));
        vec3 up = cross(look, right);
        
        return mat4(
            vec4(right, 0.0),
            vec4(up, 0.0),
            vec4(look, 0.0),
            vec4(position, 1.0)
        );
    }
    // World-aligned billboard (vertical Y axis)
    else if (billboardType == 1) {
        vec3 look = normalize(cameraPosition - position);
        look.y = 0.0;
        look = normalize(look);
        
        vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), look));
        vec3 up = vec3(0.0, 1.0, 0.0);
        
        return mat4(
            vec4(right, 0.0),
            vec4(up, 0.0),
            vec4(look, 0.0),
            vec4(position, 1.0)
        );
    }
    // Axis-aligned billboard
    else {
        vec3 forward = normalize(cameraPosition - position);
        vec3 right = normalize(cross(billboardAxis, forward));
        vec3 up = cross(forward, right);
        
        return mat4(
            vec4(right, 0.0),
            vec4(up, 0.0),
            vec4(billboardAxis, 0.0),
            vec4(position, 1.0)
        );
    }
}

void main() {
    // Apply billboard orientation and position
    mat4 billboardMatrix = calculateBillboardMatrix(aOffset);
    
    // Apply rotation around the billboard's Z axis
    float c = cos(aRotation);
    float s = sin(aRotation);
    mat2 rotationMatrix = mat2(c, -s, s, c);
    
    // Apply size and rotation to the quad vertex
    vec2 rotatedPos = rotationMatrix * (aPos.xy * aSize);
    
    // Final position
    vec4 worldPos = billboardMatrix * vec4(rotatedPos.x, rotatedPos.y, 0.0, 1.0);
    gl_Position = projection * view * worldPos;
    
    // Pass values to fragment shader
    TexCoord = aTexCoord;
    ParticleColor = aColor;
    Lifetime = aLifetime;
}