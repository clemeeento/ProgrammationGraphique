#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <string>
#include <GL/glew.h>

using std::string;

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    enum class ShaderType // Type de shader
    {
        VERTEX,
        FRAGMENT,
        PROGRAM
    };

    bool loadShaders(const char* vsFilename, const char* fsFilename); // Charger les shaders
    void use(); // Utiliser le programme de shader

private:
    string fileToString(const string& filename); // Lire un fichier
    void checkCompileErrors(GLuint object, ShaderType type); // Verifier les erreurs de compilation

    GLuint mHandle; // Identifiant du programme de shader

};

#endif // SHADER_PROGRAM_HPP