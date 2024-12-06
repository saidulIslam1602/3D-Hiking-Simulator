// HikingSimulator.h

#ifndef HIKING_SIMULATOR_H
#define HIKING_SIMULATOR_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory> // For std::unique_ptr
#include <glm/glm.hpp>
#include "Terrain.h"
#include "Hiker.h"
#include "Shader.h"
#include "Lighting.h"
#include "AnimatedCharacter.h"
#include "ParticleSystem.h"

class HikingSimulator {
public:
    HikingSimulator();
    bool initialize();
    void render(float deltaTime);
    void processInput(GLFWwindow* window, float deltaTime);
    void processMouseMovement(float xpos, float ypos);
    void cleanup();

    void setWindowDimensions(int width, int height);

private:
    Terrain terrain;
    Hiker hiker;
    AnimatedCharacter animatedCharacter;
    Lighting lighting;
    float width, height;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 cameraPosition;

    float windowWidth;
    float windowHeight;
    float lastFrameTime;

    enum class CameraMode { OVERVIEW, FOLLOW, FIRST_PERSON };
    CameraMode cameraMode;

    void setupMatrices();
    void updateViewMatrix();
    void updateProjectionMatrix();

    // Camera control variables
    float orbitAngle;
    float orbitSpeed;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    bool isMouseEnabled;
    bool firstMouse;
    float lastX;
    float lastY;
    float yaw;
    float pitch;

    // Particle system for rain
    ParticleSystem rainParticleSystem;
    bool isRaining;

    // Shader pointers
    std::unique_ptr<Shader> pathShader;
    std::unique_ptr<Shader> characterShader;
    std::unique_ptr<Shader> traceShader;

    void toggleRain();
};

#endif // HIKING_SIMULATOR_H
