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

void affichage(glm::mat4 model, ShaderProgram lightingShader)
{
    // Uniforms-----------------------------------------------------
    lightingShader.setUniform("model", model);
            
    lightingShader.setUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    lightingShader.setUniformSampler("material.diffuseMap", 0);
    lightingShader.setUniform("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
    lightingShader.setUniform("material.shininess", 32.0f);
}

void afficherPiece(glm::mat4 model, ShaderProgram lightingShader)
{
    // Mesh et texture---------------------------------------------
    // Creation du mesh
    const int numModels = 6;
    Mesh mesh[numModels];
    Texture2D texture[numModels];

    // Charger les objets
    mesh[0].loadOBJ("Models/Wall_Floor.obj"); // Sol
    mesh[1].loadOBJ("Models/Wall_Floor.obj"); // Mur droite
    mesh[2].loadOBJ("Models/Wall_Floor.obj"); // Mur gauche
    mesh[3].loadOBJ("Models/Wall_Floor.obj"); // Mur arriere
    mesh[4].loadOBJ("Models/Wall_Floor.obj"); // Mur avant
    mesh[4].loadOBJ("Models/Wall_Floor.obj"); // Toit

    // Charger les textures
    texture[0].loadTexture("Textures/Floor.jpg", true); // Sol
    texture[1].loadTexture("Textures/Mur.png", true); // Mur droite
    texture[2].loadTexture("Textures/Mur.png", true); // Mur gauche
    texture[3].loadTexture("Textures/Mur.png", true); // Mur arriere
    texture[4].loadTexture("Textures/Mur.png", true); // Mur avant
    texture[5].loadTexture("Textures/Toit.jpg", true); // Toit

    // Position des modeles----------------------------------------
    glm::vec3 modelPosition[] = 
    {
        glm::vec3(0.0f, 0.0f, 0.0f),	// Sol
        glm::vec3(50.0f, 0.0f, 0.0f),	// Mur droite
        glm::vec3(-50.0f, 0.0f, 0.0f),	// Mur gauche
        glm::vec3(0.0f, 0.0f, -50.0f),	// Mur fond
        glm::vec3(0.0f, 0.0f, 50.0f),	// Mur avant
        glm::vec3(0.0f, 100.0f, 0.0f)	// Toit
    };

    // Mise à l'échelle des modeles--------------------------------
    glm::vec3 modelScale[] = 
    {
        glm::vec3(10.0f, 1.0f, 10.0f),	// Sol
        glm::vec3(10.0f, 1.0f, 10.0f),	// Mur droite
        glm::vec3(10.0f, 1.0f, 10.0f),	// Mur gauche
        glm::vec3(10.0f, 1.0f, 10.0f),	// Mur fond
        glm::vec3(10.0f, 1.0f, 10.0f),	// Mur avant
        glm::vec3(10.0f, 1.0f, 10.0f)	// Toit
    };

    // Affichage de la piece----------------------------------------
    // Sol
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, modelScale[0]);
    affichage(model, lightingShader);
    texture[0].bind(0);
    mesh[0].draw();
    texture[0].unbind(0);

    // Mur droite
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, modelScale[0]);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    affichage(model, lightingShader);
    texture[1].bind(0);
    mesh[1].draw();
    texture[1].unbind(0);

    // Mur gauche
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, modelScale[0]);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    affichage(model, lightingShader);
    texture[2].bind(0);
    mesh[2].draw();
    texture[2].unbind(0);

    // Mur arriere
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, modelScale[0]);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    affichage(model, lightingShader);
    texture[3].bind(0);
    mesh[3].draw();
    texture[3].unbind(0);

    // Mur avant
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, modelScale[0]);
    model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    affichage(model, lightingShader);
    texture[4].bind(0);
    mesh[4].draw();
    texture[4].unbind(0);

    // Toit
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPosition[0]);
    model = glm::scale(model, modelScale[0]);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    affichage(model, lightingShader);
    texture[5].bind(0);
    mesh[5].draw();
    texture[5].unbind(0);
}