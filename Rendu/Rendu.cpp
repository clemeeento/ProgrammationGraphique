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
    mesh[0].loadOBJ("Models/Floor.obj");
    // Maisons
    mesh[1].loadOBJ("Models/Cabin_00.obj");
    // mesh[2].loadOBJ("Models/Cabin_01.obj");
    // mesh[3].loadOBJ("Models/Chalet.obj");
    // mesh[4].loadOBJ("Models/Church.obj");
    // mesh[5].loadOBJ("Models/LogCabin.obj");
    // // Végétation
    // mesh[6].loadOBJ("Models/Bush.obj");
    // mesh[7].loadOBJ("Models/BuingnaTree.obj");
    // mesh[8].loadOBJ("Models/FirTree.obj");
    // mesh[9].loadOBJ("Models/Forest.obj");
    // Animaux

    // Accessoires


    // Charger les textures
    // Sol
    texture[0].loadTexture("Textures/Floor.jpg", true);
    // Maisons
    texture[1].loadTexture("Textures/Cabin_00.png", true);
    // texture[2].loadTexture("Textures/Cabin_01.png", true);
    // texture[3].loadTexture("Textures/Chalet.png", true);
    // texture[4].loadTexture("Textures/Church.jpg", true);
    // texture[5].loadTexture("Textures/LogCabin.png", true); 
    // // Végétation
    // texture[6].loadTexture("Textures/Bush.png", true);
    // Animaux

    // Accessoires

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
        for(int i = 0; i < numModels; i = i + 1)
        {
            renderModel(lightingShader, mesh[i], texture[i], modelPosition[i], modelScale[i], modelRotation[i], model);
        }

        // Echange des buffers----------------------------------
        glfwSwapBuffers(display.gWindow);

        // Mise à jour du temps---------------------------------
        lastTime = currentTime;
    }

    // Nettoyage-----------------------------------------------------
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}