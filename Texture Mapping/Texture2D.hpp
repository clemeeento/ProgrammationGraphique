#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include <string>
#include <GL/glew.h>

using std::string;

#define STB_IMAGE_IMPLEMENTATION

class Texture2D
{
public :
    Texture2D();
    virtual ~Texture2D();

    bool loadTexture(const string& filename, bool generateMipMaps = true); // Charger une texture
    void bind(GLuint texUnit = 0); // Lier la texture

private :
    GLuint mTexture; // Identifiant de la texture
};

#endif // TEXTURE2D_HPP