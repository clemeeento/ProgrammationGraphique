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
    models.renderModel("soleil", glm::vec3(0.0f, 100.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), model); // Soleil dans le ciel

    // Lumières-----------------------------------------------------
    models.renderModel("lumiere", glm::vec3(11.5f, 3.25f, -11.5f), glm::vec3(0.0f, 0.0f, 0.0f), model); // Lumière 1
    models.renderModel("lumiere", glm::vec3(-11.21f, 2.07f, -11.21f), glm::vec3(0.0f, 0.0f, 0.0f), model); // Lumière 2
}

// Fonction pour mettre à jour la lumière du feu avec des variations de couleur et d'intensité
void updateFireLight(Lights& lights, ShaderProgram& shader, int index, glm::vec3 position) 
{
    // Générer une couleur aléatoire dans les teintes de rouge, orange et jaune
    glm::vec3 colorOptions[] = 
    {
        glm::vec3(1.0f, 0.5f, 0.0f), // Orange
        glm::vec3(1.0f, 0.3f, 0.0f), // Rouge orangé
        glm::vec3(1.0f, 0.8f, 0.0f)  // Jaune
    };
    glm::vec3 chosenColor = colorOptions[rand() % 3];

    // Intensité aléatoire 
    float intensityFactor = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.0f - 0.5f)));

    // Appliquer l'intensité à la couleur choisie
    glm::vec3 ambient = chosenColor * intensityFactor * 0.2f; // Plus faible pour l'ambiant
    glm::vec3 diffuse = chosenColor * intensityFactor;        // Pleine intensité pour le diffuse
    glm::vec3 specular = chosenColor * 0.3f;                  // Faible valeur de speculaire pour adoucir

    // Appeler setPointLight avec des valeurs d'atténuation adaptées pour la proximité
    float constant = 1.0f;
    float linear = 0.14f;
    float exponent = 0.07f;

    lights.setPointLight(shader, index, ambient, diffuse, specular, position, constant, linear, exponent);
}


int main()
{
    // Déclaration des variables-------------------------------------
    // Variables pour le temps
    double currentTime;
    double lastTime;
    double deltaTime;

    // Intensité et direction du soleil
    float intensity;
    glm::vec3 sunPosition = glm::vec3(0.0f, 100.0f, 0.0f);
    glm::vec3 sunDirection = glm::normalize(-sunPosition);

    // Contrôle de la vitesse du cycle jour-nuit
    float cycleSpeed = 0.1f;

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
    glm::vec3 pointLightPos[2] = 
    {
		glm::vec3(11.5f, 3.25f, -11.5f),
		glm::vec3(-11.21f, 2.07f, -11.21f)
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
        
        // Ajuster la luminosité du soleilen fonction du temps--
        intensity = glm::mix(1.0f, 0.0f, nightFactor); // Ajustez les valeurs 0.1f et 1.0f pour le minimum et le maximum d'intensité.

        // Ajustement de la couleur de fond selon la position du soleil
        nightFactor =  glm::clamp<float>((cos(currentTime * cycleSpeed) + 1.0f) / 2.0f, 0.0f, 1.0f);
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
        lights.setSunLight(lightingShader, sunDirection, glm::vec3(1.0f, 1.0f, 0.9f) * intensity, glm::vec3(1.0f, 1.0f, 0.8f) * intensity); // Lumière du soleil jaune

		// Lampe torche
        lights.spotlightShaders(lightingShader, fpsCamera.getPosition());

        // Configuration des points de lumière
        lights.setPointLight(lightingShader, 0, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[0], 1.0f, 0.09f, 0.032f);
        lights.setPointLight(lightingShader, 1, glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), pointLightPos[1], 1.0f, 0.09f, 0.032f);

        // Lumière du feu
        updateFireLight(lights, lightingShader, 2, glm::vec3(2.0f, 0.4f, 3.0f));

        // Affichage de la scene
        renderScene(model);

        // Echange des buffers----------------------------------
        glfwSwapBuffers(display.gWindow);

        // Mise à jour du temps---------------------------------
        lastTime = currentTime;
    }

    // Nettoyage-----------------------------------------------------
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}