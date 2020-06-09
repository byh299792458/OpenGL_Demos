#version 450 core

out vec4 frag_color;

in vec2 texcoords;

struct Light{
	vec3 position;
	vec3 color;

	float linear;
	float quadratic;
};

uniform Light light;
uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D albedo_texture;
uniform sampler2D ssao_blur_occlusion_factor_texture;

void main()
{
	vec3 frag_pos = texture(position_texture, texcoords).rgb;
	vec3 normal = texture(normal_texture, texcoords).rgb;
	vec3 diffuse = texture(albedo_texture, texcoords).rgb;
	float ssao = texture(ssao_blur_occlusion_factor_texture, texcoords).r;

	vec3 ambient_color = diffuse * 0.3 * ssao;
	vec3 view_dir = -frag_pos;
	vec3 light_dir = light.position - frag_pos;
	float distance = length(light_dir);
	vec3 halfway_dir = normalize(view_dir + light_dir);

	float diff = max(0.f, dot(light_dir, normal));
	float spec = pow(max(0.f, dot(halfway_dir, normal)), 8.0);

	vec3 diffuse_color = diffuse * diff * light.color;
	vec3 specular_color = vec3(1.f) * spec * light.color;

	float attenuation = 1.f / (1 + light.linear * distance + light.quadratic * distance * distance);
	frag_color = vec4(ambient_color + diffuse_color * attenuation + specular_color * attenuation, 1.f);
}