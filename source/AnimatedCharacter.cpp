// AnimatedCharacter.cpp

#include "AnimatedCharacter.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructor
AnimatedCharacter::AnimatedCharacter()
    : characterVAO(0), characterVBO(0), traceVAO(0), traceVBO(0),
    characterPosition(0.0f), previousPosition(0.0f),
    progress(0.0f), currentPathIndex(0),
    movementSpeed(15.0f), // Base movement speed
    characterScale(1.0f),
    currentSpeed(0.0f),
    simulationStarted(false),
    simulationFinished(false) {
}


//destructor_release OpenGL resources when the object is destroyed
AnimatedCharacter::~AnimatedCharacter() {
    cleanup();
}

// Initialize character buffers with cube vertices
void AnimatedCharacter::setupCharacterBuffers() {
    // Cube vertices (36 vertices for 12 triangles)
    std::vector<float> vertices = {
        // Positions
        // Front face
        -0.5f, -0.5f, -0.5f, // Vertex 1
         0.5f, -0.5f, -0.5f, // Vertex 2
         0.5f,  0.5f, -0.5f, // Vertex 3
         0.5f,  0.5f, -0.5f, // Vertex 4
        -0.5f,  0.5f, -0.5f, // Vertex 5
        -0.5f, -0.5f, -0.5f, // Vertex 6
        // Back face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        // Left face
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // Right face
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         // Bottom face
         -0.5f, -0.5f, -0.5f,
          0.5f, -0.5f, -0.5f,
          0.5f, -0.5f,  0.5f,
          0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f, -0.5f,
         // Top face
         -0.5f,  0.5f, -0.5f,
          0.5f,  0.5f, -0.5f,
          0.5f,  0.5f,  0.5f,
          0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f, -0.5f
    };

    //generates one vertex array object for character
    glGenVertexArrays(1, &characterVAO);
    //generates one vertex buffer object for the character
    glGenBuffers(1, &characterVBO);

    //activation of VAO to apply operations
    glBindVertexArray(characterVAO);

    //store vertex attribute data of the character buffer
    glBindBuffer(GL_ARRAY_BUFFER, characterVBO);
    
    //memory allocation for the buffer is calculated by number of elements in vertices array and the size of a single element, retrives the element of the array, data will be written once and used many times
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    /*
     0 =layout location in shader
     3= 3 components(x,y,z)
     GL_FLOAT = dataType
     GL_FALSE = data is not normalized
     3 * sizeof(float) = the stride between consecutive vertices
     (void*)0 = the offset, 0 = starting from beginning
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //passing attributes to the vertex data
    glEnableVertexAttribArray(0);

    //prevent accidental modifications to the VAO after configuration
    glBindVertexArray(0);

    std::cout << "INFO: Character buffers initialized." << std::endl;
}

//generated VAO and VBO 
void AnimatedCharacter::setupTraceBuffers() {
    glGenVertexArrays(1, &traceVAO);
    glGenBuffers(1, &traceVBO);
}


void AnimatedCharacter::loadPathData(const std::vector<glm::vec3>& path) {
    pathPoints = path; //stores the provided path point
    if (!pathPoints.empty()) {
        characterPosition = pathPoints[0]; // starting position
        previousPosition = characterPosition; //sets the previous postion
    }
    setupCharacterBuffers(); //set openGL buffer for renderings
    setupTraceBuffers();
}

void AnimatedCharacter::updatePosition(float deltaTime, const Terrain& terrain) {
    //path point validaty check
    if (pathPoints.empty() || simulationFinished) return;

    //starting simulation
    if (!simulationStarted) {
        simulationStarted = true;
        std::cout << "Simulation Started" << std::endl;
    }

    //checking the path index is greater then the pathPoint
    if (currentPathIndex >= pathPoints.size() - 1) {
        simulationFinished = true;
        std::cout << "Simulation Finished" << std::endl;
        return;
    }

    glm::vec3 start = pathPoints[currentPathIndex];
    glm::vec3 end = pathPoints[currentPathIndex + 1];

    float distance = glm::distance(start, end);

    // Ensure distance is not zero to avoid division by zero
    if (distance < 0.0001f) {
        currentPathIndex++;
        progress = 0.0f;
        return;
    }

    // Calculate slope (height difference over horizontal distance)
    float heightDiff = end.y - start.y;
    float horizontalDistance = glm::distance(glm::vec2(start.x, start.z), glm::vec2(end.x, end.z));

    //A small threshold(0.0001f) is used to avoid division by zero or extremely small numbers
    float slope = (horizontalDistance > 0.0001f) ? (heightDiff / horizontalDistance) : 0.0f;

    // Adjust speed based on slope
    float speedMultiplier;
    if (slope > 0.0f) {
        // Uphill: slower
        speedMultiplier = 1.0f / (1.0f + slope * 5.0f); //produces a value less than 0
    }
    else {
        // Downhill: faster
        speedMultiplier = 1.0f - slope * 2.0f; 
    }

    // Clamp speedMultiplier to prevent negative speeds
    speedMultiplier = glm::clamp(speedMultiplier, 0.2f, 2.0f);

    float adjustedSpeed = movementSpeed * speedMultiplier;

    //Tracks the current position along the path, ranging from 0.0 to 1.0
    progress += (adjustedSpeed * deltaTime) / distance;

    if (progress >= 1.0f) {
        progress = 0.0f; // Reset progress for the next segment
        currentPathIndex++; // Move to the next path segment
        if (currentPathIndex >= pathPoints.size() - 1) {
            simulationFinished = true;
            std::cout << "Simulation Finished" << std::endl;
            return;
        }
    }

    // mix Interpolate position between start and end based on progress
    characterPosition = glm::mix(start, end, progress);

    // Ensure character stays above terrain
    float terrainHeight = terrain.getHeightAtPosition(characterPosition.x, characterPosition.z);
    float characterOffset = characterScale * 0.5f;
    //y-position is adjusted only if it falls below the terrain + offset.
    characterPosition.y = std::max(characterPosition.y, terrainHeight + characterOffset);

    // Calculate actual movement 
    glm::vec3 movement = characterPosition - previousPosition; //character's displacement in 3D space.
    float distanceMoved = glm::length(movement);
    currentSpeed = distanceMoved / deltaTime;

    // Store position and speed
    tracePositions.push_back(characterPosition);
    traceSpeeds.push_back(currentSpeed);

    //use in the next frame's calculations.
    previousPosition = characterPosition;
}

void AnimatedCharacter::render(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {

    //shader activation
    shader.use();

    //modelMatrix = Translates the character to its position in the world.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), characterPosition);

    /*
    glfwGetTime() : Returns the elapsed time since
    time * glm::radians(50.0f): The character rotates at 50 degrees per second.
    glm::vec3(0.0f, 1.0f, 0.0f) : y axis for spinning
    */ 
    float time = static_cast<float>(glfwGetTime());
    model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));


    //uniform scale based on characterScale
    model = glm::scale(model, glm::vec3(characterScale));

    // The shader uses this matrix to transform the character’s vertices to world space.
    shader.setMat4("model", model);

    // Set object color based on speed
    glm::vec3 color = getColorBasedOnSpeed(currentSpeed);
    shader.setVec3("objectColor", color);

    // Transforms the world coordinates to the camera's view space.
    shader.setMat4("view", view);

    //Transforms the view space coordinates into normalized device coordinates
    shader.setMat4("projection", projection);

    // VAO contains all the necessary state for rendering 
    glBindVertexArray(characterVAO);

    //render the character as a set of trinangles
    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices for a cube

    //Unbinds the VAO to prevent accidental modifications.
    glBindVertexArray(0);
}

