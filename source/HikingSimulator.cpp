// HikingSimulator.cpp

#include "HikingSimulator.h"

// Include glad before GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

HikingSimulator::HikingSimulator()
    : terrain(),
    hiker("data/hiker_path.txt"),
    animatedCharacter(),
    lighting(glm::vec3(1000.0f, 1000.0f, 1000.0f), glm::vec3(1.0f, 0.95f, 0.8f)),
    width(0),
    height(0),
    viewMatrix(glm::mat4(1.0f)),
    projectionMatrix(glm::mat4(1.0f)),
    modelMatrix(glm::mat4(1.0f)),
    cameraPosition(glm::vec3(0.0f)),
    windowWidth(800.0f),
    windowHeight(600.0f),
    lastFrameTime(0.0f),
    cameraMode(CameraMode::OVERVIEW),
    orbitAngle(0.0f),
    orbitSpeed(10.0f), // Degrees per second
    cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
    cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    isMouseEnabled(false),
    firstMouse(true),
    lastX(windowWidth / 2.0f),
    lastY(windowHeight / 2.0f),
    yaw(-90.0f),
    pitch(0.0f),
    rainParticleSystem(15000), // Initialize with max 2000 particles
    isRaining(false) {
}

void HikingSimulator::setWindowDimensions(int width, int height) {
    this->windowWidth = static_cast<float>(width);
    this->windowHeight = static_cast<float>(height);
    updateProjectionMatrix();
}

bool HikingSimulator::initialize() {
    std::cout << "INFO: Initializing HikingSimulator..." << std::endl;

    if (!terrain.loadTerrainData("data/terrain.png")) {
        std::cerr << "ERROR: Failed to load terrain heightmap!" << std::endl;
        return false;
    }

    hiker.setScales(terrain.getHorizontalScale(), terrain.getHeightScale());
    hiker.setTerrain(&terrain);

    width = terrain.getWidth();
    height = terrain.getHeight();

    if (width <= 0 || height <= 0) {
        std::cerr << "ERROR: Invalid terrain dimensions!" << std::endl;
        return false;
    }

    std::cout << "INFO: Terrain dimensions: " << width << " x " << height << std::endl;
    std::cout << "INFO: Terrain scales - Horizontal: " << terrain.getHorizontalScale()
        << ", Height: " << terrain.getHeightScale() << std::endl;

    Skybox& skybox = Skybox::getInstance();
    if (!skybox.initialize("textures/skybox/")) {
        std::cerr << "ERROR: Failed to initialize skybox!" << std::endl;
        return false;
    }

    if (!hiker.loadPathData(terrain)) {
        std::cerr << "ERROR: Failed to load hiker path!" << std::endl;
        return false;
    }
    else {
        std::cout << "INFO: Hiker path loaded successfully." << std::endl;
    }

    // Load shaders
    pathShader = std::make_unique<Shader>("shaders/hikerVert.glsl", "shaders/hikerFrag.glsl");
    if (!pathShader || !pathShader->isLoaded()) {
        std::cerr << "ERROR: Failed to load path shader during initialization." << std::endl;
        return false;
    }

    characterShader = std::make_unique<Shader>("shaders/characterVert.glsl", "shaders/characterFrag.glsl");
    if (!characterShader || !characterShader->isLoaded()) {
        std::cerr << "ERROR: Failed to load character shader during initialization." << std::endl;
        return false;
    }

    traceShader = std::make_unique<Shader>("shaders/traceVert.glsl", "shaders/traceFrag.glsl");
    if (!traceShader || !traceShader->isLoaded()) {
        std::cerr << "ERROR: Failed to load trace shader during initialization." << std::endl;
        return false;
    }

    setupMatrices();
    animatedCharacter.loadPathData(hiker.getPathPoints());
    lastFrameTime = static_cast<float>(glfwGetTime());

    std::cout << "INFO: HikingSimulator initialized successfully." << std::endl;
    return true;
}

void HikingSimulator::setupMatrices() {
    updateProjectionMatrix();
    updateViewMatrix();
}

