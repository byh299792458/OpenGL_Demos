#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec3 FragWorldPos;
out vec3 normal;
out vec2 texcoords;

void main()
{
	texcoords = aTexcoords;	
	FragWorldPos = vec3(model * vec4(aPos, 1.f));
	mat3 model_invT = transpose(inverse(mat3(model)));
	normal = model_invT * aNormal;
	gl_Position = projection * view * model * vec4(aPos, 1.f);
}