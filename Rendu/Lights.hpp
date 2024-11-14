#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <string>

#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Display.hpp"

class Lights 
{
public:
    Lights(Camera& camera, Display& display);

    void spotlightShaders(ShaderProgram& lightingShader, glm::vec3 spotlightPos);
    void setPointLight(ShaderProgram& shader, int index, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float exponent);
    void setSunLight(ShaderProgram& shader, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular);
    void setDirectionalLight(ShaderProgram& shader, int index, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 specular); 

private:
    Camera& fpsCamera;
    Display& display;
};

#endif // LIGHTS_HPP
