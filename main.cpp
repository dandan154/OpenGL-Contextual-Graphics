/*
	Daniel McGuinness
*/

#include "windowmanager.h"

/*Include Libraries*/
#include <glm/glm.hpp> //0.9.9.2
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "SOIL2/SOIL2.h" //x64

#include "obj_loader.h"
#include "skybox.h"

#include <iostream>
#include <stack>

using namespace std;
using namespace glm;


//STD140 variable sizes - for more info see OpenGL 4.5 core profile, Section 7.6.2.2 (Standard Uniform Block Layout), page 137 
const int STD140_MAT4 = 64;
const int STD140_MAT3 = 48;
const int STD140_VEC4 = 16;
const int STD140_VEC3 = 16;
const int STD140_VEC2 = 8;
const int STD140_SCALAR = 4;

//Total Number of Programs
const int PROG_COUNT = 3; 

/* Window */
GLfloat winWidth = 1024; 
GLfloat winHeight = 768; 

/* Program ID */
GLuint program[PROG_COUNT];


/* Uniform Buffer Objects */

GLuint matrixBlock, fogBlock, controlBlock, attenBlock, lightblock;
GLuint uMatrixBlockIndex[PROG_COUNT], uFogBlockIndex[PROG_COUNT], uControlBlockIndex[PROG_COUNT], uAttenBlockIndex[PROG_COUNT], uLightBlockIndex[PROG_COUNT];

//UBO binding points
GLuint uMatrixBindingIndex = 1; 
GLuint uFogBindingIndex = 2;
GLuint uControlBindingIndex = 3; 
GLuint uAttenBindingIndex = 4;
GLuint uLightBindingIndex = 5; 

//UBO sizes in std140 format
int uMatrixSize = 3 * STD140_MAT4 + STD140_MAT3;	//Mat4 + Mat4 + Mat4 + Mat3
int uFogSize = STD140_VEC4 + STD140_SCALAR;			//Vec4  + scalar 
int uControlSize = 3 * STD140_SCALAR;				//scalar + scalar + scalar 
int uAttenSize = 4 * STD140_SCALAR;					//scalar + scalar + scalar + scalar
int uLightSize; 

/* Textures */
GLuint houseTextureID, houseNormID, lanternTextureID, groundTextureID;
GLuint houseTexUniformID, houseNormUniformID, standardTexUniformID;

/* Uniform Variables */
//Matrix
mat4 model, view, proj;
mat3 normal;

//Fog
vec4 fogColour = vec4(1, 0, 0, 1); 
GLfloat fogDensity = 0.2f; 

//Attenuation
GLfloat attFactor = 1.0f;
GLfloat k1 = 0.5f;
GLfloat k2 = 0.5f;
GLfloat k3 = 0.5f;


/* Model-View-Projection variables */
//View
vec3 camPos = vec3(0, 0, 5);
vec3 camDir = vec3(0, 0, 0);
vec3 camUp = vec3(0, 1, 0);

//Projection
GLfloat fov = 70.0f;
GLfloat clipNear = 0.1f;
GLfloat clipFar = 100.0f;

/* User Controls */
GLfloat xMove, yMove, zMove, yRot;
GLfloat xInc, yInc, zInc, yRotInc;



/* Scene Objects */
ObjLoader house, lantern, ground;
Skybox skybox; 

