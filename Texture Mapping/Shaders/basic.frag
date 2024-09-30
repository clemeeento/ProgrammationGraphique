#version 330 core

in vec2 TextureCoordonnees;

out vec4 fragColor; 

uniform sampler2D myTexture1;
uniform sampler2D myTexture2;

void main()
{
    fragColor = mix(texture(myTexture1, TextureCoordonnees), texture(myTexture2, TextureCoordonnees), 0.9);
}; 