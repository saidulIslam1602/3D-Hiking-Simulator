#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shader.h"

class Terrain {
public:
    Terrain();
    bool loadTerrainData(const std::string& texturePath);
    void render(const glm::mat4& model, const glm::mat4& view,
    const glm::mat4& projection, const glm::vec3& cameraPosition);
    void cleanup();

    int getWidth() const;
    int getHeight() const;
    float getHeightScale() const;
    float getHorizontalScale() const;
    float getMaxHeight() const;
    float getHeightAtPosition(float x, float z) const;
    Shader& getShader();

    void setHeightScale(float scale);
    void setHorizontalScale(float scale);

private:
    void calculateNormals();
    void setupTerrainVAO();

    Shader terrainShader;
    GLuint terrainVAO;
    GLuint terrainVBO;
    GLuint terrainEBO;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    std::vector<float> heights;

    int width;
    int height;
    float heightScale;
    float horizontalScale;
    float maxHeight;
};