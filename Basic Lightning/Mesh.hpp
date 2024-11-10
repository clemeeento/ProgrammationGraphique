#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#define GLEW_STATIC

struct Vertex
{
	glm::vec3 position; // Position du vertex
	glm::vec3 normal; // Normale du vertex
	glm::vec2 texCoords; // Coordonnées de texture 
};

class Mesh
{
public:

	Mesh();
	~Mesh();

	bool loadOBJ(const std::string& filename); // Charge un modèle OBJ
	void draw(); // Dessine le mesh

private:

	void initBuffers(); // Initialise les buffers

	bool mLoaded; // Indique si le mesh est chargé
	std::vector<Vertex> mVertices; // Vecteur de vertices
	GLuint mVBO, mVAO; // Identifiants des buffers, Vertex Buffer Object et Vertex Array Object
};

#endif //MESH_H
