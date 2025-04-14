#version 460 core

in vec4 vColor;

out vec4 FragColor;

// Uniforms
uniform vec4 debugColor = vec4(1.0, 0.0, 0.0, 1.0);
uniform bool useVertexColor = true;
uniform int debugMode = 0;

void main() {
    if (debugMode == 0) {
        FragColor = useVertexColor ? vColor : debugColor;
    }
    else if (debugMode == 1) {
        FragColor = debugColor;
    }
    else if (debugMode == 2) {
        float r = gl_FragCoord.x / 1280.0;  
        float g = gl_FragCoord.y / 720.0; 
        float b = sin(r * g * 10.0) * 0.5 + 0.5;
        
        FragColor = vec4(r, g, b, 1.0);
    }
}