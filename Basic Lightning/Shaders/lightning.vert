#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TextureCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(pos,1.0f));

    gl_Position = projection * view * model * vec4(pos, 1.0);
    TextureCoord = textureCoord;
}; 