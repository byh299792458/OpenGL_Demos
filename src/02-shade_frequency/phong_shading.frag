#version 450 core

in vec3 normal;
in vec3 FragPos;
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;


void main()
{
    // ambient
	vec3 ambient_color = material.ambient * light.ambient;

    // diffuse
    vec3 light_dir = light.position - FragPos;
    light_dir = normalize(light_dir);
    float diff = max(dot(normal, light_dir), 0.f);
    vec3 diffuse_color = material.diffuse * light.diffuse * diff;

    // specular
    vec3 view_dir = normalize(viewPos - FragPos);
    vec3 half_vec = normalize(view_dir + light_dir);
    float spec = pow(max(dot(half_vec, normal), 0.f), material.shininess);
    vec3 specular_color = material.specular * light.specular * spec;

    FragColor = vec4(ambient_color + diffuse_color + specular_color, 1.f);
}