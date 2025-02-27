#version 450 core

in vec2 TexCoord;    // Coordenadas de textura del fragmento
in vec3 Normal;      // Normal del fragmento

out vec4 FragColor;  // Color final del fragmento

uniform sampler2D texture1;   // Textura del agua
uniform bool u_HasTexture;
uniform vec4 modColor;

void main()
{
	if(u_HasTexture){
		vec4 texColor = texture(texture1, TexCoord);
		FragColor = texColor;
		//color = vec4(Normal, 1.0) * modColor; //debug normals
	}else{
		FragColor = modColor;
	}
};