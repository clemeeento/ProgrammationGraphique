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
int gWindowWidth = 1920;
int gWindowHeight = 1080;
GLFWwindow* gWindow = NULL;
bool gFullScreen = true;
bool gWireframe = false;
int gFlashlightOn = true;
glm::vec4 gClearColor(0.392f, 0.667f, 0.922f, 1.0f);

FPSCamera fpsCamera(glm::vec3(0.0f, 3.5f, 10.0f));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0;
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

// Fonction pour définir les matrices de shader
void setShaderMatrices(ShaderProgram& shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& viewPos) 
{
    shader.setUniform("model", model);
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);
    shader.setUniform("viewPos", viewPos);
}

// Fonction pour la lampe torche
void spotlightShaders(ShaderProgram lightingShader, glm::vec3 spotlightPos)
{
    lightingShader.setUniform("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    lightingShader.setUniform("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setUniform("spotLight.specular", glm::vec3(2.0f, 2.0f, 2.0f));
    lightingShader.setUniform("spotLight.position", spotlightPos);
    lightingShader.setUniform("spotLight.direction", fpsCamera.getLook());
    lightingShader.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(7.5f)));
    lightingShader.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(15.0f)));
    lightingShader.setUniform("spotLight.constant", 1.0f);
    lightingShader.setUniform("spotLight.linear", 0.0045f);
    lightingShader.setUniform("spotLight.exponent", 0.00075f);
    lightingShader.setUniform("spotLight.on", gFlashlightOn);
}

// Fonction pour les points de lumière
void setPointLight(ShaderProgram& shader, int index, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float exponent) 
{
    // Convertir l'index en chaîne pour accéder aux attributs uniformes des points de lumière
    std::string base = "pointLights[" + std::to_string(index) + "].";

    shader.setUniform((base + "ambient").c_str(), ambient);
    shader.setUniform((base + "diffuse").c_str(), diffuse);
    shader.setUniform((base + "specular").c_str(), specular);
    shader.setUniform((base + "position").c_str(), position);
    shader.setUniform((base + "constant").c_str(), constant);
    shader.setUniform((base + "linear").c_str(), linear);
    shader.setUniform((base + "exponent").c_str(), exponent);
}

// Fonction pour la lumière directionnelle
void setDirectionalLight(ShaderProgram& shader, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular) 
{
    shader.setUniform("sunLight.direction", direction);
    shader.setUniform("sunLight.diffuse", diffuse);
    shader.setUniform("sunLight.specular", specular);
}

