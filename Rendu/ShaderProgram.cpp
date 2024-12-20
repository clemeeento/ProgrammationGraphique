#include "ShaderProgram.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>


ShaderProgram::ShaderProgram()
{
    mHandle = 0;
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
    checkCompileErrors(fs, ShaderType::FRAGMENT); // Verifier les erreurs de compilation
    
    
    // Programme de shader
    mHandle = glCreateProgram(); // Creation du programme de shader
    glAttachShader(mHandle, vs); // Attachement du Vertex Shader
    glAttachShader(mHandle, fs); // Attachement du Fragment Shader
    glLinkProgram(mHandle); // Lien du programme de shader
    checkCompileErrors(mHandle, ShaderType::PROGRAM); // Verifier les erreurs de liaison
    
    glDeleteShader(vs); // Suppression du Vertex Shader
    glDeleteShader(fs); // Suppression du Fragment Shader

    mUniformLocations.clear(); // Initialisation des uniformes

    return true;
}

// Utiliser le programme de shader
void ShaderProgram::use() 
{
    if(mHandle > 0)
    {
        glUseProgram(mHandle); // Utiliser le programme de shader
        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        if (currentProgram != (GLint)mHandle) 
        {
            std::cerr << "Erreur : le programme de shader n'a pas ete lie correctement." << std::endl;
        }
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

// Verifier les erreurs de compilation
void ShaderProgram::checkCompileErrors(GLuint object, ShaderType type) 
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

// Obtenir l'emplacement d'un uniforme
GLint ShaderProgram::getUniformLocation(const GLchar* name)
{
    std::map<string, GLuint>::iterator it = mUniformLocations.find(name); // Recherche de l'uniforme

    if(it == mUniformLocations.end())
    {
        mUniformLocations[name] = glGetUniformLocation(mHandle, name); // Obtenir l'emplacement de l'uniforme
    }

    return mUniformLocations[name];
}

// Definir un uniform de type vec2
void ShaderProgram::setUniform(const GLchar* name, const glm::vec2& v)
{
    GLint localisation = getUniformLocation(name); // Obtenir l'emplacement de l'uniforme
    glUniform2f(localisation, v.x, v.y); // Definir l'uniforme
}

// Definir un uniform de type vec3
void ShaderProgram::setUniform(const GLchar* name, const glm::vec3& v)
{
    GLint localisation = getUniformLocation(name); // Obtenir l'emplacement de l'uniforme
    if (localisation == -1) 
    {
        std::cerr << "Uniform '" << name << "' non trouve dans le shader." << std::endl;
        return;
    }
    glUniform3f(localisation, v.x, v.y, v.z); // Definir l'uniforme
} 

// Definir un uniform de type vec4
void ShaderProgram::setUniform(const GLchar* name, const glm::vec4& v)
{
    GLint localisation = getUniformLocation(name); // Obtenir l'emplacement de l'uniforme
    if (localisation == -1) 
    {
        std::cerr << "Uniform '" << name << "' non trouve dans le shader." << std::endl;
        return;
    }
    glUniform4f(localisation, v.x, v.y, v.z, v.w); // Definir l'uniforme
}

// Definir un uniform de type mat4
void ShaderProgram::setUniform(const GLchar* name, const glm::mat4& m)
{
    GLint localisation = getUniformLocation(name); // Obtenir l'emplacement de l'uniforme
    if (localisation == -1) 
    {
        std::cerr << "Uniform '" << name << "' non trouve dans le shader." << std::endl;
        return;
    }
    glUniformMatrix4fv(localisation, 1, GL_FALSE, glm::value_ptr(m)); // Definir l'uniforme
}

// Definir un uniform de type float
void ShaderProgram::setUniform(const GLchar* name, const GLfloat f)
{
    GLint localisation = getUniformLocation(name); // Obtenir l'emplacement de l'uniforme
    if (localisation == -1) 
    {
        std::cerr << "Uniform '" << name << "' non trouve dans le shader." << std::endl;
        return;
    }
    glUniform1f(localisation, f); // Definir l'uniforme
}

// Definir un uniform de type sampler2D
void ShaderProgram::setUniformSampler(const GLchar* name, const GLint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot); // Activer la texture
    GLint localisation = getUniformLocation(name); // Obtenir l'emplacement de l'uniforme
    if (localisation == -1) 
    {
        std::cerr << "Uniform '" << name << "' non trouve dans le shader." << std::endl;
        return;
    }
    glUniform1i(localisation, slot); // Definir l'uniforme
}

// Obtenir l'identifiant du programme de shader
GLuint ShaderProgram::getProgram() const
{
    return mHandle;
}