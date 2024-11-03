#include "Camera.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

const float DEF_FOV = 45.0f;

Camera::Camera() :  WORLD_UP(0.0f, 1.0f, 0.0f)
{
    mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mYaw = glm::pi<float>();
    mPitch = 0.0f;
    mFov = DEF_FOV;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(mPosition, mTargetPos, mUp);
}

const glm::vec3& Camera::getLook() const
{
    return mLook;
}

const glm::vec3& Camera::getRight() const
{
    return mRight;
}

const glm::vec3& Camera::getUp() const
{
    return mUp;
}


// FPSCamera-----------------------------------------------------

FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
{
    mPosition = position;
    mYaw = yaw;
    mPitch = pitch;
}

void FPSCamera::setPosition(const glm::vec3& position)
{
    mPosition = position;
}

void FPSCamera::rotate(float yaw, float pitch)
{
    mYaw = mYaw + glm::radians(yaw);
    mPitch = mPitch + glm::radians(pitch);

    // Limitation de l'angle de pitch pour éviter les problèmes de caméra
    mPitch =  glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f); 

    // Mise à jour des vecteurs de la camera
    updateCameraVectors();
}

void FPSCamera::move(const glm::vec3& offsetPos)
{
    mPosition = mPosition + offsetPos;

    // Mise à jour des vecteurs de la camera
    updateCameraVectors();
}

void FPSCamera::updateCameraVectors()
{
    glm::vec3 look;

    // Calcul du vecteur de direction de la vue en fonction des angles yaw et pitch
    look.x = cos(mPitch) * sin(mYaw);
    look.y = sin(mPitch);
    look.z = cos(mPitch) * cos(mYaw);

    mLook = glm::normalize(look);

    // Calcul des vecteurs droit et haut
    mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
    mUp = glm::normalize(glm::cross(mRight, mLook));

    mTargetPos = mPosition + mLook;
}


// OrbitCamera---------------------------------------------------
OrbitCamera::OrbitCamera()
{
    mRadius = 10.0f;
}

void OrbitCamera::setLookAt(const glm::vec3& target)
{
    mTargetPos = target;
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
    mPosition.x = mTargetPos.x + mRadius * cos(mPitch) * sin(mYaw);
    mPosition.y = mTargetPos.y + mRadius * sin(mPitch);
    mPosition.z = mTargetPos.z + mRadius * cos(mPitch) * cos(mYaw);
}