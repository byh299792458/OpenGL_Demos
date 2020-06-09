#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_texcoords;

uniform mat4 model;
uniform mat4 light_space_trans;

void main()
{
	gl_Position = light_space_trans * model * vec4(in_pos, 1.f);
}