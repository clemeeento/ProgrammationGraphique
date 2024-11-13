#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.hpp"

#define GLEW_STATIC

class Display 
{
public: 
    Display(FPSCamera &camera);

    bool initOpenGL();
    void showFPS(GLFWwindow* gWindow);
    void update(double elapsedTime);
    void static glfw_onFramebufferSize(GLFWwindow* gWindow, int width, int height);
    void static glfw_onKey(GLFWwindow* gWindow, int key, int scancode, int action, int mode);

    GLFWwindow* gWindow = NULL;

    static int gWindowWidth; 
    static int gWindowHeight;

    static bool gFlashlightOn;

private:
    const char* APP_TITLE = "Rendu OpenGL - Clément Furnon";

    static bool gFullScreen;

    static bool VSync; 
    glm::vec4 gClearColor = glm::vec4(0.392f, 0.667f, 0.922f, 1.0f);

    FPSCamera &fpsCamera;

    const float MOVE_SPEED = 5.0;
    const float MOUSE_SENSITIVITY = 0.1f;
    const float GROUND_HEIGHT = 1.5f; 
};

#endif // CAMERA_HPP