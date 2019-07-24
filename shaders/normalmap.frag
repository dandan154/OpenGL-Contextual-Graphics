// Daniel McGuinness
#version 450 core

// Shader Inputs

in VertexData {
	vec4 fpos; 
	vec3 fnormal; 
	vec2 ftex;
};

in NormalMapping{
	vec3 eyedir_tan; 
	vec3 lightdir_tan; 
	vec3 eyedir_cam;
	vec3 lightdir_cam; 
};


// Uniforms

//Textures
layout (binding = 0)uniform sampler2D tex1; 
layout (binding = 1) uniform sampler2D normTex1; 


// Shader Outputs
out vec4 fColour;

void main()
{
	//Light variables
	vec4 lightCol = vec4(1,1,1,1);
	float lightVal = 0.4;
	vec4 lightStr = lightCol * lightVal; 

	//Initial Lighting Colours
	vec4 diffuseCol = texture( tex1, ftex );
	vec4 ambientCol =  diffuseCol * 0.2;
	vec4 specularCol = texture( tex1, ftex ) * 0.3;

	//Attenuation
	float dis = length( vec3(0,0,0) - fpos.xyz);
	float attenFactor = 1 / (pow(dis,2));

	//Diffuse Lighting Calculations
	vec3 N = normalize(texture( normTex1, ftex).rgb*2.0 - 1.0);
	vec3 L = normalize(lightdir_tan);
	diffuseCol = diffuseCol * lightStr * clamp( dot( N,L ), 0,1 ) * attenFactor;

	//Specular Lighting Calculations
	vec3 E = normalize(eyedir_tan);
	vec3 R = reflect(-L,N);
	specularCol = specularCol * lightStr * pow(clamp( dot( E,R ), 0,1 ) ,5) * attenFactor;

	//Final Output
	fColour = ambientCol + diffuseCol + specularCol;

		

}