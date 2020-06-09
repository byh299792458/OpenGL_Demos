#version 450 core

out vec4 FragColor;
in vec2 tex_coords;
in vec3 normal;
in vec3 frag_pos_world;
in vec4 frag_pos_light_space;

struct Material {
	sampler2D floor_texture;
	vec3 specular;
	float shininess;
};

struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform sampler2D depth_map;
uniform Light light;
uniform vec3 viewPos;

float IsInShadow(void)
{
	vec3 projection_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
	projection_coords = projection_coords.xyz * 0.5 + 0.5;
	float cur_depth = projection_coords.z;
	if(cur_depth > 1.f) 
		return 0.f;

	float shadow_percentage = 0.f;
	vec2 texel_size = 1.f / textureSize(depth_map, 0);
	for(int i=-1; i < 2; ++i){
		for(int j=-1; j < 2; ++j){
			float record_depth = texture(depth_map, projection_coords.xy + texel_size * vec2(i, j)).r;
			shadow_percentage += (cur_depth > record_depth + 0.005? 1.f: 0.f);
		}
	}
	return shadow_percentage /= 9;
}

void main()
{
	vec3 color = texture(material.floor_texture, tex_coords).rgb;
	vec3 ambient_color = light.ambient * color;

	vec3 light_dir = normalize(light.position - frag_pos_world);
	float diff = max(0.f, dot(normal, light_dir));
	vec3 diffuse_color = light.diffuse * color * diff;

	vec3 view_dir = normalize(viewPos - frag_pos_world);
	vec3 half_vec = normalize(view_dir + light_dir);
	float spec = pow(max(0.f, dot(half_vec, normal)), material.shininess);
	vec3 specular_color = light.specular * color * spec;

	float is_in_shadow = IsInShadow();
	vec3 result = ambient_color;
	result += (1.f - is_in_shadow) * diffuse_color;
	// result += (1.f - is_in_shadow) * specular_color;
	result = result / (result + vec3(1.f));
	// result = pow(result, 1.f/vec3(2.2f));
	FragColor = vec4(result, 1.f);
}