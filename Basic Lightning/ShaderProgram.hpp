#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>

using std::string;

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    enum class ShaderType // Type d'objet
    {
        VERTEX,
        FRAGMENT,
        PROGRAM
    };

    bool loadShaders(const char* vsFilename, const char* fsFilename); // Charger les shaders
    void use(); // Utiliser le programme de shader

    void setUniform(const GLchar* name, const glm::vec2& v); // Definir un uniform de type vec2
    void setUniform(const GLchar* name, const glm::vec3& v); // Definir un uniform de type vec3
    void setUniform(const GLchar* name, const glm::vec4& v); // Definir un uniform de type vec4
    void setUniform(const GLchar* name, const glm::mat4& m); // Definir un uniform de type mat4

    GLuint getProgram() const; // Obtenir l'identifiant du programme de shader

private:
    string fileToString(const string& filename); // Lire un fichier
    void checkCompileErrors(GLuint object, ShaderType type); // Verifier les erreurs de compilation
    GLint getUniformLocation(const GLchar* name); // Obtenir l'emplacement d'un uniforme

    GLuint mHandle; // Identifiant du programme de shader
    std::map<string, GLuint> mUniformLocations; // Tableau des uniformes
};

#endif // SHADER_PROGRAM_HPP