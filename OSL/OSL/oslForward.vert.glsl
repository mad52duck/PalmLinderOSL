#version 430

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec2 uvOut;

layout(location = 1) uniform mat4 viewProjection;
layout(location = 2) uniform mat4 world;

layout(location = 3) uniform int megaSphereTexIndex;

layout(location = 4) uniform int divider[2];

void main(){
	gl_Position = viewProjection * world * vec4(position, 1);
	uvOut = vec2(uv.x/divider[0], uv.y/divider[1]) + normal.xy - normal.xy;
	uvOut += vec2(
				(1.0/divider[0]) * (megaSphereTexIndex % divider[0]),		//x
				(1.0/divider[1]) * (megaSphereTexIndex / divider[0])		//y
	);
}