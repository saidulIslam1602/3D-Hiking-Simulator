// Hiker.cpp
#include "Hiker.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

Hiker::Hiker(const std::string& pathFile)
    : pathFile(pathFile), pathVAO(0), pathVBO(0), currentPosition(glm::vec3(0.0f)),
    progress(0.0f), currentPathIndex(0),
    horizontalScale(1.0f), heightScale(1.0f), terrainRef(nullptr) {
}

void Hiker::setScales(float hScale, float vScale) {
    horizontalScale = hScale;
    heightScale = vScale;
}

void Hiker::setTerrain(const Terrain* terrain) {
    terrainRef = terrain;
}

bool Hiker::loadPathData(const Terrain& terrain) {
    std::ifstream file(pathFile);
    if (!file.is_open()) {
        std::cerr << "ERROR: Failed to open path file: " << pathFile << std::endl;
        return false;
    }

    float x, y, z;
    pathPoints.clear();

    // Load raw path points
    while (file >> x >> y >> z) {
        pathPoints.emplace_back(x, y, z);
    }
    file.close();

    if (pathPoints.empty()) {
        std::cerr << "ERROR: No path points loaded from file: " << pathFile << std::endl;
        return false;
    }

    validatePath(terrain);
    currentPosition = pathPoints[0];
    setupPathVAO();
    return true;
}

void Hiker::validatePath(const Terrain& terrain) {
    if (pathPoints.empty()) return;

    // Find min and max of path points
    glm::vec2 minPoint(FLT_MAX, FLT_MAX);
    glm::vec2 maxPoint(-FLT_MAX, -FLT_MAX);

    for (const auto& point : pathPoints) {
        minPoint.x = std::min(minPoint.x, point.x);
        minPoint.y = std::min(minPoint.y, point.z);
        maxPoint.x = std::max(maxPoint.x, point.x);
        maxPoint.y = std::max(maxPoint.y, point.z);
    }

    // Compute path range
    glm::vec2 pathRange = maxPoint - minPoint;

    // Terrain dimensions, terrain.getHorizontalScale() scales the grid size to the actual world dimensions
    float terrainWidth = terrain.getWidth() * terrain.getHorizontalScale(); //x dimension
    float terrainDepth = terrain.getHeight() * terrain.getHorizontalScale(); //z dimension

    // Scaling factors to fit path to terrain
    float scaleX = terrainWidth / pathRange.x;
    float scaleZ = terrainDepth / pathRange.y;
    float scale = std::min(scaleX, scaleZ) * 0.8f; //multiplying by 0.8f to avoid edge overlap.

    // Adjust path points
    for (auto& point : pathPoints) {
        // Normalize to [0, 1]
        point.x = (point.x - minPoint.x) / pathRange.x;
        point.z = (point.z - minPoint.y) / pathRange.y;

        // Scale to terrain size
        point.x = point.x * terrainWidth - terrainWidth * 0.5f;
        point.z = point.z * terrainDepth - terrainDepth * 0.5f;

        // Update Y position based on terrain height
        float terrainHeight = terrain.getHeightAtPosition(point.x, point.z);
        point.y = terrainHeight + 0.5f; // Slight offset above terrain
    }

    
    std::vector<glm::vec3> smoothedPath; //stored smooth version of the path
    for (size_t i = 0; i < pathPoints.size() - 1; ++i) {
        glm::vec3 start = pathPoints[i];
        glm::vec3 end = pathPoints[i + 1];
        smoothedPath.push_back(start);

        int segments = 5; // Number of intermediate points
        for (int j = 1; j < segments; ++j) {
            float t = static_cast<float>(j) / segments;
            glm::vec3 intermediatePoint = glm::mix(start, end, t);

            // Update Y position based on terrain height
            float terrainHeight = terrain.getHeightAtPosition(intermediatePoint.x, intermediatePoint.z);
            intermediatePoint.y = terrainHeight + 0.5f;
            smoothedPath.push_back(intermediatePoint);
        }
    }
    smoothedPath.push_back(pathPoints.back());
    pathPoints = std::move(smoothedPath);
}

void Hiker::setupPathVAO() {
    if (pathVAO != 0) {
        glDeleteVertexArrays(1, &pathVAO);
        glDeleteBuffers(1, &pathVBO);
    }

    glGenVertexArrays(1, &pathVAO); //The VAO will record how the vertex data is structured and used.
    glGenBuffers(1, &pathVBO); //The VBO will hold the actual path point data.

    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO); //Transfer the path data from the CPU to the GPU
    glBufferData(GL_ARRAY_BUFFER, pathPoints.size() * sizeof(glm::vec3), pathPoints.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}


void Hiker::resetPath() {
    if (!pathPoints.empty()) {
        currentPathIndex = 0;
        progress = 0.0f;
        currentPosition = pathPoints[0];
    }
}


void Hiker::renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("model", glm::mat4(1.0f));

    glDisable(GL_DEPTH_TEST); //Disables depth testing so the path is always visible

    // Draw path
    glLineWidth(4.0f);
    shader.setVec3("pathColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Red color
    glBindVertexArray(pathVAO);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(pathPoints.size()));

    glEnable(GL_DEPTH_TEST); //ensuring normal depth behavior for other objects in the scene.
    glLineWidth(1.0f); // avoid affecting future OpenGL line rendering.
    glBindVertexArray(0);
}

void Hiker::cleanup() {
    if (pathVAO != 0) {
        glDeleteVertexArrays(1, &pathVAO);
        glDeleteBuffers(1, &pathVBO);
        pathVAO = 0;
        pathVBO = 0;
    }
    pathPoints.clear();
}

glm::vec3 Hiker::getPosition() const {
    return currentPosition;
}

const std::vector<glm::vec3>& Hiker::getPathPoints() const {
    return pathPoints;
}
