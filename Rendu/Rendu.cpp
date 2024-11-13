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

#define GLEW_STATIC

FPSCamera fpsCamera(glm::vec3(0.0f, 3.5f, 10.0f));
Display display(fpsCamera);


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
    lightingShader.setUniform("spotLight.on", display.gFlashlightOn);
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
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
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
    const int numModels = 38;
    Mesh mesh[numModels];
    Texture2D texture[numModels];

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
    
    // Modèles et texture--------------------------------------------
    // Charger les objets
    // Sol
    mesh[0].loadOBJ("Models/Floor.obj"); // Sol
    // Maisons
    mesh[1].loadOBJ("Models/Cabin_00.obj"); // Cabane 1
    mesh[2].loadOBJ("Models/Cabin_01.obj"); // Cabane 2
    mesh[3].loadOBJ("Models/Chalet.obj"); // Chalet
    mesh[4].loadOBJ("Models/Church.obj"); // Eglise
    mesh[5].loadOBJ("Models/LogCabin.obj"); // Cabane en rondins
    mesh[6].loadOBJ("Models/TreeHouse.obj"); // Cabane dans l'arbre
    // Végétation
    mesh[7].loadOBJ("Models/Bush.obj"); // Buisson
    mesh[8].loadOBJ("Models/FirTree.obj"); // Sapin
    mesh[9].loadOBJ("Models/Forest.obj"); // Forêt
    mesh[10].loadOBJ("Models/Grass.obj"); // Herbe
    mesh[11].loadOBJ("Models/OakTree.obj"); // Chêne
    mesh[12].loadOBJ("Models/PineTree.obj"); // Pin
    mesh[13].loadOBJ("Models/Pinecone.obj"); // Pomme de pin
    mesh[14].loadOBJ("Models/Pond.obj"); // Etang
    mesh[15].loadOBJ("Models/Rock.obj"); // Roche 
    mesh[16].loadOBJ("Models/Tree.obj"); // Arbre
    mesh[17].loadOBJ("Models/Trunk_00.obj"); // Tronc 1
    mesh[18].loadOBJ("Models/Trunk_01.obj"); // Tronc 2
    mesh[19].loadOBJ("Models/Shrub.obj"); // Arbuste
    // Animaux
    mesh[20].loadOBJ("Models/Badger.obj"); // Blaireau
    mesh[21].loadOBJ("Models/Bear.obj"); // Ours
    mesh[22].loadOBJ("Models/Bird.obj"); // Oiseau
    mesh[23].loadOBJ("Models/Boar.obj"); // Sanglier
    mesh[24].loadOBJ("Models/Deer_00.obj"); // Cerf 1
    mesh[25].loadOBJ("Models/Deer_01.obj"); // Cerf 2
    mesh[26].loadOBJ("Models/Elk.obj"); // Elan
    mesh[27].loadOBJ("Models/Fawn.obj"); // Faon
    mesh[28].loadOBJ("Models/Ferret.obj"); // Furet
    mesh[29].loadOBJ("Models/Fox.obj"); // Renard
    mesh[30].loadOBJ("Models/Rabbit.obj"); // Lapin
    mesh[31].loadOBJ("Models/Wolf.obj"); // Loup
    // Accessoires
    mesh[32].loadOBJ("Models/CampFire.obj"); // Feu de camp
    mesh[33].loadOBJ("Models/Car_00.obj"); // Voiture 1
    mesh[34].loadOBJ("Models/Car_01.obj"); // Voiture 2
    mesh[35].loadOBJ("Models/Swing.obj"); // Balançoire
    mesh[36].loadOBJ("Models/Sun.obj"); // Soleil

    // Charger les textures
    // Sol
    texture[0].loadTexture("Textures/Floor.jpg", true); // Sol
    // Maisons
    texture[1].loadTexture("Textures/Cabin_00.png", true); // Cabane 1
    texture[2].loadTexture("Textures/Cabin_01.png", true); // Cabane 2
    texture[3].loadTexture("Textures/Chalet.png", true); // Chalet
    texture[4].loadTexture("Textures/Church.jpg", true); // Eglise
    texture[5].loadTexture("Textures/LogCabin.png", true); // Cabane en rondins
    texture[6].loadTexture("Textures/TreeHouse.png", true); // Cabane dans l'arbre
    // Végétation
    texture[7].loadTexture("Textures/Bush.png", true); // Buisson
    texture[8].loadTexture("Textures/FirTree.png", true); // Sapin
    texture[9].loadTexture("Textures/Forest.png", true); // Forêt
    texture[10].loadTexture("Textures/Grass.png", true); // Herbe
    texture[11].loadTexture("Textures/OakTree.png", true); // Chêne
    texture[12].loadTexture("Textures/PineTree.png", true); // Pin
    texture[13].loadTexture("Textures/Pinecone.png", true); // Pomme de pin
    texture[14].loadTexture("Textures/Pond.png", true); // Etang
    texture[15].loadTexture("Textures/Rock.png", true); // Roche
    texture[16].loadTexture("Textures/Tree.png", true); // Arbre
    texture[17].loadTexture("Textures/Trunk_00.png", true); // Tronc 1
    texture[18].loadTexture("Textures/Trunk_01.png", true); // Tronc 2
    texture[19].loadTexture("Textures/Shrub.png", true); // Arbuste
    // Animaux
    texture[20].loadTexture("Textures/Badger.png", true); // Blaireau
    texture[21].loadTexture("Textures/Bear.png", true); // Ours
    texture[22].loadTexture("Textures/Bird.png", true); // Oiseau
    texture[23].loadTexture("Textures/Boar.png", true); // Sanglier
    texture[24].loadTexture("Textures/Deer_00.png", true); // Cerf 1
    texture[25].loadTexture("Textures/Deer_01.png", true); // Cerf 2
    texture[26].loadTexture("Textures/Elk.png", true); // Elan
    texture[27].loadTexture("Textures/Fawn.png", true); // Faon
    texture[28].loadTexture("Textures/Ferret.png", true); // Furet 
    texture[29].loadTexture("Textures/Fox.png", true); // Renard
    texture[30].loadTexture("Textures/Rabbit.png", true); // Lapin
    texture[31].loadTexture("Textures/Wolf.png", true); // Loup 
    // Accessoires
    texture[32].loadTexture("Textures/CampFire.png", true); // Feu de camp
    texture[33].loadTexture("Textures/Car_00.png", true); // Voiture 1 
    texture[34].loadTexture("Textures/Car_01.png", true); // Voiture 2
    texture[35].loadTexture("Textures/Swing.png", true); // Balançoire 
    texture[36].loadTexture("Textures/Sun.png", true); // Soleil


    // Mise à l'échelle des modeles
    glm::vec3 modelScale[numModels] = 
    {   
        glm::vec3(100.0f, 1.0f, 100.0f), // Sol
        glm::vec3(0.007f, 0.007f, 0.007f), // Cabane 1
        glm::vec3(0.007f, 0.007f, 0.007f), // Cabane 2
        glm::vec3(0.01f, 0.01f, 0.01f), // Chalet
        glm::vec3(0.34f, 0.34f, 0.34f), // Eglise
        glm::vec3(0.007f, 0.007f, 0.007f), // Cabane en rondins
        glm::vec3(0.34f, 0.34f, 0.34f), // Cabane dans l'arbre
        glm::vec3(1.0f, 1.0f, 1.0f), // Buisson
        glm::vec3(1.0f, 1.0f, 1.0f), // Sapin
        glm::vec3(20.0f, 20.0f, 20.0f), // Forêt
        glm::vec3(2.0f, 2.0f, 2.0f), // Herbe
        glm::vec3(1.0f, 1.0f, 1.0f), // Chêne
        glm::vec3(0.8f, 0.8f, 0.8f), // Pin
        glm::vec3(0.15f, 0.15f, 0.15f), // Pomme de pin
    };

    // Position des modeles
    glm::vec3 modelPosition[numModels] = 
    {
        glm::vec3(0.0f, 0.0f, 0.0f), // Sol
        glm::vec3(0.0f, 0.0f, 0.0f), // Cabane 1
        glm::vec3(10.0f, 0.0f, 0.0f), // Cabane 2
        glm::vec3(-10.0f, 0.0f, 0.0f), // Chalet
        glm::vec3(10.0f, 0.0f, 0.0f), // Eglise
        glm::vec3(10.0f, 0.0f, 0.0f), // Cabane en rondins
        glm::vec3(10.0f, 0.0f, 0.0f), // Cabane dans l'arbre
        glm::vec3(15.0f, 0.0f, 0.0f), // Buisson
        glm::vec3(20.0f, 0.0f, 0.0f), // Sapin
        glm::vec3(-30.0f, 0.0f, 0.0f), // Forêt
        glm::vec3(25.0f, 0.0f, 0.0f), // Herbe
        glm::vec3(35.0f, 0.0f, 0.0f), // Chêne
        glm::vec3(45.0f, 0.0f, 0.0f), // Pin
        glm::vec3(0.0f, 0.0f, 10.0f), // Pomme de pin

    };

    // Rotation des modeles
    glm::vec3 modelRotation[numModels] = 
    {
        glm::vec3(0.0f, 0.0f, 0.0f), // Sol
        glm::vec3(0.0f, 0.0f, 0.0f), // Cabane 1
        glm::vec3(0.0f, 0.0f, 0.0f), // Cabane 2
        glm::vec3(0.0f, 180.0f, 0.0f), // Chalet
        glm::vec3(0.0f, 0.0f, 0.0f), // Eglise
        glm::vec3(0.0f, 0.0f, 0.0f), // Cabane en rondins
        glm::vec3(0.0f, 0.0f, 0.0f), // Cabane dans l'arbre
        glm::vec3(0.0f, 0.0f, 0.0f), // Buisson
        glm::vec3(0.0f, 0.0f, 0.0f), // Sapin
        glm::vec3(0.0f, 0.0f, 0.0f), // Forêt
        glm::vec3(0.0f, 0.0f, 0.0f), // Herbe
        glm::vec3(0.0f, 0.0f, 0.0f), // Chêne
        glm::vec3(0.0f, 0.0f, 0.0f), // Pin
        glm::vec3(0.0f, 0.0f, 0.0f), // Pomme de pin

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
        
        // Sol
        renderModel(lightingShader, mesh[0], texture[0], modelPosition[0], modelScale[0], modelRotation[0], model);

        // Maisons
        renderModel(lightingShader, mesh[1], texture[1], modelPosition[1], modelScale[1], modelRotation[1], model); // Cabane 1
        //renderModel(lightingShader, mesh[2], texture[2], modelPosition[2], modelScale[2], modelRotation[2], model); // Cabane 2
        //renderModel(lightingShader, mesh[3], texture[3], modelPosition[3], modelScale[3], modelRotation[3], model); // Chalet
        //renderModel(lightingShader, mesh[4], texture[4], modelPosition[4], modelScale[4], modelRotation[4], model); // Eglise
        //renderModel(lightingShader, mesh[5], texture[5], modelPosition[5], modelScale[5], modelRotation[5], model); // Cabane en rondins
        renderModel(lightingShader, mesh[6], texture[6], modelPosition[6], modelScale[6], modelRotation[6], model); // Cabane dans l'arbre
        renderModel(lightingShader, mesh[7], texture[7], modelPosition[7], modelScale[7], modelRotation[7], model); // Buisson
        renderModel(lightingShader, mesh[8], texture[8], modelPosition[8], modelScale[8], modelRotation[8], model); // Sapin
        renderModel(lightingShader, mesh[9], texture[9], modelPosition[9], modelScale[9], modelRotation[9], model); // Forêt
        renderModel(lightingShader, mesh[10], texture[10], modelPosition[10], modelScale[10], modelRotation[10], model); // Herbe
        renderModel(lightingShader, mesh[11], texture[11], modelPosition[11], modelScale[11], modelRotation[11], model); // Chêne
        renderModel(lightingShader, mesh[12], texture[12], modelPosition[12], modelScale[12], modelRotation[12], model); // Pin
        renderModel(lightingShader, mesh[13], texture[13], modelPosition[13], modelScale[13], modelRotation[13], model); // Pomme de pin

        // Echange des buffers----------------------------------
        glfwSwapBuffers(display.gWindow);

        // Mise à jour du temps---------------------------------
        lastTime = currentTime;
    }

    // Nettoyage-----------------------------------------------------
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}