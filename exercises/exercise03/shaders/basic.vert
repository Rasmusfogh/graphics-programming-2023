#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// (todo) 03.X: Add the out variables here
out vec4 Normal;


// (todo) 03.X: Add the uniforms here
uniform mat4 WorldMatrix = mat4(1.0f);
uniform mat4 RotationMatrix;
uniform mat4 ViewProjMatrix;

void main()
{
	vec4 newPosition = ViewProjMatrix * WorldMatrix *  RotationMatrix * vec4(VertexPosition.xyz, 1.0);
	gl_Position = vec4(newPosition.xyz, 1.0);

	Normal = WorldMatrix * vec4(VertexNormal.xyz, 0.0f);
}
