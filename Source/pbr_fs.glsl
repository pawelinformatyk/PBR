#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}