void HikingSimulator::updateViewMatrix() {
    float hScale = terrain.getHorizontalScale();
    float terrainWidth = width * hScale;
    float terrainDepth = height * hScale;
    float maxTerrainHeight = terrain.getMaxHeight();

    switch (cameraMode) {
    case CameraMode::OVERVIEW: {
        float viewDistance = std::max(terrainWidth, terrainDepth) * 0.5f;
        float viewHeight = maxTerrainHeight * 2.0f;

        // Update camera position to orbit around terrain center
        float radians = glm::radians(orbitAngle);
        cameraPosition = glm::vec3(
            sin(radians) * viewDistance,
            viewHeight,
            cos(radians) * viewDistance
        );

        glm::vec3 target(0.0f, 0.0f, 0.0f); // Look at the terrain center
        viewMatrix = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }
    case CameraMode::FOLLOW: {
        glm::vec3 characterPos = animatedCharacter.getCurrentPosition();
        float cameraHeight = 10.0f; 
        float cameraDistance = 20.0f; 

        // Get the forward direction of the character
        glm::vec3 forwardDir = animatedCharacter.getForwardDirection();

        // Calculate the camera position behind the character
        glm::vec3 desiredPosition = characterPos - forwardDir * cameraDistance + glm::vec3(0.0f, cameraHeight, 0.0f);

        // Ensure the camera stays above the terrain
        float terrainHeightAtCamera = terrain.getHeightAtPosition(desiredPosition.x, desiredPosition.z);
        desiredPosition.y = std::max(desiredPosition.y, terrainHeightAtCamera + 2.0f); 

        cameraPosition = desiredPosition;

        glm::vec3 target = characterPos;
        viewMatrix = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }
    case CameraMode::FIRST_PERSON: {
        glm::vec3 characterPos = animatedCharacter.getCurrentPosition();
        cameraPosition = characterPos + glm::vec3(0.0f, 2.0f, 0.0f);
        viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        break;
    }
    }
}


//Defines how the 3D scene is projected onto a 2D screen
void HikingSimulator::updateProjectionMatrix() {
    float aspectRatio = windowWidth / windowHeight;
    float verticalFOV = 50.0f;  // Wider Field of View for better coverage
    float hScale = terrain.getHorizontalScale();
    float viewDistance = std::max(width * hScale, height * hScale);

    projectionMatrix = glm::perspective(
        glm::radians(verticalFOV),//Converts the field of view from degrees to radians
        aspectRatio,
        0.1f, //0.1f ensures nearby objects are rendered without clipping.
        viewDistance * 3.0f //camera can see beyond the terrain’s maximum extent.
    );
}

void HikingSimulator::processInput(GLFWwindow* window, float deltaTime) {
    // Camera mode toggles
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        cameraMode = CameraMode::OVERVIEW;
        isMouseEnabled = false;
        firstMouse = true;
        updateViewMatrix();
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        cameraMode = CameraMode::FOLLOW;
        isMouseEnabled = false;
        firstMouse = true;
        updateViewMatrix();
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        cameraMode = CameraMode::FIRST_PERSON;
        isMouseEnabled = true;
        firstMouse = true;
        updateViewMatrix();
    }

    // Toggle rain effect with 'T' key
    static bool rainTogglePressed = false; // Local static variable to handle key press state

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!rainTogglePressed) {
            isRaining = !isRaining; // Toggle rain
            rainTogglePressed = true; // Prevent multiple toggles on a single key press

            if (isRaining) {
                Skybox::getInstance().cleanup();
                if (!Skybox::getInstance().initialize("textures/cloudySkyBox/")) {
                    std::cerr << "ERROR: Failed to initialize cloudy skybox!" << std::endl;
                }
            }
            else {
                Skybox::getInstance().cleanup();
                if (!Skybox::getInstance().initialize("textures/skybox/")) {
                    std::cerr << "ERROR: Failed to initialize clear skybox!" << std::endl;
                }
            }
        }
    }
    else {
        rainTogglePressed = false; // Reset when the key is released
    }

    // Other controls
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        animatedCharacter.resetHike();
        hiker.resetPath();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        isMouseEnabled = !isMouseEnabled;
        firstMouse = true;
    }

    // Update positions
    animatedCharacter.updatePosition(deltaTime, terrain);

    updateViewMatrix();
}


