#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLEW_STATIC

const char* APP_TITLE = "OpenGL Window and Input";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
bool gFullScreen = false;

// Fonction de rappel pour la gestion des événements clavier
void glfw_onKey(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    // Touche ECHAP : fermer la fenêtre
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}

// Fonction pour afficher les FPS
void showFPS(GLFWwindow* pWindow)
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
        glfwSetWindowTitle(pWindow, outs.str().c_str());

        frameCount = 0;
    }

    frameCount = frameCount + 1;
}

int main()
{
    // Initialisation de GLFW
    if(!glfwInit())
    {
        std::cout << "Erreur d'initialisation de GLFW" << std::endl;
        return -1;
    }

    // Configuration de GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Création de la fenêtre
    GLFWwindow* pWindow = NULL;

    if(gFullScreen) 
    {
        GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
        if(pVmode != NULL)
        {
            pWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, NULL);
        }
    }
    else
    {
        pWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
    }

    if(pWindow == NULL)
    {
        std::cout << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Rendre le contexte OpenGL courant
    glfwMakeContextCurrent(pWindow);

    // Gestion des événements clavier
    glfwSetKeyCallback(pWindow, glfw_onKey);

    // Initialisation de GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
        return -1;
    }

    // Boucle principale
    while(glfwWindowShouldClose(pWindow) == false)
    {
        // Calcul des FPS
        showFPS(pWindow);

        // Gestion des événements
        glfwPollEvents();

        glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Echange des buffers
        glfwSwapBuffers(pWindow);
    }

    glfwTerminate();

    return 0;
}