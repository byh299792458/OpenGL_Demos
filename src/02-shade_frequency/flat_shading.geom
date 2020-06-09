#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 posw;
} gs_in[];

out vec3 normal;
out vec3 FragPosw;

vec3 getNormal()
{
	vec3 p1 = gs_in[1].posw-gs_in[0].posw;
	vec3 p2 = gs_in[2].posw-gs_in[0].posw;
	return normalize(cross(p1, p2));
}

void main()
{
	normal = getNormal();

	FragPosw = gs_in[0].posw;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	FragPosw = gs_in[1].posw;
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	FragPosw = gs_in[2].posw;
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}