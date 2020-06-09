#version 450 core

layout(location = 0) out float frag_color;

in vec2 texcoords;

uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D noise_texture;

uniform vec3 samples[64];

int kernel_size = 64;
float radius = .5;
float bias = 0.025;

const vec2 noise_scale = vec2(1280.0/4.0, 720/4.0);

uniform mat4 projection;

void main()
{
	vec3 frag_pos_view = texture(gbuffer_position, texcoords).xyz;
	vec3 normal_view = normalize(texture(gbuffer_normal, texcoords).rgb);
	vec3 random_tangent_view = normalize(texture(noise_texture, texcoords * noise_scale).xyz);
	
	vec3 tangent_view = normalize(random_tangent_view - normal_view * dot(random_tangent_view, normal_view));
	vec3 bitangent_view = cross(normal_view, tangent_view);
	mat3 TBN_view = mat3(tangent_view, bitangent_view, normal_view);

	float occlusion = 0.0;
	for(int i=0; i < kernel_size; ++i)
	{
		vec3 sample_dir = TBN_view * samples[i];
		vec3 sampleP = frag_pos_view + sample_dir * radius;

		vec4 sample_screen = projection * vec4(sampleP, 1.f);
		sample_screen.xyz /= sample_screen.w;
		sample_screen.xyz = sample_screen.xyz * 0.5 + 0.5;

		float screen_depth_view = texture(gbuffer_position, sample_screen.xy).z;
		
		float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos_view.z - screen_depth_view));
		occlusion += (screen_depth_view >= sampleP.z + bias ? 1.0: 0.0) * range_check;
	}
	frag_color = 1 - (occlusion / 64.);
}
