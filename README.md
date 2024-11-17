# Programmation Graphique avec OpenGL

## **Introduction**

Ce dépôt contient les ressources, le code source et les étapes suivies dans le cadre du cours de **Programmation Graphique avec OpenGL**. Ce cours introduit les concepts fondamentaux de la programmation graphique en utilisant OpenGL, tels que les shaders, le mapping de textures, les caméras virtuelles, et les techniques de rendu 3D.

Le dépôt est structuré en deux parties principales :
1. Le code des exercices guidés couvrant toutes les étapes des vidéos.
2. Un projet final intégrant et consolidant les techniques apprises, permettant de mettre en pratique les notions abordées.


## **Installation des dépendances**

Pour exécuter le code présent dans ce dépôt, vous aurez besoin des bibliothèques suivantes :  
- **OpenGL** : La bibliothèque de base pour le rendu graphique.  
- **GLFW** : Pour la gestion de la fenêtre et des entrées utilisateur.  
- **GLEW** : Pour la gestion des extensions OpenGL.  
- **glm** : Une bibliothèque mathématique pour les calculs 3D (inclus directement dans le projet via les fichiers d'en-tête).  


## **Présentation des vidéos**

Les vidéos du cours sont hébergées sur une plateforme privée, accessible uniquement via le site du professeur. Voici la liste des vidéos accompagnées de leur progression :  

1. **Introduction** : vidéo de présentation.
2. **Window and Input** : dossier `Window and Input`.
3. **First Triangle** : dossier `First Triangle`.
4. **Shaders** : dossier `Shaders`.
5. **Texture Mapping** : dossier `Shaders`.
6. **Coordinate System**
7. **Virtual Camera I** : dossier `Virtual Camera`.
8. **Virtual Camera II** : dossier `Virtual Camera`.
9. **Load Object** : dossier `Load Object`.
10. **Basic Lightning I** : dossier `Basic Lightning`.
11. **Basic Lightning II** : dossier `Basic Lightning`.
12. **Basic Lightning III** : dossier `Basic Lightning`.

Avancement : Toutes les vidéos (12/12) ont été suivies et leur code associé est inclus dans ce dépôt.


## **Projet Final : Rendu 3D**

### **Description**

Le projet final disponible dans le dossier `Rendu` met en œuvre toutes les notions apprises dans le cours pour créer une scène 3D interactive et réaliste. Voici les fonctionnalités principales :

- **Chargement de modèles 3D** : Les objets sont importés à partir de fichiers externes et rendus dans la scène.
- **Système de caméra** : Une caméra virtuelle permet d'explorer la scène de manière dynamique.
- **Éclairage avancé** : Plusieurs sources de lumière, comme des points lumineux et des spots, sont utilisées pour ajouter du réalisme.
- **Gestion des textures** : Des textures sont appliquées aux objets pour enrichir leur apparence.
- **Moteur de rendu personnalisé** : Un moteur simple mais efficace gère les transformations, le rendu et l'affichage.

### **Fonctionnement de la scène**

1. **Scène principale** : La scène du projet final représente un village entouré de forêt, avec des animaux et des accessoires tels que des voitures et des feux de camp. Les éléments de la scène sont positionnés et orientés grâce à trois matrices fondamentales :
    - Matrice de modèle : Permet de positionner, orienter et échelonner chaque objet dans la scène.
    - Matrice de vue : Définit le point de vue de la caméra, simulant les déplacements dans l'environnement.
    - Matrice de projection : Gère la perspective pour donner une profondeur réaliste à la scène.
2. **Éclairage** : Le système d’éclairage inclut un cycle jour-nuit dynamique :
    - L’intensité lumineuse et la couleur de fond changent progressivement selon une fonction sinus, simulant le passage entre le jour et la nuit.
    - Les transitions entre les phases lumineuses (jour/nuit) sont douces et harmonieuses pour un rendu immersif.
3. **Interactivité** : Le projet propose des éléments interactifs :
    - Déplacement de la caméra : Utilisez W, A, S, D pour avancer, reculer ou tourner.
    - La caméra est également contrôlable avec la souris pour ajuster l'angle de vue.
    - Lampe torche : Activez ou désactivez la lampe torche avec la touche F pour explorer les zones sombres de la scène.

### **Structure générale du code**
Le projet est organisé en plusieurs modules, chacun jouant un rôle spécifique pour une meilleure clarté et modularité :

- **Camera.hpp / Camera.cpp** : Gère les caméras (modes FPS et Orbit).
- **Display.hpp / Display.cpp** : Configure OpenGL, initialise les fenêtres et gère leur affichage.
- **Lights.hpp / Lights.cpp** : Implémente les différents types d'éclairage utilisés dans la scène.
- **Models.hpp / Models.cpp** : Gère le chargement et l'affichage des modèles 3D.
- **Mesh.hpp / Mesh.cpp** : Définit et manipule les géométries des objets.
- **Texture2D.hpp / Texture2D.cpp** : Charge et applique les textures 2D aux objets.
- **ShaderProgram.hpp / ShaderProgram.cpp** : Charge et gère les shaders pour le rendu graphique.

### **Démonstration**

Une vidéo de démonstration (`Présenation du rendu`) est disponible pour illustrer le rendu final de ce projet.


## **Compilation et Exécution**

### **Exercices des vidéos**
Pour compiler les fichiers des différentes étapes, utilisez les commandes suivantes (sous Windows avec MinGW64) :

```bash
g++ WindowAndInput.cpp -o WindowAndInput -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ FirstTriangle.cpp -o FirstTriangle -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ FirstTriangleBis.cpp -o FirstTriangleBis -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ Shaders.cpp ShaderProgram.cpp -o Shaders -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ TextureMapping.cpp ShaderProgram.cpp Texture2D.cpp -o TextureMapping -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ VirtualCamera_I.cpp ShaderProgram.cpp Texture2D.cpp -o VirtualCamera_I -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ VirtualCamera_II.cpp ShaderProgram.cpp Texture2D.cpp Camera.cpp -o VirtualCamera_II -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ LoadObjet.cpp ShaderProgram.cpp Texture2D.cpp Camera.cpp Mesh.cpp -o LoadObjet -lopengl32 -lglew32 -lglfw3 -lgdi32  
g++ BasicLightning.cpp ShaderProgram.cpp Texture2D.cpp Camera.cpp Mesh.cpp -o BasicLightning -lopengl32 -lglew32 -lglfw3 -lgdi32
```

### **Projet final**

Le projet final se compile et s'exécute avec la commande suivante :  

```bash
g++ Rendu.cpp ShaderProgram.cpp Texture2D.cpp Camera.cpp Mesh.cpp Display.cpp Models.cpp Lights.cpp -o Rendu -lopengl32 -lglew32 -lglfw3 -lgdi32  
```