void AnimatedCharacter::renderTrace(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {

    if (tracePositions.empty()) return;

    //A static vector stores the trace's data(position and color)
    static std::vector<float> vertexData;

    //Clears any previous data in the vector.
    vertexData.clear();

    //required 3 flots for color and position respectively
    vertexData.reserve(tracePositions.size() * 6); // Reserve memory to avoid reallocations


    for (size_t i = 0; i < tracePositions.size(); ++i) {
        glm::vec3 pos = tracePositions[i]; // Gets the position of a point in the trace
        glm::vec3 color = getColorBasedOnSpeed(traceSpeeds[i]); //Calculates the color of the trace point based on its speed.

        
        //Appends each position’s x, y, z
        vertexData.push_back(pos.x);
        vertexData.push_back(pos.y);
        vertexData.push_back(pos.z);

        // Color
        vertexData.push_back(color.r);
        vertexData.push_back(color.g);
        vertexData.push_back(color.b);
    }

    //update binds the Vertex Array Object (VAO) for the trace
    glBindVertexArray(traceVAO);

    //update binds the Vertex Buffer Object(VBO)
    glBindBuffer(GL_ARRAY_BUFFER, traceVBO);

    //uploads the vertex data to the GPU Dynamically
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);

    /*
    0 =layout location in shader
    3= 3 components(x,y,z)
    GL_FLOAT = dataType
    GL_FALSE = data is not normalized
    3 * sizeof(float) = the stride between consecutive vertices
    (void*)0 = the offset, 0 = starting from beginning
   */

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Color
    glEnableVertexAttribArray(1);

    // Use shader
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // continuous line
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(tracePositions.size()));
    glBindVertexArray(0);
}

