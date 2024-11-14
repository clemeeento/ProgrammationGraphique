#include "Lights.hpp"


Lights::Lights(Camera& camera, Display& display) : fpsCamera(camera), display(display) {}

// Fonction pour la lampe torche
void Lights::spotlightShaders(ShaderProgram& lightingShader, glm::vec3 spotlightPos)
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
void Lights::setPointLight(ShaderProgram& shader, int index, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float exponent) 
{
    std::string base = "pointLights[" + std::to_string(index) + "].";

    shader.setUniform((base + "ambient").c_str(), ambient);
    shader.setUniform((base + "diffuse").c_str(), diffuse);
    shader.setUniform((base + "specular").c_str(), specular);
    shader.setUniform((base + "position").c_str(), position);
    shader.setUniform((base + "constant").c_str(), constant);
    shader.setUniform((base + "linear").c_str(), linear);
    shader.setUniform((base + "exponent").c_str(), exponent);
}

// Fonction pour la lumière du soleil
void Lights::setSunLight(ShaderProgram& shader, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular) 
{
    shader.setUniform("sunLight.direction", direction);
    shader.setUniform("sunLight.diffuse", diffuse);
    shader.setUniform("sunLight.specular", specular);
}

// Fonction pour la lumière directionnelle
void Lights::setDirectionalLight(ShaderProgram& shader, int index, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular) 
{
    std::string base = "directionalLight[" + std::to_string(index) + "].";
    shader.setUniform((base + "direction").c_str(), direction);
    shader.setUniform((base + "diffuse").c_str(), diffuse);
    shader.setUniform((base + "specular").c_str(), specular);
}

