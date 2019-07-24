#pragma once

#include "windowmanager.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

class Ground {

public:
	Ground(); 
	~Ground(); 

	void setupGround();
	void perlinNoiseCalc();
	void drawGround(); 

private:

	GLuint posBuff;
	GLuint normBuff;
	GLuint uvBuff;

	GLuint shaderVert;
	GLuint shaderUV;
	GLuint shaderNorm;

	GLuint xPixelSize, zPixelSize; 
	GLfloat pScale, pFreq;
	GLuint pOct; 

	std::vector<glm::vec3> groundVerts;
	std::vector<glm::vec3> groundNorms;
	std::vector<glm::vec2> groundUVs;

	GLfloat* noise; 

};