// AnimatedCharacter.h

#ifndef ANIMATEDCHARACTER_H
#define ANIMATEDCHARACTER_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"
#include "Terrain.h"

class AnimatedCharacter {
public:
    // Constructor and Destructor
    AnimatedCharacter();
    ~AnimatedCharacter();

    // Initialization
    void loadPathData(const std::vector<glm::vec3>& path);

    // Update and Render
    void updatePosition(float deltaTime, const Terrain& terrain);
    void render(const glm::mat4& view, const glm::mat4& projection, Shader& shader);
    void renderTrace(const glm::mat4& view, const glm::mat4& projection, Shader& shader);

    // Reset
    void resetHike();

    // Cleanup
    void cleanup();

    // Accessors
    glm::vec3 getCurrentPosition() const;
    glm::vec3 getForwardDirection() const;
    bool isSimulationStarted() const;
    bool isSimulationFinished() const;


private:
    // Member variables
    unsigned int characterVAO, characterVBO;
    unsigned int traceVAO, traceVBO;
    glm::vec3 characterPosition;
    glm::vec3 previousPosition;
    std::vector<glm::vec3> pathPoints;
    float progress;
    size_t currentPathIndex;
    float movementSpeed;    // Base movement speed
    float characterScale;
    float currentSpeed;
    bool simulationStarted;
    bool simulationFinished;

    // Trace data
    std::vector<glm::vec3> tracePositions;
    std::vector<float> traceSpeeds;

    // Private methods
    void setupCharacterBuffers();
    void setupTraceBuffers();
    glm::vec3 getColorBasedOnSpeed(float speed);
};

#endif // ANIMATEDCHARACTER_H
