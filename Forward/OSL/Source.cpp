#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "App.h"
#include <sstream>
using namespace std;
int main(int argc, char* argv[])
{
	bool mode = false;
	int runTime = 10000;
	int shotRate = 9000;
	string destFolder = "";
	int instanceNumber = -1;
	if (argc == 6)
	{
		std::stringstream ss(argv[1]);
		ss >> std::boolalpha >> mode;
		runTime = atoi(argv[2]);
		shotRate = atoi(argv[3]);
		instanceNumber = atoi(argv[4]);
		destFolder = argv[5];
	}
	App app(mode, runTime, shotRate, instanceNumber, destFolder);
	app.run();

	return 0;
}