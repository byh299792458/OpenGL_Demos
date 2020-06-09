#version 450 core

out vec4 frag_color;
in vec2 texcoords;

uniform sampler2D texture2d;

void main()
{
	vec4 texel = texture(texture2d, texcoords);
	frag_color = vec4(texel.r, 0, 0, 1.f);	
}