#include "ground.h"

using namespace std;
using namespace glm;

Ground::Ground()
{
	shaderVert = 0;
	shaderNorm = 1;
	shaderUV = 2;
}

Ground::~Ground()
{
}


void Ground::setupGround()
{

	//DEFAULT TESTING VALUES
	xPixelSize = zPixelSize = 50; //Set vertex count
	pFreq = 1; 
	pScale = 2; 
	pOct = 1; 

	perlinNoiseCalc(); 

	glGenBuffers(1, &posBuff);
	glBindBuffer(GL_ARRAY_BUFFER, posBuff);
	glBufferData(GL_ARRAY_BUFFER, groundVerts.size() * sizeof(vec3), &groundVerts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &uvBuff);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuff);
	glBufferData(GL_ARRAY_BUFFER, groundUVs.size() * sizeof(vec2), &groundUVs[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normBuff);
	glBindBuffer(GL_ARRAY_BUFFER, normBuff);
	glBufferData(GL_ARRAY_BUFFER, groundNorms.size() * sizeof(vec3), &groundNorms[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Ground::perlinNoiseCalc() {

	GLfloat xFact, zFact, curFreq, curScale;
	GLfloat x, z, sum, res, val; 
	vec2 p; 

	int noiseSize = xPixelSize * zPixelSize * pOct;

	//Initialize noise value array
	noise = new GLfloat[noiseSize];
	for (int i = 0; i < noiseSize; i++) {
		noise[i] = 0; 
	}

	xFact = 1 / (xPixelSize - 1);
	zFact = 1 / (zPixelSize - 1);



	for (int i = 0; i < xPixelSize; i++) {
		for (int j = 0; j < zPixelSize; j++) {
			x = xFact * j; 
			z = zFact * i; 

			//Reset octave loop variables
			curFreq = pFreq;
			curScale = pScale;
			sum = 0;


			for (int k = 0; k < pOct; k++) {
				
				//Calculate and store perlin noise value
				p = vec2(x*curFreq, z*curFreq);
				val = perlin(p) / curScale; 
				sum += val;
				res = (sum + 1) / 2; 
	
				noise[(i *xPixelSize + j) + pOct + k] = res; 

				curFreq = curFreq * 2; 
				curScale = curScale * pScale; 
			}

		}
	}
}

void Ground::drawGround()
{
}
