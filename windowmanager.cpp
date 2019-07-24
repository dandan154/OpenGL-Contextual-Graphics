/*
	Daniel McGuinness
*/
#include "windowmanager.h"
#include <vector>

using namespace std; 


/* Default Constructor */
WindowManager::WindowManager() {

}

/*
WindowManager Constructor

params: window width (w), window height (h), window title (t)
returns: WindowManager instance
*/
WindowManager::WindowManager(int w, int h, const char* t) {
	
	width = w; 
	height = h; 
	title = t; 

	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	win = glfwCreateWindow(w, h, t, NULL, NULL);
	
	if (!win) {
		cout << "window creation failed - check glfwWindowHint commands" ; 
		glfwTerminate();
	}
	
	glfwMakeContextCurrent(win);
	glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);

}

/*
	WindowManager Destructor
*/
WindowManager::~WindowManager() {
	glfwTerminate(); 
}

/*
	Handles the main OpenGL rendering loop for drawing to screen 

	params: N/A
	returns: N/A
*/
void WindowManager::mainLoop() {
	
	while (!glfwWindowShouldClose(win))
	{
		renderer(); 
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwTerminate();
}

void WindowManager::setRenderer(void(*func)()) {
	renderer = func;
}

void WindowManager::setKeyCallback(void(*f)(GLFWwindow* window, int key, int scancode, int action, int mods))
{
	keyCallback = f;
	glfwSetKeyCallback(win, keyCallback);
}

void WindowManager::setResizeCallback(void(*f)(GLFWwindow* window, int width, int height)) {

	resizeCallback = f; 
	glfwSetWindowSizeCallback(win, resizeCallback);
}

GLuint WindowManager::buildShaders(const char*  vertexShaderPath, const char* fragmentShaderPath) {
	
	//Create Shaders
	GLuint vShaderID = glCreateShader(GL_VERTEX_SHADER); 
	GLuint fShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	//Read Shaders from File
	string vCode, fCode; 
	ifstream vStream(vertexShaderPath, ios::in), fStream(fragmentShaderPath, ios::in);

	if (vStream.is_open()) {
		stringstream sstr;
		sstr << vStream.rdbuf(); 
		vCode = sstr.str(); 
		vStream.close();
	}
	else {
		cout << "vertex shader stream failed";
		return 0; 
	}

	if (fStream.is_open()) {
		stringstream sstr;
		sstr << fStream.rdbuf();
		fCode = sstr.str();
		fStream.close();
	}
	else {
		cout << "fragment shader stream failed";
		return 0;
	}


	//Compile Shaders
	const char* vPointer = vCode.c_str(); 
	const char* fPointer = fCode.c_str(); 

	glShaderSource(vShaderID, 1, &vPointer, NULL);
	glShaderSource(fShaderID, 1, &fPointer, NULL);
	glCompileShader(vShaderID);
	glCompileShader(fShaderID);

	//Debugging
	GLint res = GL_FALSE; 
	GLint logLen; 

	//Vertex Shader Debugging
	glGetShaderiv(vShaderID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(vShaderID, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 0) {
		printf("vertex shader debugging \n");
		vector<char> vc(logLen + 1);
		glGetShaderInfoLog(vShaderID, logLen, NULL, &vc[0]);
		printf("%s\n", &vc[0]); 
	}

	//Fragment Shader Debugging
	glGetShaderiv(fShaderID, GL_COMPILE_STATUS, &res);
	glGetShaderiv(fShaderID, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 0) {
		printf("fragment shader debugging \n"); 
		vector<char> vc(logLen + 1);
		glGetShaderInfoLog(fShaderID, logLen, NULL, &vc[0]);
		printf("%s\n", &vc[0]);
	}


	//Link the Program
	GLuint progID = glCreateProgram(); 
	glAttachShader(progID, vShaderID);
	glAttachShader(progID, fShaderID);
	glLinkProgram(progID);

	//Clean-Up
	glDetachShader(progID, vShaderID);
	glDetachShader(progID, fShaderID);
	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);

	return progID; 
}
