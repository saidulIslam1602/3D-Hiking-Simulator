// Hiker.h
#ifndef HIKER_H
#define HIKER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Terrain.h"
#include "Shader.h"

class Hiker {
public:
    Hiker(const std::string& pathFile);

    bool loadPathData(const Terrain& terrain);
    void setScales(float hScale, float vScale);
    void setTerrain(const Terrain* terrain);

    //void moveForward(float deltaTime);
    //void moveBackward(float deltaTime);
    void resetPath();
    //void updatePosition(float deltaTime, const Terrain& terrain);
    void renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader);
    void cleanup();

    glm::vec3 getPosition() const;
    const std::vector<glm::vec3>& getPathPoints() const;

private:
    void validatePath(const Terrain& terrain);
    void setupPathVAO();

    std::string pathFile;
    std::vector<glm::vec3> pathPoints;
    GLuint pathVAO;
    GLuint pathVBO;
    glm::vec3 currentPosition;
    float progress;
    size_t currentPathIndex;
    float horizontalScale;
    float heightScale;
    const Terrain* terrainRef;
};

#endif // HIKER_H
