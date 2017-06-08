#version 430

in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 positionIn;
out vec3 normalIn;
out vec2 uvIn;

void main(){
	//gl_Position = vec4(position, 1);
	positionIn = position;
	normalIn = normal;
	uvIn = uv;
}