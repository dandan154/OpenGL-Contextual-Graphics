/*
	Daniel McGuinness
*/

#include "obj_loader.h"
#include "SOIL2/SOIL2.h" //x64

using namespace std;
using namespace glm;

ObjLoader::ObjLoader() {
	shaderVert = 0;
	shaderNorm = 1;
	shaderUV = 2;
	shaderTan = 3; 
	shaderBi = 4;
}

ObjLoader::~ObjLoader() {  

}


/*
	.Obj Model Loader for loading in Obj Models
	Adapted from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

*/
bool ObjLoader::loadObjFile(const char * objPath, vector<vec3> &verts, vector<vec2> &uvs, vector<vec3> &norms) {

	vector<GLuint> vertInd, uvInd, normInd;
	vector<vec3> tmpVert, tmpNorm;
	vector<vec2> tmpUV;

	FILE * objFile = fopen(objPath, "r");

	if (objFile == NULL) {
		cout << "OBJ file failed to open" << endl;
		return false;
	}
	else {
		cout << "OBJ file opened" << endl;
	}

	cout << "Loading OBJ model(may take a few seconds)..." << endl;
	while (1) {

		char lineHead[128];
		int result = fscanf(objFile, "%s", lineHead);

		if (result == EOF) {
			break;
		}


		if (strcmp(lineHead, "v") == 0) {
			vec3 vertex;
			fscanf(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tmpVert.push_back(vertex);
		}
		else if (strcmp(lineHead, "vt") == 0) {
			vec2 uv;
			fscanf(objFile, "%f %f\n", &uv.x, &uv.y);
			tmpUV.push_back(uv);
		}
		else if (strcmp(lineHead, "vn") == 0) {
			vec3 norm;
			fscanf(objFile, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
			tmpNorm.push_back(norm);
		}
		else if (strcmp(lineHead, "f") == 0) {
			string vert1, vert2, vert3;
			GLuint vertIndex[3], uvIndex[3], normIndex[3];
			int match = fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertIndex[0], &uvIndex[0], &normIndex[0], &vertIndex[1], &uvIndex[1], &normIndex[1], &vertIndex[2], &uvIndex[2], &normIndex[2]);

			if (match != 9) {
				cout << "Obj File parse has failed - incorrect format" << endl;
			}

			vertInd.push_back(vertIndex[0]);
			vertInd.push_back(vertIndex[1]);
			vertInd.push_back(vertIndex[2]);

			uvInd.push_back(uvIndex[0]);
			uvInd.push_back(uvIndex[1]);
			uvInd.push_back(uvIndex[2]);

			normInd.push_back(normIndex[0]);
			normInd.push_back(normIndex[1]);
			normInd.push_back(normIndex[2]);

		}

		for (int i = 0; i < vertInd.size(); i++) {
			vec3 vertex;
			GLuint vertIndex = vertInd[i];
			vertex = tmpVert[vertIndex - 1];
			verts.push_back(vertex);
		}

		for (int i = 0; i < uvInd.size(); i++) {
			vec2 uv;
			GLuint uvIndex = uvInd[i];
			uv = tmpUV[uvIndex - 1];
			uvs.push_back(uv);
		}

		for (int i = 0; i < normInd.size(); i++) {
			vec3 norm;
			GLuint normIndex = normInd[i];
			norm = tmpNorm[normIndex - 1];
			norms.push_back(norm);
		}

	}

	cout << "OBJ model loaded" << endl;
	//fclose(objFile);
	return true;

}

/*
	Prepare Obj for rendering - load Object from file and bind data to buffers and VAO
*/
void ObjLoader::setupObject(const char * filename, bool tanActive)
{


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	bool res = loadObjFile(filename, objVerts, objUVs, objNorms);

	generateTangents(); 

	glGenBuffers(1, &posBuff);
	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glBufferData(GL_ARRAY_BUFFER, objVerts.size() * sizeof(vec3), &objVerts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &uvBuff);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuff);
	glBufferData(GL_ARRAY_BUFFER, objUVs.size() * sizeof(vec2), &objUVs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normBuff);
	glBindBuffer(GL_ARRAY_BUFFER, normBuff);
	glBufferData(GL_ARRAY_BUFFER, objNorms.size() * sizeof(vec3), &objNorms[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (tanActive) {
		generateTangents();

		glGenBuffers(1, &tanBuff);
		glBindBuffer(GL_ARRAY_BUFFER, tanBuff);
		glBufferData(GL_ARRAY_BUFFER, objTangents.size() * sizeof(vec3), &objTangents[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &biBuff);
		glBindBuffer(GL_ARRAY_BUFFER, biBuff);
		glBufferData(GL_ARRAY_BUFFER, objBitangents.size() * sizeof(vec3), &objBitangents[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

/*
	Calculate Tangents and BiTangents for use in Normal Mapping
*/
void ObjLoader::generateTangents() {

	vec3 pos1, pos2, pos3; 
	vec3 edgePos1, edgePos2; 

	vec2 uv1, uv2, uv3; 
	vec2 edgeUV1, edgeUV2; 

	vec3 finalTan, finalBi; 

	float result; 

	for (int i = 0; i < objVerts.size(); i=i+3) {
		//Fetch vertex positions
		pos1 = objVerts[i];
		pos2 = objVerts[i + 1];
		pos3 = objVerts[i + 2];

		//Calculate position deltas
		edgePos1 = pos2 - pos1; 
		edgePos2 = pos3 - pos1; 

		//Fetch vertex UVs
		uv1 = objUVs[i];
		uv2 = objUVs[i + 1];
		uv3 = objUVs[i + 2];

		//Calculate UV deltas
		edgeUV1 = uv2 - uv1; 
		edgeUV2 = uv3 - uv1; 

		//Calculate Tangent and Bitangent
		result = 1 / (edgeUV1.x * edgeUV2.y - edgeUV1.y * edgeUV2.x);
		finalTan = (edgePos1 * edgeUV2.y - edgePos2 * edgeUV1.y) * result; 
		finalBi = (edgePos2 * edgeUV1.x - edgePos1 * edgeUV2.x) * result; 


		//Create Tangent and Bitangent vectors to generate buffers
		for (int j = 0; j < 3; j++) {
			objTangents.push_back(finalTan);
			objBitangents.push_back(finalBi);
		}
	}

}
/*
	Render Object using data from bound buffers
*/
void ObjLoader::drawObject(bool tanActive)
{

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glEnableVertexAttribArray(shaderVert);
	glVertexAttribPointer(shaderVert, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normBuff);
	glEnableVertexAttribArray(shaderNorm);
	glVertexAttribPointer(shaderNorm, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuff);
	glEnableVertexAttribArray(shaderUV);
	glVertexAttribPointer(shaderUV, 2, GL_FLOAT, GL_FALSE, 0, 0);

	if (tanActive) {
		glBindBuffer(GL_ARRAY_BUFFER, tanBuff);
		glEnableVertexAttribArray(shaderTan);
		glVertexAttribPointer(shaderTan, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, biBuff);
		glEnableVertexAttribArray(shaderBi);
		glVertexAttribPointer(shaderBi, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glDrawArrays(GL_TRIANGLES, 0, objVerts.size());

	glDisableVertexAttribArray(shaderVert);
	glDisableVertexAttribArray(shaderNorm);
	glDisableVertexAttribArray(shaderUV);

	if (tanActive) {
		glDisableVertexAttribArray(shaderBi);
		glDisableVertexAttribArray(shaderTan);
	}
}
