// Daniel McGuinness 2018
//Normal Mapping adapted from https://learnopengl.com/Advanced-Lighting/Normal-Mapping
#version 450 core

// Define the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv; 
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent; 

// Outputs
out VertexData {
	vec4 fpos; 
	vec3 fnormal; 
	vec2 ftex;

};

out NormalMapping{
	vec3 eyedir_tan; 
	vec3 lightdir_tan; 
	vec3 eyedir_cam;
	vec3 lightdir_cam; 
};


// Uniforms
layout (std140) uniform MatrixStack {
	mat4 model;
	mat4 view; 
	mat4 projection; 
	mat3 normalmatrix;
};


void main()
{
	ftex = uv; 
	vec4 position_h = vec4(position, 1.0);						
	vec4 lightpos = vec4(0, 0, 10, 0);


	/*fdiffusecolour = vec4(1, 0, 1, 1);
	fambientcolour = fdiffusecolour * 0.2;
	fspecularcolour = vec4(1.0, 1.0, 1.0, 1.0);*/

	//Lighting Calculations
	fpos = view * model * position_h;										
	fnormal = normalize(normalmatrix * normal);
	//flightdir = lightpos.xyz - fpos.xyz;


	//Normal Mapping Calculations
	mat3 mv3 = mat3(view * model); 
	vec3 T = mv3 * normalize(tangent); 
	vec3 B = mv3 * normalize(bitangent); 
	vec3 N = mv3 * normalize(normal); 
	mat3 TBN = transpose(mat3(T, B, N)); 

	vec3 lightpos_cam = ( view * vec4((lightpos.xyz),1)).xyz;

	eyedir_cam = vec3(0, 0, 0) - fpos.xyz; 
	lightdir_cam = lightpos_cam + (vec3(0,0,0) -  fpos.xyz);
	
	eyedir_tan = TBN *  eyedir_cam; 
	lightdir_tan = TBN * lightdir_cam;
	

	gl_Position = (projection * view * model) * position_h;
	



	

}


