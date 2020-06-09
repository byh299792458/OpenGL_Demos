#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 fColor;

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
    vec3 center = vec3(model * vec4(0.f, 0.f, 0.f, 1.f));
    vec3 pos = vec3(model * vec4(aPos, 1.f));
	gl_Position = projection * view * vec4(pos, 1.f);

    // normal
    vec3 normal = normalize(aPos);

    // ambient
	vec3 ambient_color = material.ambient * light.ambient;

    // diffuse
    vec3 light_dir = light.position - pos;
    light_dir = normalize(light_dir);
    float diff = max(dot(normal, light_dir), 0.f);
    vec3 diffuse_color = material.diffuse * light.diffuse * diff;

    // specular
    vec3 view_dir = normalize(viewPos - pos);
    vec3 half_vec = normalize(view_dir + light_dir);
    float spec = pow(max(dot(half_vec, normal), 0.f), material.shininess);
    vec3 specular_color = material.specular * light.specular * spec;

    fColor = ambient_color + diffuse_color + specular_color;
}








