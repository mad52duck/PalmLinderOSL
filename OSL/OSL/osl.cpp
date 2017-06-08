#include "osl.h"
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "src\SOIL.h"
#include <iostream>


void printshaderError(GLuint shader) {
	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(shader);
		std::printf("%s\n", &(errorLog[0]));
	}
}
void printprogramError(GLuint program) {
	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
		std::printf("%s\n", &(errorLog[0]));
	}
}
void osl::init( bool mode, bool resolution, int asyncer ) {
	this->asyncer = asyncer;
	if (resolution) {
		textureRes = 256;
		shaderResInput[0] = 16;
		shaderResInput[1] = 16;
	}
	else {
		textureRes = 128;
		shaderResInput[0] = 32;
		shaderResInput[1] = 16;
	}
	const char* textc;
	//compute shader
	GLuint oslshader = glCreateShader(GL_COMPUTE_SHADER);
	std::ifstream a("oslShader.comp.glsl");
	std::string text = std::string(std::istreambuf_iterator<char>(a), std::istreambuf_iterator<char>());
	textc = text.c_str();
	glShaderSource(oslshader, 1, &textc, NULL);	
	glCompileShader(oslshader);
	printshaderError(oslshader);

	oslprog = glCreateProgram();
	glAttachShader(oslprog, oslshader);
	glLinkProgram(oslprog);
	printprogramError(oslprog);
	a.close();

	//texure gen shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	a.open("textureGen.vert.glsl");
	text = std::string(std::istreambuf_iterator<char>(a), std::istreambuf_iterator<char>());
	textc = text.c_str();
	glShaderSource(vertexShader, 1, &textc, NULL);
	glCompileShader(vertexShader);
	printshaderError(vertexShader);
	a.close();

	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	a.open("textureGen.geom.glsl");
	text = std::string(std::istreambuf_iterator<char>(a), std::istreambuf_iterator<char>());
	textc = text.c_str();
	glShaderSource(geometryShader, 1, &textc, NULL);
	glCompileShader(geometryShader);
	printshaderError(geometryShader);
	a.close();

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	a.open("textureGen.frag.glsl");
	text = std::string(std::istreambuf_iterator<char>(a), std::istreambuf_iterator<char>());
	textc = text.c_str();
	glShaderSource(fragmentShader, 1, &textc, NULL);
	glCompileShader(fragmentShader);
	printshaderError(fragmentShader);
	a.close();

	textureGenProg = glCreateProgram();
	glAttachShader(textureGenProg, vertexShader);
	glAttachShader(textureGenProg, geometryShader);
	glAttachShader(textureGenProg, fragmentShader);
	glLinkProgram(textureGenProg);

	//osl forawrd shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	a.open("oslForward.vert.glsl");
	text = std::string(std::istreambuf_iterator<char>(a), std::istreambuf_iterator<char>());
	textc = text.c_str();
	glShaderSource(vertexShader, 1, &textc, NULL);
	glCompileShader(vertexShader);
	printshaderError(vertexShader);
	a.close();

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	a.open("oslForward.frag.glsl");
	text = std::string(std::istreambuf_iterator<char>(a), std::istreambuf_iterator<char>());
	textc = text.c_str();
	glShaderSource(fragmentShader, 1, &textc, NULL);
	glCompileShader(fragmentShader);
	printshaderError(fragmentShader);
	a.close();

	oslForward = glCreateProgram();
	glAttachShader(oslForward, vertexShader);
	glAttachShader(oslForward, fragmentShader);
	glLinkProgram(oslForward);

	//textures
	if (resolution) {
		createTextures(&cube, "textures/rubik256.png");
		createTextures(&sphere, "textures/daSphere256.png");
	}
	else {
		createTextures(&cube, "textures/rubik128.png");
		createTextures(&sphere, "textures/daSphere128.png");
	}

	lights.init(oslprog, nrOfLights);
	updateLights(sphereInstances, nrOfSpheres);
	updateLights(cubeInstances, nrOfCubes);
	this->dynamic = mode;
}
void osl::createTextures(oslObject* obj, std::string path) {

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &obj->positionTex);
	glBindTexture(GL_TEXTURE_2D, obj->positionTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, textureRes, textureRes);// , 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &obj->normalTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj->normalTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, textureRes, textureRes);// , 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	int width, height, channels;
	unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &channels, 4);

	glGenTextures(1, &obj->diffuseTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obj->diffuseTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);// , 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete data;
}

void osl::updateLights(oslInstance* instance, int number)
{
	for (int i = 0; i < number; i++) {
		instance[i].lightsAffecting = 0;
		for (int j = 0; j < nrOfLights; j++) {
			if (instance[i].hb.intersect(&lights.lightboxes[j])) {
				int index = instance[i].lightsAffecting++;
				instance[i].lights[index] = j;
			}
		}
	}
}

