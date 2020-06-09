#version 450 core

layout(location = 0) out float frag_color;
uniform sampler2D occlusion_factor;
in vec2 texcoords;

void main()
{
	vec2 texel_size = 1.f / textureSize(occlusion_factor, 0);
	float result = 0.f;
	for(int i=-2; i<2; ++i)
	{
		for(int j=-2; j<2; ++j)
		{
			vec2 offset = vec2(i, j) * texel_size;
			result += texture(occlusion_factor, texcoords + offset).r;
		}	
	}
	frag_color = result / 16.f;
}