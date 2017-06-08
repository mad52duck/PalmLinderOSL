#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 positionIn[];
in vec3 normalIn[];
in vec2 uvIn[];

out vec3 positionOut;
out vec3 normalOut;
out vec2 uvOut;

vec4 a[3] = {	vec4(-1,1,0,1),
				vec4(-1,-1,0,1),
				vec4(1,-1,0,1)
};
void main(){

	for(int i =0; i < 3; i++){
		gl_Position = a[i];
		positionOut = positionIn[i];
		normalOut = normalIn[i];
		uvOut = uvIn[i];
	    EmitVertex();
	}
	EndPrimitive();
}