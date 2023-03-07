#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture0;

void main()
{
	vec4 color = texture(ColorTexture0, TexCoord);
	FragColor = Color * color;
}
