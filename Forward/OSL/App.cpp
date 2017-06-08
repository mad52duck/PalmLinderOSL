#include "App.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <set>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

void GLAPIENTRY gl_callback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam);


bool wDown, sDown, aDown, dDown, eDown, qDown;
bool running;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_W)
			wDown = true;
		if (key == GLFW_KEY_S)
			sDown = true;
		if (key == GLFW_KEY_A)
			aDown = true;
		if (key == GLFW_KEY_D)
			dDown = true;
		if (key == GLFW_KEY_Q)
			qDown = true;
		if (key == GLFW_KEY_E)
			eDown = true;
		if (key == GLFW_KEY_ESCAPE)
			running = false;
	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W)
			wDown = false;
		if (key == GLFW_KEY_S)
			sDown = false;
		if (key == GLFW_KEY_A)
			aDown = false;
		if (key == GLFW_KEY_D)
			dDown = false;
		if (key == GLFW_KEY_Q)
			qDown = false;
		if (key == GLFW_KEY_E)
			eDown = false;
	}
}
void App::updateInputs() {
	movement = {  aDown + -1 *dDown, eDown + -1 * qDown, wDown + -1 * sDown };
}
App::App() {
}

App::App(bool mode, int runTime, int shotRate, int instanceNumber, std::string path) {
	this->runTime = runTime;
	this->everyXFrame = shotRate;
	this->resultPath = path;
	this->instanceID = instanceNumber;
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	w = glfwCreateWindow(Camera::SCREEN_WIDTH, Camera::SCREEN_HEIGHT, "OSL", NULL, NULL);
	glfwMakeContextCurrent(w);
	int error = glewInit();
	running = true;
	if (error)
		std::cout << ((char*)glewGetErrorString(error));
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE,	GL_DONT_CARE, GL_DONT_CARE,	0, nullptr, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glfwSetKeyCallback(w, key_callback);
	glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	forwardProgram.init(mode);

	sphereSize = 0;
	forwardProgram.sphereVao = createSphereVBO(20, 20);
	createSpheres();
	forwardProgram.spheres = sphereMatrices;

	forwardProgram.cubeVao = createCubeVBO();
	createCubes();
	forwardProgram.cubes = cubeMatrices;
	
	forwardProgram.sphereSize = sphereSize;
	createFrameBuffer();
}
App::~App(){

}

GLuint App::createSphereVBO(int xRes, int yRes) {
	float pi = glm::pi<float>();
	float xIncrement = (3.1415 * 2) / xRes;
	float yIncrement = (3.1415) / (yRes - 1);

	std::vector<vtxData> data;
	data.resize(xRes * (yRes + 1));
	//vtxData* data = new vtxData[];
	float x, y, z;
	for (int i = 0; i < yRes + 1; i++)
	{
		for (int j = 0; j < xRes; j++) {
			x = sinf(j*xIncrement)*sinf(i*yIncrement);
			y = cosf(i*yIncrement);
			z = cosf(j*xIncrement)*sinf(i*yIncrement);
			data[i*xRes + j] = { x / 2, //positions
				y / 2,
				z / 2,
				x,	//normals
				y,
				z,
				(float)j / (xRes), //uvs
				(float)i / (yRes)
			};
		}
	}

	std::vector<face> faceData;
	vtxData t = { 1,1,1,1,1,1,1,1 };
	faceData.resize(2 * xRes*yRes, { t,t,t });
	//face* faceData = new face[2*newRes*(newRes/2+1)];
	for (int i = 0; i < yRes - 1; i++) {
		for (int j = 0; j < xRes; j++) {
			int a = i*xRes + j;
			int b = i*xRes + (j + 1) % xRes;
			int c = (i + 1)*xRes + j;
			int d = (i + 1)*xRes + (j + 1) % xRes;
			int index = 2 * (i*(xRes)+j);
			vtxData va = data[a];
			vtxData vb = data[b];
			vtxData vc = data[c];
			vtxData vd = data[d];
			if (j == xRes - 1) {
				vb.u = 1.0f;
				vd.u = 1.0f;
			}
			faceData[index] = { va, vc, vb }; //winding order(?)
			faceData[index + 1] = { vb, vc, vd }; //winding order(???)
			sphereSize += 6;
		}
	}
	GLuint vbo;
	glGenVertexArrays(1, &sphereVa);
	glBindVertexArray(sphereVa);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(face)* faceData.size(), faceData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vtxData), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vtxData), (void*)offsetof(vtxData, x));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vtxData), (void*)offsetof(vtxData, u));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	forwardProgram.sphereTex = loadTexture("textures/daSphere256.png");
	return sphereVa;
}

