// Daniel McGuinness

#version 450 core

// Shader Inputs

in VertexData {
	vec4 fpos; 
	vec3 fnormal; 
	vec2 ftex;
};

in Lighting {
	vec4 fdiffusecolour;
	vec4 fambientcolour;
	vec4 fspecularcolour;
	vec3 flightdir;
};

// Uniforms

uniform sampler2D tex1; 


layout (std140) uniform Control {
	uint emitmode;
	uint attmode; 
	uint texturemode; 
};

layout (std140) uniform Attenuation {
	float att_factor; 
	float k1;
	float k2;
	float k3;
};

// Shader Outputs
out vec4 outputColor;


float shine = 8; 

void main()
{


	//Set texture for fragment
	vec4 texcolour = texture(tex1, ftex);

	//Normalize interpolated vectors - occurs during rasterization stage 
	vec3 L = normalize(flightdir);
	vec3 N = normalize(fnormal);

	//Diffuse Lighting
	vec4 diffuse = max(dot(fnormal, L), 0.0) * fdiffusecolour;

	//Specular lighting
	vec3 V = normalize(-fpos.xyz);	
	vec3 R = reflect(-L, N);
	vec4 specular = pow(max(dot(R, V), 0.0), shine) * fspecularcolour;

	//Emissive lighting
	vec4 emissive = vec4(0);
	if(emitmode == 1){ emissive = vec4(1.0, 1.0, 0.8, 1.0);}

	//Attenuation - Applied to diffuse and specular 
	float light_distance = length(flightdir);
	float att = att_factor/(k1 + k2*light_distance + k3 * pow(light_distance,2));


	// Mix the fog with lit pixel fragment colour
	vec4 shadedColor = att * (diffuse + specular) + fambientcolour + emissive;
	
	outputColor = shadedColor * texcolour;
}