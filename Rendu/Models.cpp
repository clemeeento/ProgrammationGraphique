#include "Models.hpp"


// Initialiser les modeles
void Models::initializeModels(ShaderProgram &shader) 
{
    // Structure pour regrouper les donnees de chaque modele
    struct ModelInfo 
    {
        std::string name;         // Nom du modele pour la map
        std::string objFile;      // Chemin du fichier .obj
        std::string textureFile;  // Chemin du fichier de texture
        glm::vec3 scale;          // echelle du modele
    };

    // Tableau d'initialisation avec les donnees pour chaque modele
    std::vector<ModelInfo> modelInfos = 
    {
        { "sol", "Models/Floor.obj", "Textures/Floor.jpg", glm::vec3(100.0f, 1.0f, 100.0f) },
        { "cabane1", "Models/Cabin_00.obj", "Textures/Cabin_00.png", glm::vec3(0.007f, 0.007f, 0.007f) },
        { "cabane2", "Models/Cabin_01.obj", "Textures/Cabin_01.png", glm::vec3(0.007f, 0.007f, 0.007f) },
        { "chalet", "Models/Chalet.obj", "Textures/Chalet.png", glm::vec3(0.01f, 0.01f, 0.01f) },
        { "eglise", "Models/Church.obj", "Textures/Church.jpg", glm::vec3(0.34f, 0.34f, 0.34f) },
        { "cabane_rondins", "Models/LogCabin.obj", "Textures/LogCabin.png", glm::vec3(0.007f, 0.007f, 0.007f) },
        { "cabane_arbre", "Models/TreeHouse.obj", "Textures/TreeHouse.png", glm::vec3(0.34f, 0.34f, 0.34f) },
        { "sapin", "Models/FirTree.obj", "Textures/FirTree.png", glm::vec3(1.0f, 1.0f, 1.0f) },
        { "herbe", "Models/Grass.obj", "Textures/Grass.png", glm::vec3(2.0f, 2.0f, 2.0f) },
        { "chene", "Models/OakTree.obj", "Textures/OakTree.png", glm::vec3(1.0f, 1.0f, 1.0f) },
        { "pin", "Models/PineTree.obj", "Textures/PineTree.png", glm::vec3(0.8f, 0.8f, 0.8f) },
        { "pomme_pin", "Models/Pinecone.obj", "Textures/Pinecone.png", glm::vec3(0.15f, 0.15f, 0.15f) },
        { "roche", "Models/Rock.obj", "Textures/Rock.png", glm::vec3(1.0f, 1.0f, 1.0f) },
        { "tronc1", "Models/Trunk_00.obj", "Textures/Trunk_00.png", glm::vec3(1.5f, 1.5f, 1.5f) },
        { "tronc2", "Models/Trunk_01.obj", "Textures/Trunk_01.png", glm::vec3(1.0f, 1.0f, 1.0f) },
        { "arbuste", "Models/Shrub.obj", "Textures/Shrub.png", glm::vec3(0.5f, 0.5f, 0.5f) },
        { "blaireau", "Models/Badger.obj", "Textures/Badger.png", glm::vec3(0.1f, 0.1f, 0.1f) },
        { "oiseau", "Models/Bird.obj", "Textures/Bird.png", glm::vec3(0.005f, 0.005f, 0.005f) },
        { "sanglier", "Models/Boar.obj", "Textures/Boar.png", glm::vec3(0.01f, 0.01f, 0.01f) },
        { "cerf", "Models/Deer.obj", "Textures/Deer.png", glm::vec3(0.014f, 0.014f, 0.014f) },
        { "elan", "Models/Elk.obj", "Textures/Elk.png", glm::vec3(0.13f, 0.13f, 0.13f) },
        { "faon", "Models/Fawn.obj", "Textures/Fawn.png", glm::vec3(0.025f, 0.025f, 0.025f) },
        { "furet", "Models/Ferret.obj", "Textures/Ferret.png", glm::vec3(0.2f, 0.2f, 0.2f) },
        { "renard", "Models/Fox.obj", "Textures/Fox.png", glm::vec3(0.2f, 0.2f, 0.2f) },
        { "lapin", "Models/Rabbit.obj", "Textures/Rabbit.png", glm::vec3(0.006f, 0.006f, 0.006f) },
        { "loup", "Models/Wolf.obj", "Textures/Wolf.png", glm::vec3(0.6f, 0.6f, 0.6f) },
        { "feu_camp", "Models/CampFire.obj", "Textures/CampFire.png", glm::vec3(0.01f, 0.01f, 0.01f) },
        { "voiture", "Models/Car.obj", "Textures/Car.png", glm::vec3(0.04f, 0.04f, 0.04f) },
        { "balancoire", "Models/Swing.obj", "Textures/Swing.png", glm::vec3(0.016f, 0.016f, 0.016f) },
        { "soleil", "Models/Sun.obj", "Textures/Sun.png", glm::vec3(0.5f, 0.5f, 0.5f) },
        { "lumiere", "Models/Light.obj", "Textures/Light.jpg", glm::vec3(0.3f, 0.3f, 0.3f) }
    };

    // Chargement des modeles, textures et echelles dans la map de modeles
    for (const auto& info : modelInfos) 
    {
        auto mesh = std::make_unique<Mesh>();
        auto texture = std::make_unique<Texture2D>();
        
        mesh->loadOBJ(info.objFile);              // Charger l'objet
        texture->loadTexture(info.textureFile);   // Charger la texture

        modelMap.emplace(info.name, ModelData(std::move(mesh), std::move(texture), shader, info.scale));
        std::cout << "Modele charge : " << info.name << std::endl;
    }
}

// Afficher un modele
void Models::renderModel(std::string name, glm::vec3 position, glm::vec3 rotation, glm::mat4 model) 
{
    // Verifier si le modele existe dans la map
    if (modelMap.find(name) == modelMap.end()) 
    {
        std::cerr << "Le modele '" << name << "' n'exite pas !" << std::endl;
        return;
    }

    ModelData& modelData = modelMap[name];


    // Configurer la transformation
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, modelData.scale);

    // Uniforms
    modelData.shader.setUniform("model", model);
    modelData.shader.setUniform("material.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
    modelData.shader.setUniformSampler("material.diffuseMap", 0);
    modelData.shader.setUniform("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
    modelData.shader.setUniform("material.shininess", 32.0f);

    // Lier la texture
    modelData.texture->bind(0);

    // Rendre le mesh
    modelData.mesh->draw();

    // Delier la texture
    modelData.texture->unbind(0);
}
