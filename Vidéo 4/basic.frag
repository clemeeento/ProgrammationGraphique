#version 330 core

//in vec3 color;

uniform vec4 vertColor;

out vec4 fragColor; 

void main()
{
    fragColor = vertColor;
}; 