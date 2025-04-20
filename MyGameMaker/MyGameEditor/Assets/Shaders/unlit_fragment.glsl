#version 450 core

in vec2 TexCoord;    // Coordenadas de textura del fragmento
in vec3 Normal;      // Normal del fragmento

out vec4 FragColor;  // Color final del fragmento

uniform sampler2D texture1;   // Textura
uniform bool u_HasTexture;
uniform vec4 modColor;

uniform vec2 SpriteOffset;  
uniform vec2 SpriteSize; 
uniform vec2 SheetSize;

void main()
{
	if(u_HasTexture){
		//vec4 texColor = texture(texture1, TexCoord);

		vec2 uv_min = SpriteOffset / SheetSize;
		vec2 uv_max = (SpriteOffset + SpriteSize) / SheetSize;

    	// Remap texture coordinates to the sub-region
    	vec2 croppedTexCoords = uv_min + TexCoord * (uv_max - uv_min);

    	vec4 texColor = texture(texture1, croppedTexCoords);

		FragColor = texColor * modColor;
	}else{
		FragColor = modColor;
	}
};