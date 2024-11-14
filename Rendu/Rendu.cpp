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
#include "Display.hpp"
#include "Models.hpp"
#include "Lights.hpp"

#define GLEW_STATIC

FPSCamera fpsCamera(glm::vec3(0.0f, 1.5f, 10.0f));
ShaderProgram lightingShader;
Display display(fpsCamera);
Models models;
Lights lights(fpsCamera, display);


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

    // Matrices de modèle, vue et projection
    glm::mat4 model = glm::mat4(1.0f);   
    glm::mat4 view = glm::mat4(1.0f);  
    glm::mat4 projection = glm::mat4(1.0f);

    // Position de la vue
    glm::vec3 viewPos;

    // Initialisation d'OpenGL---------------------------------------
    if(!display.initOpenGL())
    {
        std::cerr << "Erreur d'initialisation d'OpenGL" << std::endl;
        return -1;
    }

    // Shaders-------------------------------------------------------
	lightingShader.loadShaders("Shaders/lighting.vert", "Shaders/lighting.frag");
    lightingShader.use();

    // Initialisation des modèles------------------------------------
    models.initializeModels(lightingShader);

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
    while(glfwWindowShouldClose(display.gWindow) == false)
    {
        // Calcul des FPS---------------------------------------
        display.showFPS(display.gWindow);

        // Temps écoulé depuis la dernière image----------------
        currentTime = glfwGetTime(); // Temps écoulé depuis l'initialisation de GLFW
        deltaTime = currentTime - lastTime; // Temps écoulé depuis la dernière image

        // Gestion des evenements-------------------------------
        glfwPollEvents();

        // Mise à jour------------------------------------------
        display.update(deltaTime);

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
        projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)display.gWindowWidth / (float)display.gWindowHeight, 0.1f, 200.0f); 

        // Position de la vue
        glm::vec3 viewPos = fpsCamera.getPosition();
        
        // Utiliser le programme de shader
        lightingShader.use();

        // Uniforms du shader .vert
        lightingShader.setUniform("model", model);
        lightingShader.setUniform("view", view);
        lightingShader.setUniform("projection", projection);
        lightingShader.setUniform("viewPos", viewPos);

		// Configuration de la lumière directionnelle (soleil)
        lights.setDirectionalLight(lightingShader, sunDirection, glm::vec3(1.0f, 1.0f, 0.9f), glm::vec3(0.2f, 0.2f, 0.2f)); // Lumière du soleil jaune

		// Configuration des points de lumière
        lights.setPointLight(lightingShader, 0, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[0], 1.0f, 0.22f, 0.20f); // Lumière verte
        lights.setPointLight(lightingShader, 1, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[1], 1.0f, 0.22f, 0.20f); // Lumière rouge
        lights.setPointLight(lightingShader, 2, glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[2], 1.0f, 0.22f, 0.20f); // Lumière bleue

		// Lampe torche
        lights.spotlightShaders(lightingShader, fpsCamera.getPosition());
		
        // Affichage de la scene
        // Sol
        models.renderModel("sol", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);


        models.renderModel("cabane1", glm::vec3(-5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);


        // Afficher la position de la camera
        std::cout << "Position de la camera : " << viewPos.x << " " << viewPos.y << " " << viewPos.z << std::endl;

        // Echange des buffers----------------------------------
        glfwSwapBuffers(display.gWindow);

        // Mise à jour du temps---------------------------------
        lastTime = currentTime;
    }

    // Nettoyage-----------------------------------------------------
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}