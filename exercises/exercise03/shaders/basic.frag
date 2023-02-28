#version 330 core

// (todo) 03.X: Add the in variables here
in vec4 Normal;

out vec4 FragColor;

uniform vec3 Color = vec3(1);

void main()
{
	vec4 newColor = vec4(Color.rgb, 1) * dot(normalize(Normal.xyz),vec3(1));
	FragColor = newColor;
}
