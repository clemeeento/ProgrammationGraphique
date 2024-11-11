#include "Texture2D.hpp"
#include <stb/stb_image.h>


Texture2D::Texture2D() : mTexture(0)
{

}

Texture2D::~Texture2D()
{

}

// Charger une texture
bool Texture2D::loadTexture(const string& filename, bool generateMipMaps)
{
    int width, height, components;

    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);

    if(imageData == NULL)
    {
        fprintf(stderr, "Erreur lors du chargement de la texture %s\n", filename.c_str());
        return false;
    }

    //Inverser l'image horizontalement
    int widthInBytes = width * 4;
    unsigned char* top = NULL;
    unsigned char* bottom = NULL;
    unsigned char temp = 0;
    int halfHeight = height / 2;
    for(int row = 0; row < halfHeight; row = row + 1)
    {
        top = imageData + row * widthInBytes;
        bottom = imageData + (height - row - 1) * widthInBytes;
        for(int col = 0; col < widthInBytes; col = col + 1)
        {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top = top + 1;
            bottom = bottom + 1;
        }
    }

    glGenTextures(1, &mTexture); // Générer un identifiant de texture
    glBindTexture(GL_TEXTURE_2D, mTexture); // Lier la texture

    // Paramètres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Axe des abscisses, répétition de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Axe des ordonnées, répétition de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtre de la texture lorsqu'elle est réduite, interpolation linéaire
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtre de la texture lorsqu'elle est agrandie, interpolation linéaire
    
    // GL_TEXTURE_2D : Type de texture, 0 : Niveau de détail, GL_RGBA : Format de stockage, width : Largeur, height : Hauteur, 
    // 0 : Bordure, GL_RGBA : Format de stockage, GL_UNSIGNED_BYTE : Type de données, imageData : Données de l'image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); // Charger l'image dans la texture

    // Générer les mipmaps
    if(generateMipMaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D); // Générer les mipmaps
    }
    stbi_image_free(imageData); // Libérer la mémoire de l'image
    glBindTexture(GL_TEXTURE_2D, 0); // Délier la texture

    return true;
}

// Lier la texture
void Texture2D::bind(GLuint texUnit)
{
    glActiveTexture(GL_TEXTURE0 + texUnit); // Activer la texture
    glBindTexture(GL_TEXTURE_2D, mTexture); // Lier la texture
}

// Delier la texture
void Texture2D::unbind(GLuint texUnit)
{
    glActiveTexture(GL_TEXTURE0 + texUnit); // Activer la texture
    glBindTexture(GL_TEXTURE_2D, 0); // Délier la texture
}
