#version 430
 
in vec3 positionOut;
in vec3 normalOut;
in vec2 uvOut;

out vec4 fragment_color;

layout(binding = 0, rgba32f) uniform image2D positionTex;
layout(binding = 1, rgba32f) uniform image2D normalTex;


void main(){
	ivec2 size = imageSize(positionTex);
	ivec2 imageCoord = ivec2(uvOut * size);

	imageStore(positionTex, imageCoord, vec4(positionOut, 0));
	imageStore(normalTex, imageCoord, vec4(normalOut, 0));

	fragment_color = vec4(normalOut, 0);
}