#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexcoords;

out vec2 texcoords;

void main()
{
	texcoords = aTexcoords;
	gl_Position = vec4(aPos, 1.f);
}