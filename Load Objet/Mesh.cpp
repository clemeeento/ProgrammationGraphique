#include "Mesh.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

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
    std::vector<unsigned int> vertexIndices, uvIndices; // Vecteurs pour stocker les indices des sommets et des textures
    std::vector<glm::vec3> tempVertices; // Vecteur temporaire pour les positions des sommets
    std::vector<glm::vec2> tempUVs;      // Vecteur temporaire pour les coordonnées de texture

    // Vérifie que le fichier est un .obj
    if (filename.find(".obj") != std::string::npos)
    {
        std::ifstream fin(filename, std::ios::in); // Ouvre le fichier OBJ en mode lecture
        if (!fin)
        {
            std::cerr << "Impossible d'ouvrir " << filename << std::endl;
            return false;
        }

        std::cout << "Chargement du fichier OBJ " << filename << " ..." << std::endl;

        std::string lineBuffer;
        // Parcours chaque ligne du fichier pour analyser les sommets et faces
        while (std::getline(fin, lineBuffer))
        {
            // Si la ligne commence par 'v ', elle contient un sommet
            if (lineBuffer.substr(0, 2) == "v ")
            {
                std::istringstream v(lineBuffer.substr(2));
                glm::vec3 vertex; // Crée un sommet
                v >> vertex.x; v >> vertex.y; v >> vertex.z; // Lit les coordonnées
                tempVertices.push_back(vertex); // Ajoute le sommet à la liste
            }
            // Si la ligne commence par 'vt', elle contient des coordonnées de texture
            else if (lineBuffer.substr(0, 2) == "vt")
            {
                std::istringstream vt(lineBuffer.substr(3));
                glm::vec2 uv; // Crée des coordonnées de texture
                vt >> uv.s; vt >> uv.t; // Lit les coordonnées
                tempUVs.push_back(uv); // Ajoute la coordonnée de texture
            }
            // Si la ligne commence par 'f ', elle contient une face
            else if (lineBuffer.substr(0, 2) == "f ")
            {
                int p1, p2, p3; // Indices pour les sommets de la face
                int t1, t2, t3; // Indices pour les textures
                int n1, n2, n3; // Indices pour les normales (ignorés ici)
                const char* face = lineBuffer.c_str();
                // Lit les indices des sommets et textures de la face
                int match = sscanf(face, "f %i/%i/%i %i/%i/%i %i/%i/%i",
                                   &p1, &t1, &n1,
                                   &p2, &t2, &n2,
                                   &p3, &t3, &n3);
                if (match != 9)
                    std::cout << "Échec du parsing du fichier OBJ avec ce simple chargeur OBJ" << std::endl;

                // Enregistre les indices des sommets et textures
                vertexIndices.push_back(p1);
                vertexIndices.push_back(p2);
                vertexIndices.push_back(p3);

                uvIndices.push_back(t1);
                uvIndices.push_back(t2);
                uvIndices.push_back(t3);
            }
        }

        // Ferme le fichier
        fin.close();

        // Associe chaque sommet de chaque triangle avec ses attributs
        for (unsigned int i = 0; i < vertexIndices.size(); i++)
        {
            // Récupère les coordonnées du sommet en utilisant l'indice
            glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
			glm::vec2 uv = tempUVs[uvIndices[i] - 1];

			Vertex meshVertex;
			meshVertex.position = vertex;
			meshVertex.texCoords = uv;

			mVertices.push_back(meshVertex);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL); // Position des sommets
    glEnableVertexAttribArray(0); // Activation de l'attribut de sommet

    // Coordonnées de la texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Coordonnées de la texture
    glEnableVertexAttribArray(1); // Activation de l'attribut de texture

    glBindVertexArray(0); // Debind du VAO
}