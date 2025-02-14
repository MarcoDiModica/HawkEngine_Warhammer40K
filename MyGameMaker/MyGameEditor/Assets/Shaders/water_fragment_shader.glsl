#version 450 core

in vec2 TexCoord;    // Coordenadas de textura del fragmento
in vec3 FragPos;     // Posición del fragmento en el espacio mundial
in vec3 Normal;      // Normal del fragmento
in float offset;     // Diferencia entre la posición original y la desplazada

out vec4 FragColor;  // Color final del fragmento

uniform vec3 u_ColorLow;      // Color para las alturas bajas
uniform vec3 u_ColorHigh;     // Color para las alturas altas
uniform sampler2D texture1;   // Textura del agua
uniform float u_Amplitude;    // Amplitud de las olas
uniform float u_Factor;       // Factor para ajustar la presencia de colores bajos

void main() {
    float minHeight = -u_Amplitude;
    float maxHeight = u_Amplitude;

    // Normaliza el valor de offset entre [0, 1]
    float height = clamp((offset - minHeight) / (maxHeight - minHeight), 0.0, 1.0);

    // Ajusta el height para sesgarlo hacia las partes más bajas
    height = pow(height, u_Factor); // Usa una potencia para cambiar la distribución

    // Interpola entre los colores bajos y altos
    vec3 color = mix(u_ColorHigh, u_ColorLow, height);

    // Asigna el color final
    FragColor = vec4(color, 1.0);
}