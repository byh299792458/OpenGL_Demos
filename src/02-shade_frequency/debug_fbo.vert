#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_texture;

out vec2 texcoords;

void main()
{	
	texcoords = in_texture;
	gl_Position = vec4(in_pos, 1.f);
}