// Reset hike stats
void AnimatedCharacter::resetHike() {
    currentPathIndex = 0;
    progress = 0.0f;
    characterPosition = pathPoints.empty() ? glm::vec3(0.0f) : pathPoints[0];
    previousPosition = characterPosition;
    tracePositions.clear();
    traceSpeeds.clear();
    simulationStarted = false;
    simulationFinished = false;
}

// Cleanup OpenGL resources
void AnimatedCharacter::cleanup() {
    if (characterVAO) glDeleteVertexArrays(1, &characterVAO);
    if (characterVBO) glDeleteBuffers(1, &characterVBO);

    if (traceVAO) glDeleteVertexArrays(1, &traceVAO);
    if (traceVBO) glDeleteBuffers(1, &traceVBO);

    characterVAO = 0;
    characterVBO = 0;
    traceVAO = 0;
    traceVBO = 0;

    std::cout << "INFO: Character resources cleaned up." << std::endl;
}

glm::vec3 AnimatedCharacter::getCurrentPosition() const {
    return characterPosition;
}

glm::vec3 AnimatedCharacter::getForwardDirection() const {
    if (pathPoints.empty() || currentPathIndex >= pathPoints.size() - 1)
        return glm::vec3(0.0f, 0.0f, -1.0f); // Default forward direction

    glm::vec3 start = pathPoints[currentPathIndex];
    glm::vec3 end = pathPoints[currentPathIndex + 1];
    glm::vec3 direction = glm::normalize(end - start); //converting the vector into a unit vector, represent direction
    return glm::vec3(direction.x, 0.0f, direction.z); // Ignore Y component for horizontal rotation
}

bool AnimatedCharacter::isSimulationStarted() const {
    return simulationStarted;
}

bool AnimatedCharacter::isSimulationFinished() const {
    return simulationFinished;
}

glm::vec3 AnimatedCharacter::getColorBasedOnSpeed(float speed) {
    // Define min and max speed for mapping
    float minSpeed = movementSpeed * 0.5f; // Minimum speed (e.g., uphill)
    float maxSpeed = movementSpeed * 1.5f; // Maximum speed (e.g., downhill)

    // Normalize speed to [0,1]
    float normalizedSpeed = (speed - minSpeed) / (maxSpeed - minSpeed);
    normalizedSpeed = glm::clamp(normalizedSpeed, 0.0f, 1.0f); //Speeds below minSpeed will map to 0.0, and speeds above maxSpeed will map to 1.0.

    // Map to color gradient (blue to yellow)
    glm::vec3 slowColor(0.0f, 0.0f, 1.0f); // Blue
    glm::vec3 fastColor(1.0f, 1.0f, 0.0f); // Yellow

    return glm::mix(slowColor, fastColor, normalizedSpeed);
}


