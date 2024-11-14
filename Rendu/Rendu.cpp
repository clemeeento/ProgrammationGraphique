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

// Structure pour stocker la vegetation
struct SceneObject {
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
};

// Vecteur pour stocker la vegetation
std::vector<SceneObject> vegetation;

// Fonction pour initialiser la végétation
void initializeSceneObjects() 
{

    for (int i = -150; i <= 150; i = i + 10) 
    {
        for (int j = -150; j <= 150; j = j + 10) 
        {
            if (i * i + j * j > 800) // Éviter la forêt proche du village
            {
                glm::vec3 position(i + (rand() % 2) + (rand() % 2), 0.0f, j + (rand() % 2) - (rand() % 2)); // Position aléatoire + ou - 2
                glm::vec3 rotation(0.0f, rand() % 360, 0.0f);

                int randomProbability = rand() % 100; // Probabilité (0 à 99)

                if (randomProbability < 80) 
                { 
                    // 85% de probabilité pour les arbres
                    int randomTree = rand() % 3;
                    switch(randomTree) 
                    {
                        case 0: vegetation.push_back({"sapin", position, rotation}); break;
                        case 1: vegetation.push_back({"pin", position, rotation}); break;
                        case 2: vegetation.push_back({"chene", position, rotation}); break;
                    }
                } 
                else
                { 
                    // 15% pour autres types de végétation
                    int randomVegetation = rand() % 3;
                    switch(randomVegetation) 
                    {
                        case 0: vegetation.push_back({"arbuste", position, rotation}); break;
                        case 1: vegetation.push_back({"tronc1", position, rotation}); break;
                        case 2: vegetation.push_back({"pomme_pin", position, rotation}); break;
                    }
                } 
            }
        }
    }
}



