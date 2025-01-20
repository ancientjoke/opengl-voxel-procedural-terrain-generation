#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aType;
layout (location = 2) in vec3 aNormal;

out vec3 ourColor;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const uint BlockType_Snow = uint(1);
const uint BlockType_Rock = uint(2);
const uint BlockType_Dirt = uint(3);
const uint BlockType_Grass = uint(4);
const uint BlockType_Sand = uint(5);
const uint BlockType_Shallow = uint(6);

void main()
{
	// Deciding cube color depending on it's type
	if (aType == BlockType_Snow)
	    ourColor = vec3(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
	else if (aType == BlockType_Rock)
	    ourColor = vec3(142.0f/255.0f, 142.0f/255.0f, 142.0f/255.0f);
	else if (aType == BlockType_Dirt)
	    ourColor = vec3(113.0f/255.0f, 79.0f/255.0f, 54.0f/255.0f);
	else if (aType == BlockType_Grass)
	    ourColor = vec3(11.0f/255.0f, 128.0f/255.0f, 0.0f/255.0f);
	else if (aType == BlockType_Sand)
	    ourColor = vec3(240.0f/255.0f, 240.0f/255.0f, 64.0f/255.0f);
	else
	    ourColor = vec3(0.0f/255.0f, 0.0f/255.0f, 255.0f/255.0f);

	vec3 FragPos = vec3(model * vec4(aPos.xyz, 1.0));
	//Normal = mat3(transpose(inverse(model))) * aNormal;
    Normal = aNormal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}