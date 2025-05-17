#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform int billboardType;
uniform int particleType;
uniform int instanceOffset;
uniform vec3 cameraPosition;
uniform vec3 cameraUp;
uniform vec3 billboardAxis;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aOffset;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec2 aSize;
layout(location = 5) in float aRotation;
layout(location = 6) in float aLifetime;
layout(location = 7) in vec4 endColor;
layout(location = 8) in vec2 aEndSize;
layout(location = 9) in vec2 spriteOffset;
layout(location = 10) in vec2 spriteSize;
layout(location = 11) in vec2 sheetSize;

out vec2 TexCoord;
out vec4 ParticleColor;
out float Lifetime;
out vec4 EndColor;

out vec2 SpriteOffset;  
out vec2 SpriteSize; 
out vec2 SheetSize;

mat4 calculateBillboardMatrix(vec3 position) {
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
    mat4 billboardMatrix = calculateBillboardMatrix(aOffset);
    
    float c = cos(aRotation);
    float s = sin(aRotation);
    mat2 rotationMatrix = mat2(c, -s, s, c);
    
    vec2 totalSize = mix(aSize, aEndSize, aLifetime);
    vec2 rotatedPos = rotationMatrix * (aPos.xy * totalSize);
    
    vec4 worldPos = billboardMatrix * vec4(rotatedPos.x, rotatedPos.y, 0.0, 1.0);
    gl_Position = projection * view * worldPos;
    
    TexCoord = aTexCoord;
    ParticleColor = aColor;
    Lifetime = aLifetime;
    EndColor = endColor;  
    SpriteOffset = spriteOffset;
    SpriteSize = spriteSize;
    SheetSize = sheetSize;
}