// Fontion pour afficher la scene complete
void renderScene(glm::mat4 model)
{
    // Sol de base---------------------------------------------------
    models.renderModel("sol", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);

    // Positionner les bâtiments avec un espacement légèrement réduit (distance minimum de 9 unités)
    models.renderModel("cabane1", glm::vec3(-14.0f, 0.0f, -14.0f), glm::vec3(0.0f, 45.0f, 0.0f), model);
    models.renderModel("cabane2", glm::vec3(14.0f, 0.0f, -14.0f), glm::vec3(0.0f, -45.0f, 0.0f), model);
    models.renderModel("chalet", glm::vec3(-15.0f, 0.0f, 7.0f), glm::vec3(0.0f, 290.0f, 0.0f), model);
    models.renderModel("eglise", glm::vec3(0.0f, 0.0f, 19.0f), glm::vec3(0.0f, 180.0f, 0.0f), model);
    models.renderModel("cabane_rondins", glm::vec3(15.0f, 0.0f, 7.0f), glm::vec3(0.0f, -120.0f, 0.0f), model);
    models.renderModel("cabane_arbre", glm::vec3(0.0f, 0.0f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);

    // Végétation (forêt)--------------------------------------------
    for (const auto& plant : vegetation) 
    {
        models.renderModel(plant.name, plant.position, plant.rotation, model);
    }

    // Animaux-------------------------------------------------------
    // Plusieurs oiseaux dans le ciel à une hauteur de 30
    models.renderModel("oiseau", glm::vec3(-10.0f, 30.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);
    models.renderModel("oiseau", glm::vec3(15.0f, 30.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);
    models.renderModel("oiseau", glm::vec3(-5.0f, 30.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);
    models.renderModel("oiseau", glm::vec3(25.0f, 30.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);
    models.renderModel("oiseau", glm::vec3(-20.0f, 30.0f, 25.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);

    // Ajout de groupes de chaque animal pour plus de vie dans la forêt

    // Blaireaux
    models.renderModel("blaireau", glm::vec3(-25.0f, 0.0f, 10.0f), glm::vec3(0.0f, 15.0f, 0.0f), model);
    models.renderModel("blaireau", glm::vec3(-28.0f, 0.0f, 12.0f), glm::vec3(0.0f, -30.0f, 0.0f), model);

    // Sangliers
    models.renderModel("sanglier", glm::vec3(-18.0f, 0.0f, 25.0f), glm::vec3(0.0f, -30.0f, 0.0f), model);
    models.renderModel("sanglier", glm::vec3(-20.0f, 0.0f, 23.0f), glm::vec3(0.0f, 45.0f, 0.0f), model);

    // Cerfs
    models.renderModel("cerf", glm::vec3(30.0f, 0.0f, -25.0f), glm::vec3(0.0f, 45.0f, 0.0f), model);
    models.renderModel("cerf", glm::vec3(28.0f, 0.0f, -27.0f), glm::vec3(0.0f, 30.0f, 0.0f), model);

    // Elans
    models.renderModel("elan", glm::vec3(-35.0f, 0.0f, 20.0f), glm::vec3(0.0f, -20.0f, 0.0f), model);
    models.renderModel("elan", glm::vec3(-33.0f, 0.0f, 22.0f), glm::vec3(0.0f, 10.0f, 0.0f), model);

    // Faons proches des élans
    models.renderModel("faon", glm::vec3(-36.0f, 0.0f, 18.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);
    models.renderModel("faon", glm::vec3(-32.0f, 0.0f, 24.0f), glm::vec3(0.0f, 0.0f, 0.0f), model);

    // Furets
    models.renderModel("furet", glm::vec3(20.0f, 0.0f, 30.0f), glm::vec3(0.0f, 90.0f, 0.0f), model);
    models.renderModel("furet", glm::vec3(18.0f, 0.0f, 32.0f), glm::vec3(0.0f, -45.0f, 0.0f), model);

    // Renards
    models.renderModel("renard", glm::vec3(-20.0f, 0.0f, -30.0f), glm::vec3(0.0f, -45.0f, 0.0f), model);
    models.renderModel("renard", glm::vec3(-22.0f, 0.0f, -28.0f), glm::vec3(0.0f, 25.0f, 0.0f), model);

    // Lapins dispersés en petits groupes
    models.renderModel("lapin", glm::vec3(22.0f, 0.0f, -28.0f), glm::vec3(0.0f, 15.0f, 0.0f), model);
    models.renderModel("lapin", glm::vec3(24.0f, 0.0f, -30.0f), glm::vec3(0.0f, -10.0f, 0.0f), model);
    models.renderModel("lapin", glm::vec3(25.0f, 0.0f, -26.0f), glm::vec3(0.0f, 5.0f, 0.0f), model);
    models.renderModel("lapin", glm::vec3(-15.0f, 0.0f, 32.0f), glm::vec3(0.0f, -10.0f, 0.0f), model);
    models.renderModel("lapin", glm::vec3(-14.0f, 0.0f, 34.0f), glm::vec3(0.0f, 20.0f, 0.0f), model);

    // Loups éloignés
    models.renderModel("loup", glm::vec3(-40.0f, 0.0f, 40.0f), glm::vec3(0.0f, 180.0f, 0.0f), model);
    models.renderModel("loup", glm::vec3(35.0f, 0.0f, 25.0f), glm::vec3(0.0f, -45.0f, 0.0f), model);


    // Accessoires---------------------------------------------------
    models.renderModel("voiture", glm::vec3(-17.0f, 0.0f, -11.0f), glm::vec3(0.0f, 45.0f, 0.0f), model); // Cabane1
    models.renderModel("voiture", glm::vec3(18.0f, 0.0f, -12.0f), glm::vec3(0.0f, -45.0f, 0.0f), model); // Cabane2
    models.renderModel("voiture", glm::vec3(-13.0f, 0.0f, 12.0f), glm::vec3(0.0f, 110.0f, 0.0f), model); // Chalet
    models.renderModel("voiture", glm::vec3(20.0f, 0.0f, 3.0f), glm::vec3(0.0f, -120.0f, 0.0f), model); // Cabane en rondins
    models.renderModel("feu_camp", glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), model); // Feu de camp au centre
    models.renderModel("balancoire", glm::vec3(-3.0f, 0.0f, 2.0f), glm::vec3(0.0f, 30.0f, 0.0f), model); // Balançoire près du centre
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
    
    // Initialisation de la végétation--------------------------------
    initializeSceneObjects();

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
        renderScene(model);

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