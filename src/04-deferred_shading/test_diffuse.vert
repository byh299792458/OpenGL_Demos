#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texcoords;

void main()
{
	texcoords = aTexcoords;	
	gl_Position = projection * view * model * vec4(aPos, 1.f);
}