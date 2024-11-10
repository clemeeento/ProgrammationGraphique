#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Camera 
{
public :

    glm::mat4 getViewMatrix() const; // Retourne la matrice de vue

    virtual void setPosition(const glm::vec3& position) {} // Definir la position de la camera
    virtual void rotate(float yaw, float pitch) {} // Rotation de la camera en degres
    virtual void move(const glm::vec3& offsetPos) {} // Deplacement de la camera

    const glm::vec3& getLook() const ;// Retourne le point cible
    const glm::vec3& getRight() const ;// Retourne le vecteur droit
    const glm::vec3& getUp() const ;// Retourne le vecteur haut

    float getFOV() const {return mFov;}// Retourne le champ de vision
    void setFOV(float fov) {mFov = fov;}// Definir le champ de vision

protected :
    Camera();

    virtual void updateCameraVectors() {} // Mise à jour des vecteurs de la camera

    glm::vec3 mPosition;
    glm::vec3 mTargetPos;
    glm::vec3 mUp;
    glm::vec3 mLook;
    glm::vec3 mRight;
    const glm::vec3 WORLD_UP;

    // Euler angles (en radians)
    float mYaw;
    float mPitch;

    // Paramètres de la camera
    float mFov; // Champ de vision en degres
};


class FPSCamera : public Camera
{
public :

    FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = glm::pi<float>(), float pitch = 0.0f); 

    virtual void setPosition(const glm::vec3& position); // Definir la position de la camera
    virtual void rotate(float yaw, float pitch); // Rotation de la camera en degres
    virtual void move(const glm::vec3& offsetPos); // Deplacement de la camera

private :

    virtual void updateCameraVectors(); // Mise à jour des vecteurs de la camera
};


class OrbitCamera : public Camera
{
public :

    OrbitCamera();

    virtual void rotate(float yaw, float pitch); // Rotation de la camera en degres

    void setLookAt(const glm::vec3& target); // Definir le point cible
    void setRadius(float radius); // Definir le rayon
    void updateCameraVectors(); // Mise à jour des vecteurs de la cam

private :

    float mYaw;
    float mPitch;
    float mRadius;
};

#endif // CAMERA_HPP