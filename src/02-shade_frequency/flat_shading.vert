#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 posw;
} vs_out;

void main()
{
	vs_out.posw = vec3(model * vec4(aPos, 1.f));
	gl_Position = projection * view * model * vec4(aPos, 1.f);
}