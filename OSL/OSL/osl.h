#pragma once
#include <GL\glew.h>
#include <GL\GL.h>
#include <glm\common.hpp>
#include "Lights.h"
#include <string>
#include "Hitbox.h"

class osl {
	struct oslObject{
		GLuint positionTex, normalTex, diffuseTex;
		GLuint va;
		GLuint megaTex;
		int size;
	};
	struct oslInstance {
		GLuint tex;
		Hitbox hb;
		int32_t lights[10];
		int lightsAffecting;
		bool fixed;
		int interval;
		int counter;
	};

	
	public:			//"private" variables
		int textureRes;
		int shaderResInput[2];
		static const int maxObjects = 500;
		GLuint oslprog, textureGenProg, oslForward;
		oslObject sphere, cube;
		int nrOfSpheres;
		int nrOfCubes;
		glm::ivec2 sphereTexSize;
		oslInstance sphereInstances[maxObjects];
		oslInstance cubeInstances[maxObjects];
		glm::mat4* cubes;
		glm::mat4* spheres;
		Lights lights;
		const int nrOfLights = 10;
		GLuint indexBuffer;

		GLuint megaSphereTex;
		GLuint megaCubeTex;

		GLuint lockTex;
		bool dynamic;
		int asyncer;
	public:
		osl();
		~osl();
		void init(bool mode, bool resolution, int asyncer);
		void render( glm::mat4 vp, glm::vec3 camPos, float dt);
		void renderInstances(glm::mat4 vp);
		void updateShading(oslObject &object, oslInstance &instance, glm::mat4 &world, int index);
		void generateTextures(GLuint sphereVa, int sphereSize, oslObject object);
		void createSphereTextures(int number);
		void createCubeTextures(int number);
		void createTextures(oslObject* obj, std::string path);
		void updateLights(oslInstance* instance, int number);
		void setupShading(glm::vec3 &camPos, oslObject* obj);
		void initDesync(glm::vec3 camPos);
};