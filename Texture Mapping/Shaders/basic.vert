#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordonnees;

out vec2 TextureCoordonnees;

void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    TextureCoordonnees = textureCoordonnees;
}; 