#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 normal;

void main()
{
	normal = normalize(aPos);
	gl_Position = projection * view * model * vec4(aPos, 1.f);
	FragPos = vec3(model * vec4(aPos, 1.f));
}
