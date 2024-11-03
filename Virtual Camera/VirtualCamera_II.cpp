#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.hpp"
#include "Texture2D.hpp"
#include "Camera.hpp"

#define GLEW_STATIC

const char* APP_TITLE = "OpenGL Virtual Camrea II";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gFullScreen = false;
bool gWireframe = false;
const std::string texture1Filename = "Textures/wooden_crate.jpg";
const std::string texture2Filename = "Textures/grid.jpg";

FPSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 5.0f));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0;
const float MOUSE_SENSITIVITY = 0.1f;


// Fonction de rappel pour la gestion des evenements clavier
void glfw_onKey(GLFWwindow* gWindow, int key, int scancode, int action, int mode)
{
    // Touche ECHAP : fermer la fenêtre
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    if(key == GLFW_KEY_Q && action == GLFW_PRESS)
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

// Fonction de rappel pour la gestion de la taille de la fenêtre
void glfw_onFramebufferSize(GLFWwindow* gWindow, int width, int height)
{
    gWindowWidth = width;
    gWindowHeight = height;
    glViewport(0, 0, gWindowWidth, gWindowHeight);
}

// Fonction de rappel pour la gestion de la souris
void glfw_onMouseMove(GLFWwindow* gWindow, double posX, double posY)
{
    // static glm::vec2 lastMousePos = glm::vec2(0, 0);

    // // Rotation de la caméra
    // if(glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT) == 1)
    // {
    //     gYaw = gYaw - ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
    //     gPitch = gPitch + ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
    // }

    // // Zoom
    // if(glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT) == 1)
    // {
    //     float dx = 0.01f * ((float)posX - lastMousePos.x);
    //     float dy = 0.01f * ((float)posY - lastMousePos.y);
    //     gRadius = gRadius + dx - dy;
    // }

    // lastMousePos.x = (float)posX;
    // lastMousePos.y = (float)posY;
}

// Fonction de rappel pour la gestion de la molette de la souris
void glfw_onMouseScroll(GLFWwindow* gWindow, double deltaX, double deltaY)
{
    // Zoom
    double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

    // Limiter le champ de vision
    fov = glm::clamp(fov, 1.0, 120.0);

    // Definir le champ de vision
    fpsCamera.setFOV((float)fov);
}

// Fonction de mise à jour / Deplacement de la camera
void update(double elapsedTime)
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

    // Gestion des evenements de la souris
    glfwSetCursorPosCallback(gWindow, glfw_onMouseMove);

    // Gestion de la molette de la souris
    glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

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
    glClearColor(0.23f, 0.38f, 0.47f, 1.0f); // Couleur de fond
    glViewport(0, 0, gWindowWidth, gWindowHeight); // Taille de la fenêtre
    glEnable(GL_DEPTH_TEST); // Activation du test de profondeur

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
    {   
        // Position des sommets  //Coordonnées de la texture
        // Face avant
		-1.0f,  1.0f,  1.0f,    0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,    1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,    0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,    0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,    1.0f, 0.0f,

		// Face arrière
		-1.0f,  1.0f, -1.0f,    0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,    0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,    1.0f, 0.0f,

		// Face gauche
		-1.0f,  1.0f, -1.0f,    0.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,    1.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,    1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,    0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
		-1.0f, -1.0f,  1.0f,    1.0f, 0.0f,

		// Face droite
		 1.0f,  1.0f,  1.0f,    0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,    0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f,    0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,    1.0f, 0.0f,

		// Face du dessus
		-1.0f,  1.0f, -1.0f,    0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f,    1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,    0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f,    0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,    1.0f, 0.0f,

		// Face du dessous
		-1.0f, -1.0f,  1.0f,    0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,    1.0f, 1.0f,
		-1.0f, -1.0f,  1.0f,    0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
    };

    glm::vec3 cubePose = glm::vec3(0.0f, 0.0f, -5.0f); // Position du cube
    glm::vec3 floorPos = glm::vec3(0.0f, -1.0f, 0.0f); // Position du sol

    // Creation des buffers----------------------------------------
    GLuint vbo, vao; // Vertex Buffer Object, Vertex Array Object

    glGenBuffers(1, &vbo); // Creation du VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Lier le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copie des donnees, GL_STATIC_DRAW : utilisation des donnees statiques

    glGenVertexArrays(1, &vao); // Creation du VAO
    glBindVertexArray(vao); // Lier le VAO

    // Position des sommets
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL); // Position des sommets
    glEnableVertexAttribArray(0); // Activation de l'attribut de sommet

    // Coordonnées de la texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Coordonnées de la texture
    glEnableVertexAttribArray(1); // Activation de l'attribut de texture


    // Creation des shaders----------------------------------------
    ShaderProgram shaderProgram; 
    shaderProgram.loadShaders("Shaders/basic.vert", "Shaders/basic.frag"); // Charger les shaders

    // Chargement des textures-------------------------------------
    Texture2D texture1;
    texture1.loadTexture(texture1Filename, true);

    Texture2D texture2;
    texture2.loadTexture(texture2Filename, true);


    float cubeAngle = 0.0f; // Angle de rotation
    double lastTime = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW

    // Boucle principale-------------------------------------------
    while(glfwWindowShouldClose(gWindow) == false)
    {
        double currentTime = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW
        double deltaTime = currentTime - lastTime; // Temps écoulé depuis la dernière image

        // Calcul des FPS--------------------------------------
        showFPS(gWindow);

        // Gestion des evenements------------------------------
        glfwPollEvents();

        // Mise à jour-----------------------------------------
        update(deltaTime);

        // Effacement de la fenêtre----------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Dessin-----------------------------------------------

        // Dessin du cube
        // Lier la texture
        texture1.bind(0);

        // Matrices
        // Initialisation des matrices (identité)
        glm::mat4 model = glm::mat4(1.0f);  
        glm::mat4 view = glm::mat4(1.0f);   
        glm::mat4 projection = glm::mat4(1.0f); 

        model = glm::translate(model, cubePose); // Translation du cube

        view = fpsCamera.getViewMatrix(); // Matrice de vue

        projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f); // Matrice de projection

        // Utiliser le programme de shader
        shaderProgram.use(); 

        // Uniforms
        shaderProgram.setUniform("model", model); // Matrice de modèle
        shaderProgram.setUniform("view", view); // Matrice de vue
        shaderProgram.setUniform("projection", projection); // Matrice de projection

        glBindVertexArray(vao); // Lier le VAO
        glDrawArrays(GL_TRIANGLES, 0, 36); // Dessin du cube


        // Dessin du sol
        // Lier la texture
        texture2.bind(0);
        
        // Matrices
        model = glm::translate(model, floorPos) * glm::scale(model, glm::vec3(10.0f, 0.01f, 10.0f));

        shaderProgram.setUniform("model", model); // Matrice de modèle

        glDrawArrays(GL_TRIANGLES, 0, 36); // Dessin du sol

        glBindVertexArray(0); // Desactivation du VAO

        // Echange des buffers---------------------------------
        glfwSwapBuffers(gWindow);

        lastTime = currentTime; // Mise à jour du temps
    }

    // Nettoyage---------------------------------------------------
    glDeleteVertexArrays(1, &vao); // Suppression du VAO
    glDeleteBuffers(1, &vbo); // Suppression du VBO
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}