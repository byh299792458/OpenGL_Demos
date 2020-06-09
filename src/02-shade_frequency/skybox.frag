#version 450 core

out vec4 FragColor;

in vec3 tex_coords;
uniform samplerCube skybox_sampler;

void main()
{
	FragColor = texture(skybox_sampler, tex_coords);
}