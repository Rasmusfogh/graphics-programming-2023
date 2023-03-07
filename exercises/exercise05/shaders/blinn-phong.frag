#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform vec3 AmbientColor;

uniform float AmbientReflection;
uniform float DiffuseReflection;
uniform float SpecularReflection;
uniform float SpecularExponent;

uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

uniform sampler2D ColorTexture0;

vec3 GetSpecularReflection(vec3 N, vec3 H) {
	return LightColor * SpecularReflection * pow(clamp(dot(N, H), 0, 1), SpecularExponent);
}

vec3 GetDiffuseReflection(vec3 L, vec3 N) {
	return LightColor * DiffuseReflection * Color.rgb * texture(ColorTexture0, TexCoord).rgb * max(dot(L, N), 0);
}

vec3 GetAmbientReflection() {
	return AmbientColor * AmbientReflection * Color.rgb * texture(ColorTexture0, TexCoord).rgb;
}

vec3 GetBlinnPhongReflection(vec3 L, vec3 N, vec3 H) {
	return GetAmbientReflection() + GetDiffuseReflection(L, N) + GetSpecularReflection(N, H);
}

void main()
{
	vec3 L = normalize(LightPosition - WorldPosition);
	vec3 V = normalize(CameraPosition - WorldPosition);
	vec3 H = normalize(L + V);
	vec3 N = normalize(WorldPosition);
	FragColor = vec4(GetBlinnPhongReflection(L, N, H).rgb, 1);
}
