#include "Camera.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>



Camera::Camera()
{
    mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mYaw = 0.0f;
    mPitch = 0.0f;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(mPosition, mTarget, mUp);
}

// OrbitCamera
OrbitCamera::OrbitCamera()
{
    mRadius = 10.0f;
}

void OrbitCamera::setLookAt(const glm::vec3& target)
{
    mTarget = target;
}

void OrbitCamera::setRadius(float radius)
{
    mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

void OrbitCamera::rotate(float yaw, float pitch)
{
    // Conversion des angles en radians
    mYaw = glm::radians(yaw);
    mPitch = glm::radians(pitch);

    // Limitation de l'angle de pitch pour éviter les problèmes de caméra
    mPitch =  glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f); 

    // Mise à jour des vecteurs de la cam
    updateCameraVectors();
}

void OrbitCamera::updateCameraVectors()
{
    // Conversion des coordonnées sphériques en coordonnées cartésiennes
    mPosition.x = mTarget.x + mRadius * cos(mPitch) * sin(mYaw);
    mPosition.y = mTarget.y + mRadius * sin(mPitch);
    mPosition.z = mTarget.z + mRadius * cos(mPitch) * cos(mYaw);
}