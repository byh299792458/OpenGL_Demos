#version 450 core

out vec4 frag_color;
in vec3 normal;
in vec4 pos;
uniform samplerCube skybox;
uniform vec3 cam_pos_world;

void main(){
	vec3 reflect_dir = reflect(vec3(pos) - cam_pos_world, normal);
	frag_color =vec4(texture(skybox, reflect_dir).rgb, 1.f);
}