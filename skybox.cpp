/*
	Daniel McGuinness
*/

#include "skybox.h"
#include <iostream>

Skybox::Skybox() {
	shaderPos = 0;
}

Skybox::~Skybox()
{
	glDeleteBuffers(1, &vboPos);
}


void Skybox::loadTextures() {

	glGenTextures(1, &boxTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, boxTextureID);

	boxTextureID = SOIL_load_OGL_cubemap
	(
		"..\\textures\\skybox\\left.png",
		"..\\textures\\skybox\\right.png",
		"..\\textures\\skybox\\up.png",
		"..\\textures\\skybox\\down.png",
		"..\\textures\\skybox\\front.png",
		"..\\textures\\skybox\\back.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

void Skybox::setupSkybox(GLuint prog) {

	GLfloat vertPos[] = {

		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	posSize = sizeof(vertPos);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboPos);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertPos), &vertPos, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	texUniform = glGetUniformLocation(prog, "skybox");
	loadTextures(); 

}


void Skybox::drawSkybox() {
 

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, boxTextureID);
	glUniform1i(texUniform, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glEnableVertexAttribArray(shaderPos);
	glVertexAttribPointer(shaderPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(shaderPos);
	glBindVertexArray(0);

}
