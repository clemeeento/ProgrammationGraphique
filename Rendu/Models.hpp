#ifndef MODELS_HPP
#define MODELS_HPP

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.hpp"
#include "Texture2D.hpp"
#include "ShaderProgram.hpp"

struct ModelData 
{
    std::unique_ptr<Mesh> mesh; // Mesh du modèle 
    std::unique_ptr<Texture2D> texture; // Texture du modèle
    ShaderProgram shader; // Programme de shader
    glm::vec3 scale; // Echelle

    // Constructeur par défaut
    ModelData() = default;

    // Constructeur avec paramètres
    ModelData(std::unique_ptr<Mesh> m, std::unique_ptr<Texture2D> t, ShaderProgram& s, glm::vec3 sc)
        : mesh(std::move(m)), texture(std::move(t)), shader(s), scale(sc) {}
};

class Models 
{
public:
    void initializeModels(ShaderProgram& shader); // Initialiser les modèles
    void renderModel(std::string name, glm::vec3 position, glm::vec3 rotation, glm::mat4 model); // Afficher un modèle

private:
    std::unordered_map<std::string, ModelData> modelMap; // Map pour stocker les modèles avec un nom en clé
};


#endif // MODELS_HPP