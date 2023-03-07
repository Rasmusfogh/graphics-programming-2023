#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;

uniform sampler2D GrassTexture;
uniform vec2 DirtTextureRange;
uniform sampler2D DirtTexture;
uniform vec2 RockTextureRange;
uniform sampler2D RockTexture;
uniform vec2 SnowTextureRange;
uniform sampler2D SnowTexture;

uniform vec2 ColorTextureScale;

float inverseMix(vec2 range, float value)
{
	return clamp((value - range.x) / (range.y - range.x), 0, 1);
}

void main()
{
	vec4 color0 = texture(GrassTexture, TexCoord * ColorTextureScale);
	vec4 color1 = texture(DirtTexture, TexCoord * ColorTextureScale);
	vec4 color2 = texture(RockTexture, TexCoord * ColorTextureScale);
	vec4 color3 = texture(SnowTexture, TexCoord * ColorTextureScale);

	vec4 color = color0;

	color = mix(color, color1, inverseMix(DirtTextureRange, Height));
	color = mix(color, color2, inverseMix(RockTextureRange, Height));
	color = mix(color, color3, inverseMix(SnowTextureRange, Height));

	FragColor = Color * color;
}
