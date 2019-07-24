#pragma once

#include "windowmanager.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

class ObjLoader {
public:
	
	GLuint vao; 
	
	GLuint posBuff;
	GLuint normBuff;
	GLuint uvBuff;
	GLuint tanBuff; 
	GLuint biBuff; 

	GLuint shaderVert;
	GLuint shaderUV;
	GLuint shaderNorm;
	GLuint shaderTan; 
	GLuint shaderBi;

	std::vector<glm::vec3> objVerts;
	std::vector<glm::vec3> objNorms;
	std::vector<glm::vec2> objUVs;
	std::vector<glm::vec3> objTangents;
	std::vector<glm::vec3> objBitangents;

	ObjLoader();
	~ObjLoader();
	bool loadObjFile(const char * objPath, std::vector<glm::vec3>& verts, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& norms);
	void setupObject(const char * filename, bool tanActive);
	void generateTangents();
	void drawObject(bool tanActive);

private:



};