/*
	Creates buffers for uniform variables, allocates space in memory and binds static data
*/
void setupUniformBuffers(GLuint prog[]) {
	

	for (int i = 0; i < PROG_COUNT; i++) {
		//Set Uniform block bindings
		uMatrixBlockIndex[i] = glGetUniformBlockIndex(prog[i], "MatrixStack");
		uFogBlockIndex[i] = glGetUniformBlockIndex(prog[i], "Fog");
		uControlBlockIndex[i] = glGetUniformBlockIndex(prog[i], "Control");
		uAttenBlockIndex[i] = glGetUniformBlockIndex(prog[i], "Attenuation");

		glUniformBlockBinding(prog[i], uMatrixBlockIndex[i], uMatrixBindingIndex);
		glUniformBlockBinding(prog[i], uFogBlockIndex[i], uFogBindingIndex);
		glUniformBlockBinding(prog[i], uControlBlockIndex[i], uControlBindingIndex);
		glUniformBlockBinding(prog[i], uAttenBlockIndex[i], uAttenBindingIndex);
	}
	//Generate buffers for uniforms and allocate space
	glGenBuffers(1, &matrixBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock);
	glBufferData(GL_UNIFORM_BUFFER, uMatrixSize, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, uMatrixBindingIndex, matrixBlock, 0, uMatrixSize);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	glGenBuffers(1, &fogBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, fogBlock);
	glBufferData(GL_UNIFORM_BUFFER, uFogSize, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, uFogBindingIndex, fogBlock, 0, uFogSize);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &controlBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, controlBlock);
	glBufferData(GL_UNIFORM_BUFFER, uControlSize, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, uControlBindingIndex, controlBlock, 0, uControlSize);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &attenBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, attenBlock);
	glBufferData(GL_UNIFORM_BUFFER, uAttenSize, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, uAttenBindingIndex, attenBlock, 0, uAttenSize);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	//Assign values to uniform buffers with static data
	glBindBuffer(GL_UNIFORM_BUFFER, fogBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_VEC4, value_ptr(fogColour));
	glBufferSubData(GL_UNIFORM_BUFFER, STD140_VEC4, STD140_SCALAR, &fogDensity);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, attenBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_SCALAR, &attFactor);
	glBufferSubData(GL_UNIFORM_BUFFER, STD140_SCALAR, STD140_SCALAR, &k1);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * STD140_SCALAR, STD140_SCALAR, &k2);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * STD140_SCALAR, STD140_SCALAR, &k3);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

/*
	Prints instruction set to console
*/
void instruct() {
	cout << "\n" << endl;
	cout << "Camera Controls" << endl;
	cout << "===============" << endl;
	cout << "WS - Forwards and Backwards" << endl;
	cout << "AD - Left and Right" << endl;
	cout << "RF - Up and Down\n" << endl;
	cout << "[ ] - rotate around Y-axis" << endl;

}


