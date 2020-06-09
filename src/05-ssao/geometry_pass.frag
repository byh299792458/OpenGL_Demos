#version 450 core

layout(location = 0) out vec3 gbuffer_position;
layout(location = 1) out vec3 gbuffer_normal;
layout(location = 2) out vec3 gbuffer_albedo;

in vec3 frag_pos_view;
in vec3 normal_view;
in vec2 texcoords;

uniform sampler2D diffuse_texture1;

void main()
{
	gbuffer_position = frag_pos_view;
	gbuffer_normal = normalize(normal_view);
	gbuffer_albedo = vec3(0.95);
}