#include "Display.hpp"


// Initialisation des membres statiques
int Display::gWindowWidth = 1920;
int Display::gWindowHeight = 1080;
bool Display::gFlashlightOn = true;
bool Display::gFullScreen = true;
bool Display::VSync = true;

// Constructeur
Display::Display(FPSCamera &camera) : fpsCamera(camera)
{
}

// Fonction d'initialisation d'OpenGL
bool Display::initOpenGL()
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

    // Active/Désactive la V-Sync pour ne pas limiter les FPS
    glfwSwapInterval(VSync);

    // Gestion des evenements clavier
    glfwSetKeyCallback(gWindow, glfw_onKey);

    // Gestion de la taille de la fenêtre
    glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);

    // Cacher et saisir le curseur, mouvement illimité
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

    // Initialisation de GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Erreur d'initialisation de GLEW" << std::endl;
        return false;
    }

    // Configuration de l'affichage
    glClearColor(gClearColor.r, gClearColor.g, gClearColor.b, gClearColor.a);
    glViewport(0, 0, gWindowWidth, gWindowHeight); // Taille de la fenêtre
    glEnable(GL_DEPTH_TEST); // Activation du test de profondeur

    return true;
}

// Fonction de rappel pour la gestion de la taille de la fenêtre
void Display::glfw_onFramebufferSize(GLFWwindow* gWindow, int width, int height)
{
    gWindowWidth = width;
    gWindowHeight = height;
    glViewport(0, 0, gWindowWidth, gWindowHeight);
}

// Fonction pour afficher les FPS
void Display::showFPS(GLFWwindow* gWindow)
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

        // Affichage des informations
        std::ostringstream outs;
        outs.precision(3); // 3 chiffres après la virgule
        outs << std::fixed
             << APP_TITLE << "   "
             << "FPS: " << fps; 
        glfwSetWindowTitle(gWindow, outs.str().c_str());

        frameCount = 0;
    }

    frameCount = frameCount + 1;
}

// Fonction de mise à jour / Deplacement de la camera
void Display::update(double elapsedTime)
{   
    // Orientation de la caméra
    double mouseX, mouseY;

    // Récupérer la position de la souris
    glfwGetCursorPos(gWindow, &mouseX, &mouseY);

    // Rotation de la caméra de la différence de position de la souris depuis le centre de la fenêtre. Multiplier par la sensibilité de la souris
    fpsCamera.rotate((float)(gWindowWidth / 2 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2 - mouseY) * MOUSE_SENSITIVITY);

    // Réinitialiser la position de la souris au centre de la fenêtre
    glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

    // Deplacement de la camera
    // Avant / Arrière
    if(glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
    }
    else if(glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());
    }

    // Gauche / Droite
    if(glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
    }
    else if(glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());
    }

    // Haut / Bas
    if(glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
    }
    else if(glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
    }

    // Vérification de la hauteur pour empêcher de traverser le sol
    if(fpsCamera.getPosition().y < GROUND_HEIGHT)
    {
        glm::vec3 pos = fpsCamera.getPosition();
        pos.y = GROUND_HEIGHT;  // Contraindre la hauteur au-dessus du sol
        fpsCamera.setPosition(pos);
    }
}

// Fonction de rappel pour la gestion des evenements clavier
void Display::glfw_onKey(GLFWwindow* gWindow, int key, int scancode, int action, int mode)
{
    // Touche ECHAP : fermer la fenêtre
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    // Touche F1 : activer/désactiver la V-Sync
    if(key == GLFW_KEY_F1 && action == GLFW_PRESS)
    {
        VSync = !VSync;
        glfwSwapInterval(VSync);
    }

    // Touche F2 : basculer en mode plein écran
    if(key == GLFW_KEY_F2 && action == GLFW_PRESS)
    {
        gFullScreen = !gFullScreen;

        if(gFullScreen)
        {
            GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
            if(pVmode != NULL)
            {
                glfwSetWindowMonitor(gWindow, pMonitor, 0, 0, pVmode->width, pVmode->height, GLFW_DONT_CARE);
            }
        }
        else
        {
            glfwSetWindowMonitor(gWindow, NULL, 100, 100, gWindowWidth, gWindowHeight, GLFW_DONT_CARE);
        }
    }

    // Touche F : activer/désactiver la lampe torche
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		// Activer / Désactiver la lampe torche
		gFlashlightOn = !gFlashlightOn;
	}
}
