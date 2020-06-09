#version 450 core

out vec4 frag_color;
in vec2 texcoords;
uniform sampler2D tex;

void main()
{
	frag_color = vec4(texture(tex, texcoords).xyz, 1.f);
}