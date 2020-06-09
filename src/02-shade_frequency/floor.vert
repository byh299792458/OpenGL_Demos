#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 light_space_trans;

out vec2 tex_coords;
out vec3 normal;
out vec3 frag_pos_world;
out vec4 frag_pos_light_space;

void main()
{
	normal = aNormal;
	tex_coords = aTexCoords;
	frag_pos_world = aPos;
	frag_pos_light_space = light_space_trans * model * vec4(aPos, 1.f);
	gl_Position = projection * view * model * vec4(aPos, 1.f);	
}