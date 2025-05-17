#version 460 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    // Calculate distance from light to fragment
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // Map to [0,1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;
    
    // Write as depth
    gl_FragDepth = lightDistance;
}