//Daniel McGuinness
#version 450 core

layout(location = 0) in vec3 position; 

// Uniforms
layout (std140) uniform MatrixStack {
	mat4 model;
	mat4 view; 
	mat4 projection; 
	mat3 normalmatrix;
};

out vec3 texCoords;

void main(){

	vec4 position_h = vec4(position, 1.0); 
	texCoords = position; 
	gl_Position = (( projection * model ) * position_h).xyww;
	

}