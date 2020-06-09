#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec4 pos;

void main()
{
	normal = in_normal;
	pos = model * vec4(in_pos, 1.f);
	gl_Position = projection * view * pos;
}