GLuint App::createSphereVBO(int resolution)
{
	float increment = (3.1415 * 2) / resolution;
	std::vector<vtxData> data;
	data.resize(resolution*(resolution / 2 + 1));
	float x, y, z;
	for (int i = 0; i < resolution / 2 + 1; i++)
	{
		for (int j = 0; j < resolution; j++) {
			x = sinf(j*increment)*sinf(i*increment);
			y = cosf(i*increment);
			z = cosf(j*increment)*sinf(i*increment);
			data[i*resolution + j] = { x / 2, //positions
				y / 2,
				z / 2,
				x,	//normals
				y,
				z,
				(float)j / (resolution), //uvs
				1 - (float)i / (resolution / 2)
			};
		}
	}
	int newRes = resolution;
	std::vector<face> faceData;
	vtxData t = { 1,1,1,1,1,1,1,1 };
	faceData.resize(2 * newRes*(newRes / 2 + 1) + 10, { t,t,t });
	for (int i = 0; i < newRes / 2; i++) {
		for (int j = 0; j < newRes; j++) {
			int a = i*newRes + j;
			int b = i*newRes + (j + 1) % newRes;
			int c = (i + 1)*newRes + j;
			int d = (i + 1)*newRes + (j + 1) % newRes;
			int index = 2 * (i*(newRes)+j);
			vtxData va = data[a];
			vtxData vb = data[b];
			vtxData vc = data[c];
			vtxData vd = data[d];
			if (j == newRes - 1) {
				vb.u = 1.0f;
				vd.u = 1.0f;
			}
			faceData[index] = { va, vc, vb }; //winding order(?)
			faceData[index + 1] = { vb, vc, vd }; //winding order(???)
			sphereSize += 6;
		}
	}
	GLuint vbo;
	glGenVertexArrays(1, &sphereVa);
	glBindVertexArray(sphereVa);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(face)* faceData.size(), faceData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vtxData), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vtxData), (void*)offsetof(vtxData, x));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vtxData), (void*)offsetof(vtxData, u));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	forwardProgram.sphereTex = loadTexture("textures/daSphere256.png");
	return sphereVa;
}


GLuint App::createCubeVBO()
{
	GLuint vbo;
	glGenVertexArrays(1, &cubeVa);
	glBindVertexArray(cubeVa);
	//Vertices
	glm::vec3 pos[8] = { {0.5, 0.5, 0.5},
						{0.5, 0.5, -0.5},
						{0.5, -0.5, 0.5},
						{0.5, -0.5, -0.5},
						{-0.5, 0.5, 0.5},
						{-0.5, 0.5, -0.5},
						{-0.5, -0.5, 0.5},
						{-0.5, -0.5, -0.5}
	};
	//Normals
	glm::vec3 norm[6] =
	{
		{ 0.0, 0.0, -1.0 },
		{ 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ 0.0, -1.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ -1.0, 0.0, 0.0 }
	};
	//UV coordinates
	glm::vec2 UVs[12] = { {0, 0 } ,    //0
						{ 0.5, 0 } ,   //1
						{ 1, 0 } ,	   //2
						{ 0, 1.f/3.f },	   //3
						{ 0.5, 1.f/3.f },  //4
						{ 1, 1.f / 3.f },	   //5
						{ 0, 2.f/3.f },	   //6
						{ 0.5, 2.f/3.f },  //7
						{ 1, 2.f/3.f },	   //8
						{ 0, 1 },	   //9
						{ 0.5, 1 },	   //10
						{ 1, 1 }	   //11
	};
	//Faces
	betterFace faces[12] = { { { pos[0], norm[4], UVs[10] },{ pos[1], norm[4], UVs[11] },{ pos[2], norm[4], UVs[7] } },
							{ { pos[1], norm[4], UVs[11] },{ pos[3], norm[4], UVs[8] },{ pos[2], norm[4], UVs[7] } },

							{ { pos[1], norm[1], UVs[5] },{ pos[0], norm[1], UVs[2] },{ pos[4], norm[1], UVs[1] } } ,
							{ { pos[1], norm[1], UVs[5] },{ pos[4], norm[1], UVs[1] },{ pos[5], norm[1], UVs[4] } } ,

							{ { pos[4], norm[5], UVs[8] },{ pos[7], norm[5], UVs[4] },{ pos[5], norm[5], UVs[7] } } ,
							{ { pos[4], norm[5], UVs[8] },{ pos[6], norm[5], UVs[5] },{ pos[7], norm[5], UVs[4] } } ,

							{ { pos[2], norm[3], UVs[7] },{ pos[3], norm[3], UVs[4] },{ pos[6], norm[3], UVs[6] } } ,
							{ { pos[3], norm[3], UVs[4] },{ pos[7], norm[3], UVs[3] },{ pos[6], norm[3], UVs[6] } } ,

							{ { pos[0], norm[2], UVs[10] },{ pos[6], norm[2], UVs[6] },{ pos[2], norm[2], UVs[7] } } ,
							{ { pos[0], norm[2], UVs[10] },{ pos[4], norm[2], UVs[9] },{ pos[6], norm[2], UVs[6] } } ,

							{ { pos[1], norm[0], UVs[1] },{ pos[5], norm[0], UVs[0] },{ pos[7], norm[0], UVs[3] } } ,
							{ { pos[1], norm[0], UVs[1] },{ pos[7], norm[0], UVs[3] },{ pos[3], norm[0], UVs[4] } }

	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(betterFace) * 12, &faces, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(betterData), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(betterData), (void*)offsetof(betterData, normals));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(betterData), (void*)offsetof(betterData, UV));

	forwardProgram.cubeTex = loadTexture("textures/rubik256.png");
	return cubeVa;
}

