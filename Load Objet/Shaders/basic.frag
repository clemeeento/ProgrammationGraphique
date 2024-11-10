#version 330 core

in vec2 TextureCoordonnees;

out vec4 fragColor; 

uniform sampler2D myTexture1;

void main()
{
    fragColor = texture(myTexture1, TextureCoordonnees);
}; 