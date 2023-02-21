#version 330 core

out vec4 FragColor;
in vec4 Color;
// (todo) 02.5: Add Color input variable here


void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord

	float alpha = 1 - length(gl_PointCoord * 2 - 1);
	FragColor = vec4(Color.rgb, alpha * Color.a);
}