void osl::createSphereTextures(int number) {
	glGenTextures(1, &sphere.megaTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sphere.megaTex);
	int height = 4096;
	if (textureRes == 128) {
		height = 2048;
	}
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 4096, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	nrOfSpheres = number;
}

void osl::createCubeTextures(int number) {
	glGenTextures(1, &cube.megaTex);
		glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cube.megaTex);
	int height = 4096;
	if (textureRes == 128) {
		height = 2048;
	}
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 4096, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
	nrOfCubes = number;
}

void osl::render( glm::mat4 vp, glm::vec3 camPos, float dt )
{
	if (dynamic) 
	{
		glUseProgram(oslprog);
		updateLights(sphereInstances, nrOfSpheres);
		updateLights(cubeInstances, nrOfCubes);
		lights.update(oslprog, dt);

		setupShading(camPos, &sphere);
		for (int i = 0; i < nrOfSpheres; i++) {
			if (--sphereInstances[i].counter <= 0) {
				sphereInstances[i].counter = sphereInstances[i].interval;
				if (sphereInstances[i].lightsAffecting > 0) {
					updateShading(sphere, sphereInstances[i], spheres[i], i);
					if (dynamic) {
						sphereInstances[i].fixed = false;
					}
					else {
						sphereInstances[i].lightsAffecting = 0;
						sphereInstances[i].fixed = true;
					}
				}
				else if (!sphereInstances[i].fixed) {
					updateShading(sphere, sphereInstances[i], spheres[i], i);
					sphereInstances[i].fixed = true;
				}
			}
		}
		setupShading(camPos, &cube);
		for (int i = 0; i < nrOfCubes; i++) {
			if (--cubeInstances[i].counter <= 0) {
				cubeInstances[i].counter = cubeInstances[i].interval;
				if (cubeInstances[i].lightsAffecting > 0) {
					updateShading(cube, cubeInstances[i], cubes[i], i);
					if (dynamic) {
						cubeInstances[i].fixed = false;
					}
					else {
						cubeInstances[i].lightsAffecting = 0;
						cubeInstances[i].fixed = true;
					}
				}
				else if (!cubeInstances[i].fixed) {
					updateShading(cube, cubeInstances[i], cubes[i], i);
					cubeInstances[i].fixed = true;
				}
			}
		}
		glUseProgram(0);
	}
	renderInstances(vp);
}
void osl::renderInstances(glm::mat4 vp) {
	glUseProgram(oslForward);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(sphere.va);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glUniformMatrix4fv(1, 1, GL_FALSE, &vp[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(0, 0);
	glUniform1iv(4, 2, shaderResInput);
	glBindTexture(GL_TEXTURE_2D, sphere.megaTex);
	for (int i = 0; i < nrOfSpheres; i++) {
		glUniform1i(3, i);
		glUniformMatrix4fv(2, 1, GL_FALSE, &spheres[i][0][0]);
		glDrawArrays(GL_TRIANGLES, 0, sphere.size);
	}

	glBindVertexArray(cube.va);

	glBindTexture(GL_TEXTURE_2D, cube.megaTex);
	for (int i = 0; i < nrOfCubes; i++) {
		glUniform1i(3, i);
		glUniformMatrix4fv(2, 1, GL_FALSE, &cubes[i][0][0]);
		glDrawArrays(GL_TRIANGLES, 0, cube.size);
	}

}

void osl::setupShading(glm::vec3 &camPos, oslObject* obj) {

	glBindImageTexture(0, obj->positionTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, obj->normalTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(2, obj->diffuseTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	glBindImageTexture(3, obj->megaTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glUniform1i(4, shaderResInput[0]);
	glUniform3fv(1, 1, &camPos[0]);
}
void osl::initDesync(glm::vec3 camPos)
{
	for (int i = 0; i < nrOfSpheres; i++) {
		sphereInstances[i].interval = 1 +(glm::length(camPos - sphereInstances[i].hb.position) / asyncer);
		sphereInstances[i].counter = 1;
	}
	for (int i = 0; i < nrOfCubes; i++) {
		cubeInstances[i].interval = 1 +(glm::length(camPos - cubeInstances[i].hb.position) / asyncer);
		cubeInstances[i].counter = 1;
	}
}
void osl::updateShading( oslObject &object, oslInstance &instance, glm::mat4 &world, int index) {
	glUniformMatrix4fv(2, 1, GL_FALSE, &world[0][0]);
	glUniform1iv(7, 10, instance.lights);
	glUniform1i(6, instance.lightsAffecting);
	glUniform1i(5, index);
	glDispatchCompute(textureRes / 16, textureRes / 16, 1);
}
void osl::generateTextures(GLuint va, int size, oslObject object)
{
	glUseProgram(textureGenProg);
	glBindVertexArray(va);
	glEnable(GL_TEXTURE_2D);
	
	glBindImageTexture(0, object.positionTex,	0,	GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(1, object.normalTex,		0,	GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawArrays(GL_TRIANGLES, 0, size);
	glUseProgram(0);
}
osl::osl() {
}

osl::~osl()
{
}
