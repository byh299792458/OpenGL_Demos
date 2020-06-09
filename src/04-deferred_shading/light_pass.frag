#version 450 core

out vec4 FragColor;

in vec2 texcoords;
 
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light{
    vec3 position;
    vec3 color;

    float linear;
    float quadratic;
};

const int num_lights = 32;
uniform Light lights[num_lights];
uniform vec3 view_pos;

void main()
{
    vec3 FragPos = texture(gPosition, texcoords).rgb;
    vec3 normal = texture(gNormal, texcoords).rgb;
    vec3 diffuse = texture(gAlbedoSpec, texcoords).rgb;
    float specular = texture(gAlbedoSpec, texcoords).a;

    vec3 ambient_color = diffuse * 0.1;
    vec3 lighting = ambient_color;
    vec3 view_dir = normalize(view_pos - FragPos);
    for(int i=0; i<num_lights; ++i)
    {
        vec3 light_dir = normalize(lights[i].position - FragPos);
        float diff = max(dot(normal, light_dir), 0.f);
        vec3 diffuse_color = diff * diffuse * lights[i].color;

        vec3 halfway_dir = normalize(view_dir + light_dir);
        float spec = pow(max(dot(normal, halfway_dir), 0.f), 16.0);
        vec3 specular_color = spec * specular * lights[i].color;

        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.f / (1.f + 
                lights[i].linear * distance + 
                lights[i].quadratic * distance * distance);
        diffuse_color *= attenuation;
        specular_color *= attenuation;
        lighting += diffuse_color + specular_color;
    }
    FragColor = vec4(lighting, 1.f);
}