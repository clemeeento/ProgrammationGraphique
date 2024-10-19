#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>


class Camera 
{
public :

    glm::mat4 getViewMatrix() const; // Retourne la matrice de vue

    virtual void rotate(float yaw, float pitch) {} // Rotation de la caméra en degrés

protected :
    Camera();

    glm::vec3 mPosition;
    glm::vec3 mTarget;
    glm::vec3 mUp;

    // Euler angles (en radians)
    float mYaw;
    float mPitch;
};

class OrbitCamera : public Camera
{
public :

    OrbitCamera();

    virtual void rotate(float yaw, float pitch); // Rotation de la caméra en degrés

    void setLookAt(const glm::vec3& target); // Definir le point cible
    void setRadius(float radius); // Definir le rayon
    void updateCameraVectors(); // Mise à jour des vecteurs de la cam

private :

    float mYaw;
    float mPitch;
    float mRadius;
};

#endif // CAMERA_HPP