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

// Fonction de rappel pour la gestion des événements clavier
void glfw_onKey(GLFWwindow* gWindow, int key, int scancode, int action, int mode)
{
    // Touche ECHAP : fermer la fenêtre
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }
}

// Fonction pour afficher les FPS
void showFPS(GLFWwindow* gWindow)
{
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW

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

    // Création de la fenêtre
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
        std::cout << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return false;
    }

    // Rendre le contexte OpenGL courant
    glfwMakeContextCurrent(gWindow);

    // Gestion des événements clavier
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
    if(!initOpenGL())
    {
        std::cerr << "Erreur d'initialisation d'OpenGL" << std::endl;
        return -1;
    }

    GLfloat vertices[] = {
        0.0f, 0.5f, 0.0f, // Sommet haut
        0.5f, -0.5f, 0.0f,  // Sommet droit
        -0.5f, -0.5f, 0.0f  // Sommet gauche
    };

    GLuint vbo;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // Boucle principale
    while(glfwWindowShouldClose(gWindow) == false)
    {
        // Calcul des FPS
        showFPS(gWindow);

        // Gestion des événements
        glfwPollEvents();

        // Effacement de la fenêtre
        glClear(GL_COLOR_BUFFER_BIT);

        // Echange des buffers
        glfwSwapBuffers(gWindow);
    }

    glfwTerminate();

    return 0;
}