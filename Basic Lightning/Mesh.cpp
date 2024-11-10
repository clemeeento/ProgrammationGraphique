#include "Mesh.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

// Fonction pour séparer une chaîne de caractères en fonction d'un délimiteur
std::vector<std::string> split(std::string s, std::string t)
{
    std::vector<std::string> res;
    while(1)
    {
        int pos = s.find(t);
        if(pos == -1)
        {
            res.push_back(s);
            break;
        }
        res.push_back(s.substr(0, pos));
        s = s.substr(pos + 1, s.size() - pos - 1);
    }
    return res;
}

// Constructeur de la classe Mesh, initialisant le statut de chargement à faux
Mesh::Mesh()
{
    mLoaded = false;
}

// Destructeur de la classe Mesh, libérant les ressources allouées par OpenGL
Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &mVAO); // Supprime le VAO
    glDeleteBuffers(1, &mVBO);      // Supprime le VBO
}

// Charge un fichier OBJ et extrait les informations de sommets et de textures
bool Mesh::loadOBJ(const std::string& filename)
{
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices; // Vecteurs pour stocker les indices des sommets, des textures et des normales
    std::vector<glm::vec3> tempVertices; // Vecteur temporaire pour les positions des sommets
    std::vector<glm::vec2> tempUVs;      // Vecteur temporaire pour les coordonnées de texture
    std::vector<glm::vec3> tempNormals;  // Vecteur temporaire pour les normales

    // Vérifie que le fichier est un .obj
    if(filename.find(".obj") != std::string::npos)
    {
        std::ifstream fin(filename, std::ios::in); // Ouvre le fichier OBJ en mode lecture
        if(!fin)
        {
            std::cerr << "Impossible d'ouvrir " << filename << std::endl;
            return false;
        }

        std::cout << "Chargement du fichier OBJ " << filename << " ..." << std::endl;

        std::string lineBuffer;
        // Parcours chaque ligne du fichier pour analyser les sommets et faces
        while(std::getline(fin, lineBuffer))
        {
            // Crée un flux à partir de la ligne
            std::stringstream ss(lineBuffer); 
            std::string cmd;
            ss >> cmd;

            // Si la ligne commence par 'v', elle contient un sommet
            if(cmd == "v")
            {
                glm::vec3 vertex; // Crée un sommet
                int dim = 0; // Compteur pour les dimensions
                while(dim < 3 && ss >> vertex[dim]) // Lit les coordonnées du sommet
                {
                    dim = dim + 1;
                }
                tempVertices.push_back(vertex); // Ajoute le sommet à la liste
            }
            // Si la ligne commence par 'vt', elle contient des coordonnées de texture
            else if(cmd == "vt")
            {
                glm::vec2 uv; // Crée des coordonnées de texture
                int dim = 0; // Compteur pour les dimensions
                while(dim < 2 && ss >> uv[dim]) // Lit les coordonnées de texture
                {
                    dim = dim + 1;
                }
                tempUVs.push_back(uv); // Ajoute la coordonnée de texture
            }
            // Si la ligne commence par 'vn', elle contient une normale
            else if(cmd == "vn")
            {
                glm::vec3 normal; // Crée une normale
                int dim = 0; // Compteur pour les dimensions
                while(dim < 3 && ss >> normal[dim]) // Lit les coordonnées de la normale
                {
                    dim = dim + 1;
                }
                normal = glm::normalize(normal); // Normalise la normale
                tempNormals.push_back(normal); // Ajoute la normale à la liste
            }
            // Si la ligne commence par 'f', elle contient une face
            else if(cmd == "f")
            {
                std::string faceData;
                int vertexIndex, uvIndex, normalIndex; // Indices des sommets v, des textures vt et des normales vn

                while(ss >> faceData)
                {
                    std::vector<std::string> data = split(faceData, "/"); // Sépare les données de la face

                    // Si la face contient un sommet
                    if(data[0].size() > 0)
                    {
                        sscanf_s(data[0].c_str(), "%d", &vertexIndex); // Récupère l'indice du sommet
                        vertexIndices.push_back(vertexIndex); // Ajoute l'indice du sommet
                    }

                    // Si la face contient une texture
                    if(data.size() >= 1)
                    {
                        if(data[1].size() > 0)
                        {
                            sscanf_s(data[1].c_str(), "%d", &uvIndex); // Récupère l'indice de la texture
                            uvIndices.push_back(uvIndex); // Ajoute l'indice de la texture
                        }
                    }

                    // Si la face contient une normale
                    if(data.size() >= 2)
                    {
                        if(data[2].size() > 0)
                        {
                            sscanf_s(data[2].c_str(), "%d", &normalIndex); // Récupère l'indice de la normale
                            normalIndices.push_back(normalIndex); // Ajoute l'indice de la normale
                        }
                    }
                }
            }
        }

        // Ferme le fichier
        fin.close();

        // Associe chaque sommet de chaque triangle avec ses attributs
        for(unsigned int i = 0; i < vertexIndices.size(); i = i + 1)
        {
            Vertex meshVertex;

            if(tempVertices.size() > 0)
            {
                glm::vec3 vertex = tempVertices[vertexIndices[i] - 1]; // Récupère le sommet
                meshVertex.position = vertex; // Ajoute la position du sommet
            }

            if(tempNormals.size() > 0)
            {
                glm::vec3 normal = tempNormals[normalIndices[i] - 1]; // Récupère la normale
                meshVertex.normal = normal; // Ajoute la normale
            }

            if(tempUVs.size() > 0)
            {
                glm::vec2 uv = tempUVs[uvIndices[i] - 1]; // Récupère la coordonnée de texture
                meshVertex.texCoords = uv; // Ajoute la coordonnée de texture
            }

            mVertices.push_back(meshVertex); // Ajoute le sommet au vecteur de sommets
		}

		// Crée les buffers et les initialise
		initBuffers();

		return (mLoaded = true);
	}

	// Retourne faux si le fichier n'est pas un .obj
	return false;
}

void Mesh::draw()
{
	if(!mLoaded)
    {
        return;
    }

    glBindVertexArray(mVAO); // Lier le VAO
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size()); // Dessin du mesh
    glBindVertexArray(0); // Debind du VAO
}

void Mesh::initBuffers()
{
	glGenBuffers(1, &mVBO); // Creation du VBO
    glBindBuffer(GL_ARRAY_BUFFER, mVBO); // Lier le VBO
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // Copie des donnees, GL_STATIC_DRAW : utilisation des donnees statiques

    glGenVertexArrays(1, &mVAO); // Creation du VAO
    glBindVertexArray(mVAO); // Lier le VAO

    // Position des sommets
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); // Position des sommets
    glEnableVertexAttribArray(0); // Activation de l'attribut de sommet

    // Coordonnées de la normale
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat))); // Coordonnées de la normale
    glEnableVertexAttribArray(1); // Activation de l'attribut de normale

    // Coordonnées de la texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat))); // Coordonnées de la texture
    glEnableVertexAttribArray(2); // Activation de l'attribut de texture

    glBindVertexArray(0); // Debind du VAO
}