/*
	Initialization
*/
void init(WindowManager* m) {

	//Initialize Control Variables
	xMove = yMove = zMove = yRot = 0.0f;
	xInc = yInc = zInc = 0.05f;
	yRotInc = 0.0f; 

	//Build Shader Programs
	try
	{

		cout << "Program 0 - Standard vertex and fragment shaders" << endl;
		program[0] = m->buildShaders("..\\shaders\\standard.vert", "..\\shaders\\standard.frag");

		cout << "Program 1 - Skybox vertex and fragment shaders" << endl; 
		program[1] = m->buildShaders("..\\shaders\\cubemap.vert", "..\\shaders\\cubemap.frag");

		cout << "Program 2 - Normal Mapping vertex and fragment shaders" << endl;
		program[2] = m->buildShaders("..\\shaders\\normalmap.vert", "..\\shaders\\normalmap.frag");
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	setupUniformBuffers(program);

	
	//Setup Skybox Object 
	skybox = Skybox(); 
	skybox.setupSkybox(program[1]);

	//Setup House Object
	house = ObjLoader(); 
	house.setupObject("..\\models\\house.obj", true);
	houseTextureID = SOIL_load_OGL_texture("..\\textures\\house_2_diffuse.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	houseNormID = SOIL_load_OGL_texture("..\\textures\\house_2_normal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	//Setup Latern Object
	lantern = ObjLoader(); 
	lantern.setupObject("..\\models\\lantern.obj", false); 
	lanternTextureID = SOIL_load_OGL_texture("..\\textures\\lantern_diffuse.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);


	//Set up Ground Object
	ground = ObjLoader();
	ground.setupObject("..\\models\\ground.obj", false);
	groundTextureID = SOIL_load_OGL_texture("..\\textures\\ground_diffuse.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);


	//Setup Texture Uniforms
	houseTexUniformID = glGetUniformLocation(program[2], "tex1");
	houseNormUniformID = glGetUniformLocation(program[2], "normTex1");
	standardTexUniformID = glGetUniformLocation(program[0], "tex1");

}


void controls(GLFWwindow* window, int key, int s, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//Model Transformation Controls
	if (key == 'D') { xMove = xMove - xInc; }
	if (key == 'A') { xMove = xMove + xInc; }

	if (key == 'F') { yMove = yMove + yInc; }
	if (key == 'R') { yMove = yMove - yInc; }

	if (key == 'S') { zMove = zMove - zInc; }
	if (key == 'W') { zMove = zMove + zInc; }

	if (key == '[') { yRotInc = yRotInc + 0.05f; }
	if (key == ']') { yRotInc = yRotInc - 0.05f; }

}



void displayObjects() {


	//Projection Matrix
	proj = perspective(radians(fov), (winWidth / winHeight), clipNear, clipFar);

	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 2* STD140_MAT4, STD140_MAT4, value_ptr(proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);//*/

	//View Matrix
	view = lookAt(camPos, camDir, camUp);

	glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock); 
	glBufferSubData(GL_UNIFORM_BUFFER, STD140_MAT4, STD140_MAT4, value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	//Model-View Matrix Stack
	stack<mat4> modelStack;
	modelStack.push(mat4(1.0f));
	
	//Apply Unique rotation transformations to the skybox
	modelStack.push(modelStack.top());
	{

		modelStack.top() = rotate(modelStack.top(), radians(yRot), vec3(0, 1, 0));

		glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_MAT4, value_ptr(modelStack.top()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//Allow new objects to be drawn over skybox
		glUseProgram(program[1]);
		glDepthFunc(GL_LEQUAL);
		skybox.drawSkybox();
		glDepthFunc(GL_LESS);
	}
	modelStack.pop(); 

	//Lantern Transformations - These follow the 'camera' 
	modelStack.push(modelStack.top());
	{
		glUseProgram(program[0]);

		modelStack.top() = translate(modelStack.top(), vec3(0.1f, -0.1f, 4.8f));
		modelStack.top() = scale(modelStack.top(), vec3(0.15, 0.15, 0.15));
		mat3 normmv = transpose(inverse(mat3(view * modelStack.top())));

		glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_MAT4, value_ptr(modelStack.top()));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * STD140_MAT4, STD140_MAT3, value_ptr(normmv));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		GLuint emit = 1;
		glBindBuffer(GL_UNIFORM_BUFFER, controlBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_SCALAR, &emit);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lanternTextureID);
		glUniform1i(lanternTextureID, 0);

		lantern.drawObject(false);
	}
	modelStack.pop();

	// Global transformations
	modelStack.top() = rotate(modelStack.top(), radians(yRot), vec3(0, 1, 0));
	modelStack.top() = translate(modelStack.top(), vec3(xMove, yMove, zMove));


	//House Transformations
	modelStack.push(modelStack.top()); 
	{	

		glUseProgram(program[2]);

		modelStack.top() = translate(modelStack.top(), vec3(3.0f, -2.0f, 0.6f));
		modelStack.top() = scale(modelStack.top(), vec3(0.005f, 0.005f, 0.005f));

		mat3 normmv = transpose(inverse(mat3(view * modelStack.top())));

		glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_MAT4, value_ptr(modelStack.top()));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * STD140_MAT4, STD140_MAT3, value_ptr(normmv));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, houseTextureID);
		glUniform1i(houseTexUniformID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, houseNormID);
		glUniform1i(houseNormUniformID, 1);

		house.drawObject(true); 

	}
	modelStack.pop(); 

	//Terrain Transformations
	modelStack.push(modelStack.top());
	{
		glUseProgram(program[0]);


		modelStack.top() = translate(modelStack.top(), vec3(0, -14, 0));
		modelStack.top() = scale(modelStack.top(), vec3(0.2, 0.2, 0.2));

		mat3 normmv = transpose(inverse(mat3(view * modelStack.top())));

		glBindBuffer(GL_UNIFORM_BUFFER, matrixBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_MAT4, value_ptr(modelStack.top()));
		glBufferSubData(GL_UNIFORM_BUFFER, 3 * STD140_MAT4, STD140_MAT3, value_ptr(normmv));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		GLuint emit = 0;
		glBindBuffer(GL_UNIFORM_BUFFER, controlBlock);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, STD140_SCALAR, &emit);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, groundTextureID);
		glUniform1i(groundTextureID, 0);

		ground.drawObject(false);
	}
	modelStack.pop();

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	yRot += yRotInc; 
}

void render() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program[0]);

	displayObjects();

	glDisableVertexAttribArray(0);
	glUseProgram(0);
}


void resize(GLFWwindow* window, int width, int height) {
	
	winWidth = width; 
	winHeight = height; 
	glfwSetWindowSize(window, width, height);
}

int main() {

	if (!glfwInit()) { glfwTerminate(); };

	//create window and make current context
	WindowManager* m = new WindowManager(winWidth, winHeight, "Assignment 2 - Daniel McGuinness");

	glewExperimental = GL_TRUE;
	glewInit();

	init(m);
	
	instruct(); 

	m->setRenderer(render);
	m->setKeyCallback(controls);
	m->setResizeCallback(resize);
	m->mainLoop(); 
}