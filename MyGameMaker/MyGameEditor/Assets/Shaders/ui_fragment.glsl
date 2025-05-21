#version 450 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform bool u_HasTexture;
uniform vec4 modColor;

uniform vec2 SpriteOffset;
uniform vec2 SpriteSize;
uniform vec2 SheetSize;

void main()
{
    if (u_HasTexture) {
        vec2 uv_min = SpriteOffset / SheetSize;
        vec2 uv_max = (SpriteOffset + SpriteSize) / SheetSize;
        vec2 croppedTexCoords = uv_min + TexCoord * (uv_max - uv_min);

        vec4 texColor = texture(texture1, croppedTexCoords);

        if (texColor.rgb == vec3(1.0)) {
            // Es un texto FreeType (blanco con alpha)
            FragColor = vec4(modColor.rgb, texColor.a * modColor.a);
        } else {
            // Es un sprite normal
            FragColor = texColor * modColor;
        }
    } else {
        FragColor = modColor;
    }
}