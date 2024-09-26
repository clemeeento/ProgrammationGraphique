#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLEW_STATIC

const char* APP_TITLE = "OpenGL First Triangle";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;
bool gFullScreen = false;
bool gWireframe = false;

// Shaders
const GLchar* vertexShaderSource = // Vertex Shader
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;" // Position du sommet
    "void main()"
    "{"
        "gl_Position = vec4(position.x, position.y, position.z, 1.0);" // Position du sommet
    "}"; 

const GLchar* fragmentShaderSource = // Fragment Shader
    "#version 330 core\n"
    "out vec4 fragColor;" // Couleur du fragment
    "void main()" 
    "{ fragColor = vec4(0.35f, 0.96f, 0.3f, 1.0f);}"; 

// Fonction de rappel pour la gestion des evenements clavier
void glfw_onKey(GLFWwindow* gWindow, int key, int scancode, int action, int mode)
{
    // Touche ECHAP : fermer la fenêtre
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    if(key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        gWireframe = !gWireframe;
        if(gWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Mode vide
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Mode plein
        }
    }
}

// Fonction pour afficher les FPS
void showFPS(GLFWwindow* gWindow)
{
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime(); // Temps ecoule depuis l'initialisation de GLFW

    elapsedSeconds = currentSeconds - previousSeconds;

    // Limite de 4 par seconde
    if(elapsedSeconds > 0.25)
    {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapsedSeconds;
        double msPerFrame = 1000.0 / fps;

        // Affichage des informations
        std::ostringstream outs;
        outs.precision(3); // 3 chiffres après la virgule
        outs << std::fixed
             << APP_TITLE << "   "
             << "FPS: " << fps << "    "
             << "Frame Time: " << msPerFrame << " (ms)";
        glfwSetWindowTitle(gWindow, outs.str().c_str());

        frameCount = 0;
    }

    frameCount = frameCount + 1;
}

// Fonction d'initialisation d'OpenGL
bool initOpenGL()
{
    // Initialisation de GLFW
    if(!glfwInit())
    {
        std::cout << "Erreur d'initialisation de GLFW" << std::endl;
        return false;
    }

    // Configuration de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Creation de la fenêtre
    if(gFullScreen) 
    {
        GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
        if(pVmode != NULL)
        {
            gWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, NULL);
        }
    }
    else
    {
        gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
    }

    if(gWindow == NULL)
    {
        std::cout << "Erreur lors de la creation de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return false;
    }

    // Rendre le contexte OpenGL courant
    glfwMakeContextCurrent(gWindow);

    // Gestion des evenements clavier
    glfwSetKeyCallback(gWindow, glfw_onKey);

    // Initialisation de GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
        return false;
    }

    // Configuration de l'affichage
    glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

    return true;
}

int main()
{
    // Initialisation d'OpenGL-------------------------------------
    if(!initOpenGL())
    {
        std::cerr << "Erreur d'initialisation d'OpenGL" << std::endl;
        return -1;
    }

    // Sommets du triangle-----------------------------------------
    GLfloat vertices[] = 
    {   // Position des sommets 
        //Triangle 0 & 1
        -0.5f,  0.5f, 0.0f, 
         0.5f,  0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f, 
    };

    GLuint indices[] = 
    {   // Indices des sommets
        0, 1, 2, // Triangle 0
        0, 2, 3, // Triangle 1
    };

    // Equivalent à (avec ibo)
    // GLfloat vertices[] = 
    // {   // Position des sommets 
    //     //Triangle 0
    //     -0.5f,  0.5f, 0.0f, 
    //      0.5f,  0.5f, 0.0f, 
    //      0.5f, -0.5f, 0.0f,

    //     //Triangle 1
    //     -0.5f,  0.5f, 0.0f, 
    //      0.5f, -0.5f, 0.0f,
    //     -0.5f, -0.5f, 0.0f
    // };

    // Creation des buffers----------------------------------------
    GLuint vbo, ibo, vao; // Vertex Buffer Object, Vertex Array Object, Index Buffer Object

    glGenBuffers(1, &vbo); // Creation du VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Lier le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copie des donnees, GL_STATIC_DRAW : utilisation des donnees statiques

    glGenVertexArrays(1, &vao); // Creation du VAO
    glBindVertexArray(vao); // Lier le VAO

    // Position des sommets
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // Position des sommets, 0 : attribut de sommet, 3 : nombre de composantes, 0 : decalage, NULL : pas de decalage
    glEnableVertexAttribArray(0); // Activation de l'attribut de sommet

    glGenBuffers(1, &ibo); // Creation de l'IBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Lier l'IBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Copie des donnees, GL_STATIC_DRAW : utilisation des donnees statiques


    // Creation des shaders----------------------------------------
    GLint result;
    GLchar infoLog[512];
    // Vertex Shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER); // Creation du Vertex Shader
    glShaderSource(vs, 1, &vertexShaderSource, NULL); // Specification du code source, 1 : nombre de chaînes
    glCompileShader(vs); // Compilation du shader

    glGetShaderiv(vs, GL_COMPILE_STATUS, &result); // Recuperation du statut de compilation
    if(!result)
    {
        glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog); // Recuperation des erreurs
        std::cout << "Erreur de compilation du Vertex Shader : " << infoLog << std::endl;
    }
    
    // Fragment Shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); // Creation du Fragment Shader
    glShaderSource(fs, 1, &fragmentShaderSource, NULL); // Specification du code source, 1 : nombre de chaînes
    glCompileShader(fs); // Compilation du shader

    glGetShaderiv(fs, GL_COMPILE_STATUS, &result); // Recuperation du statut de compilation
    if(!result)
    {
        glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog); // Recuperation des erreurs
        std::cout << "Erreur de compilation du Fragment Shader : " << infoLog << std::endl;
    }
    
    // Programme de shader
    GLuint shaderProgram = glCreateProgram(); // Creation du programme de shader
    glAttachShader(shaderProgram, vs); // Attachement du Vertex Shader
    glAttachShader(shaderProgram, fs); // Attachement du Fragment Shader
    glLinkProgram(shaderProgram); // Lien du programme

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result); // Recuperation du statut de lien
    if(!result)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog); // Recuperation des erreurs
        std::cout << "Erreur de lien du programme de shader : " << infoLog << std::endl;
    }
    
    glDeleteShader(vs); // Suppression du Vertex Shader
    glDeleteShader(fs); // Suppression du Fragment Shader

    // Boucle principale-------------------------------------------
    while(glfwWindowShouldClose(gWindow) == false)
    {
        // Calcul des FPS
        showFPS(gWindow);

        // Gestion des evenements
        glfwPollEvents();

        // Effacement de la fenêtre
        glClear(GL_COLOR_BUFFER_BIT);

        // Dessin
        glUseProgram(shaderProgram); // Activation du programme de shader
        glBindVertexArray(vao); // Lier le VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Dessin des triangles, 6 : nombre d'indices, 0 : decalage
        glBindVertexArray(0); // Desactivation du VAO

        // Echange des buffers
        glfwSwapBuffers(gWindow);
    }

    // Nettoyage---------------------------------------------------
    glDeleteProgram(shaderProgram); // Suppression du programme de shader
    glDeleteVertexArrays(1, &vao); // Suppression du VAO
    glDeleteBuffers(1, &vbo); // Suppression du VBO
    glDeleteBuffers(1, &ibo); // Suppression de l'IBO
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}