//Daniel McGuinness
#version 450 core

in vec3 texCoords;

layout(binding = 0) uniform samplerCube skybox;

out vec4 outputColour;

void main()
{    
    outputColour = texture(skybox, texCoords);
}