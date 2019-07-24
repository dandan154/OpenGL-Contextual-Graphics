/*
	Daniel McGuinness
*/

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#define GLEW_STATIC
#include "GL/glew.h"	//x64
#include "GLFW/glfw3.h" //x64 VS2015 edition

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

class WindowManager {
private:
	GLFWwindow* win;
	const char* title; 
	int width;
	int height;
	void(*renderer)(); 
	void(*keyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods);
	void(*resizeCallback)(GLFWwindow* window, int width, int height);

public:
	WindowManager();
	WindowManager(int width, int height, const char* title);
	~WindowManager();

	void mainLoop();
	void setRenderer(void (*f)());
	void setKeyCallback(void(*f)(GLFWwindow* window, int key, int scancode, int action, int mods));
	void setResizeCallback(void(*f)(GLFWwindow* window, int width, int height));


	GLuint buildShaders(const char*  vertexShaderPath, const char* fragmentShaderPath);

};
#endif