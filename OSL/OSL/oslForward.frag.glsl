#version 430
 
in vec2 uvOut;

out vec4 fragment_color;

layout(location = 0) uniform sampler2D diffTex;



void main(){
	vec2 uv = uvOut.xy;
	fragment_color = vec4(texture(diffTex, uv).xyz, 1);
}