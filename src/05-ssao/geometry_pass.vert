#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoords;

out vec3 frag_pos_view;
out vec3 normal_view;
out vec2 texcoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;


void main()
{
	texcoords = in_texcoords;

	mat4 vm = view * model;
	mat3 vm_invT = transpose(inverse(mat3(vm)));

	vec4 view_pos = vm * vec4(in_pos, 1.f);
	frag_pos_view = view_pos.xyz;
	normal_view = vm_invT * in_normal;	

	gl_Position = projection * view_pos;
}