#include "ShaderProgram.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


ShaderProgram::ShaderProgram() : mHandle(0) // Constructeur
{
    
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(mHandle);
}

// Charger les shaders
bool ShaderProgram::loadShaders(const char* vsFilename, const char* fsFilename) 
{
    string vsString = fileToString(vsFilename); // Lire le fichier du Vertex Shader
    string fsString = fileToString(fsFilename); // Lire le fichier du Fragment Shader

    const GLchar* vsSourcePtr = vsString.c_str(); // Convertir en chaine de caractères
    const GLchar* fsSourcePtr = fsString.c_str(); // Convertir en chaine de caractères

    GLuint vs = glCreateShader(GL_VERTEX_SHADER); // Creation du Vertex Shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); // Creation du Fragment Shader

    glShaderSource(vs, 1, &vsSourcePtr, NULL); // Specification du code source, 1 : nombre de chaînes
    glShaderSource(fs, 1, &fsSourcePtr, NULL); // Specification du code source, 1 : nombre de chaînes
    

    glCompileShader(vs); // Compilation du shader
    checkCompileErrors(vs, ShaderType::VERTEX); // Verifier les erreurs de compilation
    
    glCompileShader(fs); // Compilation du shader
    checkCompileErrors(vs, ShaderType::FRAGMENT); // Verifier les erreurs de compilation
    
    
    // Programme de shader
    mHandle = glCreateProgram(); // Creation du programme de shader
    glAttachShader(mHandle, vs); // Attachement du Vertex Shader
    glAttachShader(mHandle, fs); // Attachement du Fragment Shader
    glLinkProgram(mHandle); // Lien du programme de shader
    
    glDeleteShader(vs); // Suppression du Vertex Shader
    glDeleteShader(fs); // Suppression du Fragment Shader

    return true;
}

// Utiliser le programme de shader
void ShaderProgram::use() 
{
    if(mHandle > 0)
    {
        glUseProgram(mHandle); // Utiliser le programme de shader
    }
}

// Lire un fichier
string ShaderProgram::fileToString(const string& filename)
{
    std::stringstream ss;
    std::ifstream file;

    try
    {
        file.open(filename, std::ios::in);

        if(!file.fail())
        {
            ss << file.rdbuf();
        }

        file.close();
    }
    catch(std::exception ex)
    {
        std::cerr << "Erreur lors de la lecture du fichier : " << filename << std::endl;
    }

    return ss.str();
}

void ShaderProgram::checkCompileErrors(GLuint object, ShaderType type) // Verifier les erreurs de compilation
{
    int status = 0;

    if(type == ShaderType::PROGRAM)
    {
        glGetProgramiv(object, GL_LINK_STATUS, &status); // Obtenir le status de la liaison

        if(status == GL_FALSE)
        {
            GLint length = 0;
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &length);

            string errorLog(length, ' ');
            glGetProgramInfoLog(object, length, &length, &errorLog[0]); // Obtenir le message d'erreur

            std::cerr << "Erreur de liaison du programme de shader : " << errorLog << std::endl;
        }
    }

    else // VERTEX ou FRAGMENT
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &status); // Obtenir le status de la compilation

        if(status == GL_FALSE)
        {
            GLint length = 0;
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &length);

            string errorLog(length, ' ');
            glGetShaderInfoLog(object, length, &length, &errorLog[0]); // Obtenir le message d'erreur

            std::cerr << "Erreur de compilation du shader : " << errorLog << std::endl;
        }
    }
}