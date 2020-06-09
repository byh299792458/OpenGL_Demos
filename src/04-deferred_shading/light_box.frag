#version 450 core

in vec2 texcoords;
uniform vec3 color;
out vec4 FragColor;

void main()
{
	FragColor = vec4(color, 1.f);	
}