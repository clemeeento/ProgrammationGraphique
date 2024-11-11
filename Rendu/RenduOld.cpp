#include <iostream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.hpp"
#include "Texture2D.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"

#define GLEW_STATIC

const char* APP_TITLE = "Rendu OpenGL - Clément Furnon";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gFullScreen = false;
bool gWireframe = false;
int gFlashlightOn = true;
glm::vec4 gClearColor(0.392f, 0.667f, 0.922f, 1.0f);

FPSCamera fpsCamera(glm::vec3(0.0f, 3.5f, 10.0f));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 30.0;
const float MOUSE_SENSITIVITY = 0.1f;
const float GROUND_HEIGHT = 2.0f; // Hauteur du sol


// Fonction de rappel pour la gestion des evenements clavier
void glfw_onKey(GLFWwindow* gWindow, int key, int scancode, int action, int mode)
{
    // Touche ECHAP : fermer la fenêtre
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(gWindow, GL_TRUE);
    }

    if(key == GLFW_KEY_F1 && action == GLFW_PRESS)
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

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		// Activer / Désactiver la lampe torche
		gFlashlightOn = !gFlashlightOn;
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

    // Vérification de la hauteur pour empêcher de traverser le sol
    if(fpsCamera.getPosition().y < GROUND_HEIGHT)
    {
        glm::vec3 pos = fpsCamera.getPosition();
        pos.y = GROUND_HEIGHT;  // Contraindre la hauteur au-dessus du sol
        fpsCamera.setPosition(pos);
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

    // Désactive la V-Sync pour ne pas limiter les FPS
    //glfwSwapInterval(0);

    // Gestion des evenements clavier
    glfwSetKeyCallback(gWindow, glfw_onKey);

    // Gestion de la molette de la souris
    glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

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

int main()
{
    // Initialisation d'OpenGL-------------------------------------
    if(!initOpenGL())
    {
        std::cerr << "Erreur d'initialisation d'OpenGL" << std::endl;
        return -1;
    }

    // Variables pour la couleur de fond jour et nuit
    glm::vec4 dayColor(0.39f, 0.66f, 0.92f, 1.0f); // Bleu clair pour le jour
    glm::vec4 nightColor(0.0f, 0.0f, 0.0f, 1.0f);  // Noir pour la nuit
    float sunAngle = 0.0f;  // Angle initial du "soleil"

    // Creation des shaders----------------------------------------
    ShaderProgram lightingShader;
	lightingShader.loadShaders("Shaders/lighting.vert", "Shaders/lighting.frag"); // Charger les shaders
    
    // Mesh et texture---------------------------------------------
    // Creation du mesh
    const int numModels = 9;
    Mesh mesh[numModels];
    Texture2D texture[numModels];

    // Charger les objets
    mesh[0].loadOBJ("models/floor.obj");
    mesh[1].loadOBJ("Models/gallinera_table_4k.obj");
    mesh[2].loadOBJ("Models/Mur.obj");

	

    // Charger les textures
    texture[0].loadTexture("textures/laminate_floor.jpg", true);
    texture[1].loadTexture("Textures/gallinera_table_diff_4k.jpg", true);
    texture[2].loadTexture("Textures/painted_concrete_02_disp_4k.png", true);


    // Position des modeles----------------------------------------
    glm::vec3 modelPosition[] = 
    {
        glm::vec3(0.0f, 0.0f, 0.0f),	// Sol
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(.0f, 0.0f, 50.0f)

    };
    // Mise à l'échelle des modeles--------------------------------
    glm::vec3 modelScale[] = 
    {
        glm::vec3(10.0f, 1.0f, 10.0f),	// Sol
        glm::vec3(1.0f, 1.0f, 1.0f) ,
        glm::vec3(10.0f, 10.0f, 1.0f)   
    };

    //Position des lumieres---------------------------------------
    glm::vec3 pointLightPos[3] = 
    {
		glm::vec3(-5.0f, 3.8f, 0.0f),
		glm::vec3(0.5f,  3.8f, 0.0f),
		glm::vec3(5.0f,  3.8,  0.0f)
	};


    double lastTime = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW

    // Boucle principale-------------------------------------------
    while(glfwWindowShouldClose(gWindow) == false)
    {
        // Calcul des FPS--------------------------------------
        showFPS(gWindow);

        double currentTime = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW
        double deltaTime = currentTime - lastTime; // Temps écoulé depuis la dernière image

        // Gestion des evenements------------------------------
        glfwPollEvents();

        // Mise à jour-----------------------------------------
        update(deltaTime);

        // Effacement de la fenêtre----------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Mise à jour de l'angle et direction du soleil
        sunAngle = sunAngle + deltaTime * 0.5f;  // Ajustez la vitesse de rotation ici
        glm::vec3 sunDirection = glm::vec3(cos(sunAngle), sin(sunAngle), -0.3f); // Direction changeant au fil du temps

        // Ajustement de la couleur de fond selon la position du soleil
        float nightFactor = glm::clamp((2.0f + sunDirection.y) / 2.0f, 0.0f, 1.0f); // Valeur entre 0 (jour) et 1 (nuit)
        glm::vec4 currentBackgroundColor = glm::mix(dayColor, nightColor, nightFactor);

        // Appliquer la couleur de fond
        glClearColor(currentBackgroundColor.r, currentBackgroundColor.g, currentBackgroundColor.b, currentBackgroundColor.a);

        //Dessin-----------------------------------------------
        // Matrices
        // Initialisation des matrices (identité)
        glm::mat4 model = glm::mat4(1.0f);  
        glm::mat4 view = glm::mat4(1.0f);   
        glm::mat4 projection = glm::mat4(1.0f); 

        // Matrice de vue
        view = fpsCamera.getViewMatrix(); 

        // Matrice de projection
        projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f); 

        // Position de la vue
        glm::vec3 viewPos;
        viewPos.x = fpsCamera.getPosition().x;
        viewPos.y = fpsCamera.getPosition().y;
        viewPos.z = fpsCamera.getPosition().z;

        // Utiliser le programme de shader
        lightingShader.use();

        // Uniforms
        lightingShader.setUniform("model", model);
		lightingShader.setUniform("view", view);
		lightingShader.setUniform("projection", projection);
		lightingShader.setUniform("viewPos", viewPos);

       // Lumiere directionnelle
		lightingShader.setUniform("sunLight.direction", sunDirection); // Direction mise à jour
        lightingShader.setUniform("sunLight.diffuse", glm::vec3(1.0f, 1.0f, 0.9f));  // Lumière du soleil douce
        lightingShader.setUniform("sunLight.specular", glm::vec3(0.2f, 0.2f, 0.2f));

		// Point de lumiere 1
		lightingShader.setUniform("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightingShader.setUniform("pointLights[0].diffuse", glm::vec3(0.0f, 1.0f, 0.0f));	// Lumiere verte
		lightingShader.setUniform("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setUniform("pointLights[0].position", pointLightPos[0]);
		lightingShader.setUniform("pointLights[0].constant", 1.0f);
		lightingShader.setUniform("pointLights[0].linear", 0.22f);
		lightingShader.setUniform("pointLights[0].exponent", 0.20f);

		// Point de lumiere 2
		lightingShader.setUniform("pointLights[1].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightingShader.setUniform("pointLights[1].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));	// Lumiere rouge
		lightingShader.setUniform("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setUniform("pointLights[1].position", pointLightPos[1]);
		lightingShader.setUniform("pointLights[1].constant", 1.0f);
		lightingShader.setUniform("pointLights[1].linear", 0.22f);
		lightingShader.setUniform("pointLights[1].exponent", 0.20f);

		// Point de lumiere 3
		lightingShader.setUniform("pointLights[2].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightingShader.setUniform("pointLights[2].diffuse", glm::vec3(0.0f, 0.0f, 1.0f));	// Lumiere bleue
		lightingShader.setUniform("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setUniform("pointLights[2].position", pointLightPos[2]);
		lightingShader.setUniform("pointLights[2].constant", 1.0f);
		lightingShader.setUniform("pointLights[2].linear", 0.22f);
		lightingShader.setUniform("pointLights[2].exponent", 0.20f);

		// Lampe torche
		glm::vec3 spotlightPos = fpsCamera.getPosition();

		lightingShader.setUniform("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightingShader.setUniform("spotLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		lightingShader.setUniform("spotLight.specular", glm::vec3(2.0f, 2.0f, 2.0f));
		lightingShader.setUniform("spotLight.position", spotlightPos);
		lightingShader.setUniform("spotLight.direction", fpsCamera.getLook());
		lightingShader.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(7.5f)));
		lightingShader.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(15.0f)));
		lightingShader.setUniform("spotLight.constant", 1.0f);
		lightingShader.setUniform("spotLight.linear", 0.0045f);
		lightingShader.setUniform("spotLight.exponent", 0.00075f);
		lightingShader.setUniform("spotLight.on", gFlashlightOn);


        // Dessin de la scene
        for(int i = 0; i < numModels; i = i + 1)
        {
            // Matrice de modèle
            model = glm::mat4(1.0f);
            model = glm::translate(model, modelPosition[i]);
            model = glm::scale(model, modelScale[i]);

            // Uniforms
            lightingShader.setUniform("model", model);
            
            lightingShader.setUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			lightingShader.setUniformSampler("material.diffuseMap", 0);
			lightingShader.setUniform("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
			lightingShader.setUniform("material.shininess", 32.0f);

            // Lier la texture
            texture[i].bind(0);

            // Dessin du mesh
            mesh[i].draw();

            // Delier la texture
            texture[i].unbind(0);
        }

        // Echange des buffers---------------------------------
        glfwSwapBuffers(gWindow);

        lastTime = currentTime; // Mise à jour du temps
    }

    // Nettoyage---------------------------------------------------
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}