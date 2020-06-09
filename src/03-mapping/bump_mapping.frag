#version 450 core

in vec2 texcoords;
in vec3 normal;
in vec3 FragPos_world;

out vec4 FragColor;

struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material{
	sampler2D color;
	sampler2D normal;
	vec3 specular; 
	float shininess;
};

uniform Material material;
uniform Light light;
uniform vec3 viewpos;

void main()
{
	vec3 color = texture(material.color, texcoords).rgb;
	vec3 bump_normal = texture(material.normal, texcoords).rgb;
	bump_normal = normalize(bump_normal * 2.0 - 1.0);

	vec3 ambient_color = color * light.ambient;
	
	vec3 light_dir = normalize(light.position - FragPos_world);
	float diff = max(dot(light_dir, bump_normal), 0.f);
	vec3 diffuse_color = diff * color;

	vec3 reflect_dir = reflect(-light_dir, bump_normal);
	vec3 view_dir = normalize(viewpos - FragPos_world);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.f), material.shininess);
	vec3 specular_color = spec * material.specular * light.specular;

	vec3 result = vec3(ambient_color + diffuse_color + specular_color);
	result = result / result + vec3(1);
	FragColor = vec4(color, 1.f);
}