// Fonction pour afficher un modèle
void renderModel(ShaderProgram &shader, Mesh &mesh, Texture2D &texture, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::mat4 model)
{
    // Matrice de modèle
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    // Uniforms
    shader.setUniform("model", model);
    shader.setUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    shader.setUniformSampler("material.diffuseMap", 0);
    shader.setUniform("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
    shader.setUniform("material.shininess", 32.0f);

    // Lier la texture
    texture.bind(0);

    // Dessin du mesh
    mesh.draw();

    // Delier la texture
    texture.unbind(0);
}


int main()
{
    // Déclaration des variables-------------------------------------
    // Variables pour le temps
    double currentTime;
    double lastTime;
    double deltaTime;

    // Variables pour la position et la direction du soleil
    float sunAngle = 10.0f;  // Angle initial du soleil
    glm::vec3 sunDirection;

    // Variables pour la couleur de fond jour et nuit
    glm::vec4 dayColor(0.39f, 0.66f, 0.92f, 1.0f); // Bleu clair pour le jour
    glm::vec4 nightColor(0.0f, 0.0f, 0.0f, 1.0f);  // Noir pour la nuit
    float nightFactor; // Facteur de transition entre le jour et la nuit
    glm::vec4 currentBackgroundColor; // Couleur de fond actuelle

    // Shaders
    ShaderProgram lightingShader;

    // Modèles et textures
    const int numModels = 6;
    Mesh mesh[numModels];
    Texture2D texture[numModels];

    // Matrices de modèle, vue et projection
    glm::mat4 model = glm::mat4(1.0f);   
    glm::mat4 view = glm::mat4(1.0f);  
    glm::mat4 projection = glm::mat4(1.0f);

    // Position de la vue
    glm::vec3 viewPos;

    // Initialisation d'OpenGL---------------------------------------
    if(!initOpenGL())
    {
        std::cerr << "Erreur d'initialisation d'OpenGL" << std::endl;
        return -1;
    }

    // Shaders-------------------------------------------------------
	lightingShader.loadShaders("Shaders/lighting.vert", "Shaders/lighting.frag");
    
    // Modèles et texture--------------------------------------------
    // Charger les objets
    mesh[0].loadOBJ("Models/Floor.obj"); // Sol
    mesh[1].loadOBJ("Models/Cabin_00.obj");
    
    // Charger les textures
    texture[0].loadTexture("Textures/Floor.jpg", true); // Sol
    texture[1].loadTexture("Textures/Cabin_00.png", true); 
   
    // Position des modeles
    glm::vec3 modelPosition[numModels] = 
    {
        glm::vec3(0.0f, 0.0f, 0.0f), // Sol
        glm::vec3(0.0f, 0.0f, 0.0f)
    };

    // Rotation des modeles
    glm::vec3 modelRotation[numModels] = 
    {
        glm::vec3(0.0f, 0.0f, 0.0f), // Sol
        glm::vec3(0.0f, 0.0f, 0.0f)
    };

    // Mise à l'échelle des modeles
    glm::vec3 modelScale[numModels] = 
    {
        glm::vec3(10.0f, 1.0f, 10.0f),	// Sol
        glm::vec3(0.01f, 0.01f, 0.01f)
    };

    //Position des lumieres------------------------------------------
    glm::vec3 pointLightPos[3] = 
    {
		glm::vec3(-5.0f, 3.8f, 0.0f),
		glm::vec3(0.5f,  3.8f, 0.0f),
		glm::vec3(5.0f,  3.8,  0.0f)
	};
    
    // Temps écoulé depuis l'initialisation de GLFW------------------
    lastTime = glfwGetTime(); 

    // Boucle principale---------------------------------------------
    while(glfwWindowShouldClose(gWindow) == false)
    {
        // Calcul des FPS---------------------------------------
        showFPS(gWindow);

        // Temps écoulé depuis la dernière image----------------
        currentTime = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW
        deltaTime = currentTime - lastTime; // Temps écoulé depuis la dernière image

        // Gestion des evenements-------------------------------
        glfwPollEvents();

        // Mise à jour------------------------------------------
        update(deltaTime);

        // Effacement de la fenêtre-----------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Mise à jour de l'angle et direction du soleil--------
        sunAngle = sunAngle + deltaTime * 0.01f;  // Ajustez la vitesse de rotation ici
        sunDirection = glm::vec3(cos(sunAngle), sin(sunAngle), -0.3f); // Direction changeant au fil du temps

        // Ajustement de la couleur de fond selon la position du soleil
        nightFactor = glm::clamp((2.0f + sunDirection.y) / 2.0f, 0.0f, 1.0f); // Valeur entre 0 (jour) et 1 (nuit)
        currentBackgroundColor = glm::mix(dayColor, nightColor, nightFactor); // Mélange de couleurs
        glClearColor(currentBackgroundColor.r, currentBackgroundColor.g, currentBackgroundColor.b, currentBackgroundColor.a); // Couleur de fond

        //Dessin------------------------------------------------
        // Initialisation des matrices (identité)
        model = glm::mat4(1.0f);  
        view = glm::mat4(1.0f);   
        projection = glm::mat4(1.0f); 

        // Matrice de vue
        view = fpsCamera.getViewMatrix(); 

        // Matrice de projection
        projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f); 

        // Position de la vue
        glm::vec3 viewPos = fpsCamera.getPosition();

        // Utiliser le programme de shader
        lightingShader.use();

        // Uniforms du shader .vert
        setShaderMatrices(lightingShader, model, view, projection, viewPos);

		// Configuration de la lumière directionnelle (soleil)
        setDirectionalLight(lightingShader, sunDirection, glm::vec3(1.0f, 1.0f, 0.9f), glm::vec3(0.2f, 0.2f, 0.2f)); // Lumière du soleil jaune

		// Configuration des points de lumière
        setPointLight(lightingShader, 0, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[0], 1.0f, 0.22f, 0.20f); // Lumière verte
        setPointLight(lightingShader, 1, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[1], 1.0f, 0.22f, 0.20f); // Lumière rouge
        setPointLight(lightingShader, 2, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[2], 1.0f, 0.22f, 0.20f); // Lumière bleue

		// Lampe torche
        spotlightShaders(lightingShader, fpsCamera.getPosition());
		
        // Affichage de la scene
        for(int i = 0; i < numModels; i = i + 1)
        {
            renderModel(lightingShader, mesh[i], texture[i], modelPosition[i], modelScale[i], modelRotation[i], model);
        }

        // Echange des buffers----------------------------------
        glfwSwapBuffers(gWindow);

        // Mise à jour du temps---------------------------------
        lastTime = currentTime;
    }

    // Nettoyage-----------------------------------------------------
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}