void HikingSimulator::processMouseMovement(float xpos, float ypos) {
    if (!isMouseEnabled) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void HikingSimulator::render(float deltaTime) {
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Update orbitAngle for camera rotation
    if (cameraMode == CameraMode::OVERVIEW) {
        orbitAngle += orbitSpeed * deltaTime; 
        if (orbitAngle >= 360.0f) {
            orbitAngle -= 360.0f;
        }
        updateViewMatrix();
    }

    // allow the skybox to render correctly even at maximum depth.
    glDepthFunc(GL_LEQUAL);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMatrix)); // Remove translation

    //Renders the skybox using the modified view matrix and the current projection matrix
    Skybox::getInstance().render(skyboxView, projectionMatrix);

    //Restores default depth comparison for subsequent rendering
    glDepthFunc(GL_LESS);

    // Enables back-face culling to improve performance by not rendering back-facing polygons.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Setup terrain shader
    Shader& terrainShader = terrain.getShader();
    terrainShader.use();

    // Set light properties
    terrainShader.setVec3("light.position", lighting.getPosition());
    terrainShader.setVec3("light.color", glm::vec3(1.0f));
    terrainShader.setVec3("light.ambient", glm::vec3(0.3f));
    terrainShader.setVec3("light.diffuse", glm::vec3(0.7f));
    terrainShader.setVec3("light.specular", glm::vec3(0.5f));

    // Set material properties
    terrainShader.setVec3("material.ambient", glm::vec3(0.3f, 0.4f, 0.3f));
    terrainShader.setVec3("material.diffuse", glm::vec3(0.4f, 0.6f, 0.4f));
    terrainShader.setVec3("material.specular", glm::vec3(0.2f));
    terrainShader.setFloat("material.shininess", 16.0f);

    terrainShader.setVec3("viewPos", cameraPosition);

    // Set matrices
    terrainShader.setMat4("model", modelMatrix);
    terrainShader.setMat4("view", viewMatrix);
    terrainShader.setMat4("projection", projectionMatrix);

    // Render terrain
    terrain.render(modelMatrix, viewMatrix, projectionMatrix, cameraPosition);

    // Disable face culling for transparent objects
    glDisable(GL_CULL_FACE);

    // Render path
    if (pathShader && pathShader->isLoaded()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        pathShader->use();
        pathShader->setMat4("model", modelMatrix);
        pathShader->setMat4("view", viewMatrix);
        pathShader->setMat4("projection", projectionMatrix);
        pathShader->setVec3("pathColor", glm::vec3(1.0f, 0.0f, 0.0f));

        hiker.renderPath(viewMatrix, projectionMatrix, *pathShader);

        glDisable(GL_BLEND);
    }

    // Render animated character
    if (characterShader && characterShader->isLoaded()) {
        characterShader->use();

        characterShader->setVec3("lightPosition", lighting.getPosition());
        characterShader->setVec3("lightColor", glm::vec3(1.0f));

        characterShader->setVec3("materialAmbient", glm::vec3(0.2f, 0.2f, 0.2f));
        characterShader->setVec3("materialDiffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        characterShader->setVec3("materialSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
        characterShader->setFloat("materialShininess", 32.0f);

        // Set camera/view position
        characterShader->setVec3("viewPos", cameraPosition);

        // Set matrices
        characterShader->setMat4("view", viewMatrix);
        characterShader->setMat4("projection", projectionMatrix);

        // Render the animated character
        animatedCharacter.render(viewMatrix, projectionMatrix, *characterShader);
    }


    // Render the trace
    if (traceShader && traceShader->isLoaded()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        animatedCharacter.renderTrace(viewMatrix, projectionMatrix, *traceShader);

        glDisable(GL_BLEND);
    }

    // Update and render rain particle system if rain effect is active
    if (isRaining) {
        rainParticleSystem.update(deltaTime, cameraPosition, terrain);
        rainParticleSystem.render(viewMatrix, projectionMatrix);
        std::cout << "INFO: Rendering rain particles." << std::endl;
    }
}

void HikingSimulator::cleanup() {
    terrain.cleanup();
    hiker.cleanup();
    animatedCharacter.cleanup();
    rainParticleSystem.cleanup();
    Skybox::getInstance().cleanup();
    std::cout << "INFO: HikingSimulator cleaned up successfully." << std::endl;
}
