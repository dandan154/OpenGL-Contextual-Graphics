// Daniel McGuinness

#version 450 core

// Define the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv; 

// Outputs
out VertexData {
	vec4 fpos; 
	vec3 fnormal; 
	vec2 ftex;

};
out Lighting {
	vec4 fdiffusecolour;
	vec4 fambientcolour;
	vec4 fspecularcolour;
	vec3 flightdir;
};


// Uniforms
layout (std140) uniform MatrixStack {
	mat4 model;
	mat4 view; 
	mat4 projection; 
	mat3 normalmatrix;
};

//uniform vec4 lightpos; 

void main()
{
	ftex = uv; 

	vec4 position_h = vec4(position, 1.0);						
	vec4 lightpos = vec4(0, 0, 0, 0);

	fdiffusecolour = vec4(1, 1, 1, 1);
	fambientcolour = fdiffusecolour * 0.2;
	fspecularcolour = vec4(1.0, 1.0, 1.0, 1.0);

	//Lighting Calculations
	fpos = (view * model) * position_h;										
	fnormal = normalize(normalmatrix * normal);
	flightdir = lightpos.xyz - fpos.xyz;

	gl_Position = (projection * view * model) * position_h;
	



	

}


