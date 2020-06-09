#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

out vec2 texcoords;
out vec3 normal;
out vec3 FragPos_world;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	normal = aNormal;
	texcoords = aTexCoords;
	FragPos_world = aPos;
	gl_Position = projection * view * vec4(aPos, 1.f);
}