void App::createCubes()
{
	int index = 0;
	glm::vec3 pos;
	float radius = sqrtf(3) / 2;
	for (int y = 0; y < 4; y++) {
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++) {
				pos = glm::vec3(i * 2 * 2, y*2, j * 2 * 2);
				forwardProgram.cubeObjs[index].hb.init(pos, radius);
				forwardProgram.cubeObjs[index].fixed = false;
				cubeMatrices[index++] = glm::translate(glm::mat4(1), pos);
			}
		}
	}
}

void App::createSpheres()
{
	int index = 0;
	glm::vec3 pos;
	for (int y = 0; y < 4; y++) {
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++) {
				pos = glm::vec3(i * 2 * 2, y*2, (j * 2 + 1) * 2);
				forwardProgram.sphereObjs[index].hb.init(pos, 0.5f);
				forwardProgram.sphereObjs[index].fixed = false;
				sphereMatrices[index++] = glm::translate(glm::mat4(1), pos);
			}
		}
	}
}

GLuint App::loadTexture(std::string path)
{
	GLuint tex;
	GLint width, height, channel;
	unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &channel, 4);

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);// , 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void App::createFrameBuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &screenSaveTex);
	
	glBindTexture(GL_TEXTURE_2D, screenSaveTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Camera::SCREEN_WIDTH * 2, Camera::SCREEN_HEIGHT * 2, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenSaveTex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void App::saveFrameToFile(int nr)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, Camera::SCREEN_WIDTH, Camera::SCREEN_HEIGHT, 0, 0, Camera::SCREEN_WIDTH, Camera::SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	std::vector<unsigned char> imgData(Camera::SCREEN_WIDTH * Camera::SCREEN_HEIGHT  * 3);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, Camera::SCREEN_WIDTH, Camera::SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, &imgData[0]);

	std::string path = this->resultPath + "/imgFOR" + std::to_string(nr) + ".bmp";
	int err = SOIL_save_image(
		path.c_str(),
		SOIL_SAVE_TYPE_BMP,
		Camera::SCREEN_WIDTH, Camera::SCREEN_HEIGHT, 3,
		&imgData[0]
	);
}

double xpos, ypos, lastx, lasty;
void App::controls(float dt)
{
	glfwGetCursorPos(w, &xpos, &ypos);
	camera.update(lastx - xpos, lasty - ypos, dt);
	lastx = xpos;
	lasty = ypos;
	updateInputs();
	camera.move(movement, dt);
}

void App::run() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glfwGetCursorPos(w, &lastx, &lasty);
	double time = 0.0;
	glfwSetTime(time);

	int nrOfScreenShots = 0;
	int totalFrames = 0;
	glfwSwapInterval(0);
	if (!forwardProgram.dynamic)
	{
		nrOfScreenShots = 9;
	}
	while(!glfwWindowShouldClose(w) && running){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();	
		//controls(0.005f);
		forwardProgram.render(camera.view, camera.getViewProjection(), camera.cameraPos, 0.005f);
		glfwSwapBuffers(w);
		/*int a = glGetError();
		if (a) {
			std::cout << glewGetErrorString(a) << std::endl;
		}*/
		totalFrames++;
		if (totalFrames == runTime)
			running = false;
		if (totalFrames % everyXFrame == 0)
		{
			saveFrameToFile(nrOfScreenShots++);
		}
	}
	time = glfwGetTime();
	std::ofstream logFile;
	logFile.open(this->resultPath + "/logFOR.txt", std::ios_base::app);
	logFile << time << " " << forwardProgram.dynamic << " " << this->instanceID << "\n";
	logFile.close();
}

void GLAPIENTRY gl_callback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam)
{
	static std::set<GLuint> SHOWED_ID;

	if (type != GL_DEBUG_TYPE_ERROR) {
		if (SHOWED_ID.insert(id).second == false) return;
	}

	const char *stype = "",
		*sseverity = "",
		*ssource = "";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		stype = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		stype = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		stype = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		stype = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		stype = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		stype = "OTHER";
		break;
	case GL_DEBUG_TYPE_MARKER:
		stype = "MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		stype = "PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		stype = "POP_GROUP";
		break;
	default:
		stype = "UNKNOWN";
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		sseverity = "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		sseverity = "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		sseverity = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		sseverity = "NOTIFICATION";
		break;
	default:
		sseverity = "UNKNOWN";
	}
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		ssource = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		ssource = "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		ssource = "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		ssource = "THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		ssource = "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		ssource = "OTHER";
		break;
	default:
		ssource = "UNKNOWN";
	}
	printf("[OpenGL] [%i:%s] [%i:%s] [%i:%s] %u: %*s\n",
		(int)severity, sseverity,
		(int)type, stype,
		(int)source, ssource,
		id, length, message
	);
}
