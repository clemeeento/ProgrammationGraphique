#version 330 core

//out vec3 color;

layout (location = 0) in vec3 position;

uniform vec2 posOffset;

void main()
{
    gl_Position = vec4(position.x + posOffset.x, position.y + posOffset.y, position.z, 1.0);
}; 