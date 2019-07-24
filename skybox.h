#pragma once
#include "windowmanager.h"
#include "SOIL2/SOIL2.h" //x64
#include <vector>
#include <stdio.h>
#include <glm/glm.hpp>


class Skybox {
public:
	GLuint boxTextureID;

	GLuint vao; 
	GLuint vboPos;
	GLuint shaderPos; 
	
	GLuint texUniform;
	int posSize; 
	
	Skybox();
	~Skybox(); 

	void setupSkybox(GLuint prog);
	void drawSkybox();

private:
	void loadTextures();

};
