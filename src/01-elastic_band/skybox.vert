#version 450 core

layout(location = 0) in vec3 in_pos;

out vec3 texcoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	texcoords = in_pos;
	vec4 pos = projection * view * vec4(in_pos, 1.f);
	gl_Position = pos.xyww;
}