// Terrain.cpp
#include "Terrain.h"
#include <glad/glad.h>   // Include glad first
#include "../Linker/include/stb/stb_image.h"
#include <iostream>

Terrain::Terrain()
    : terrainShader("shaders/terrainVert.glsl", "shaders/terrainFrag.glsl"),
    terrainVAO(0), terrainVBO(0), terrainEBO(0),
    width(0), height(0),
    heightScale(500.0f),
    horizontalScale(1.0f), 
    maxHeight(0.0f)
{}

bool Terrain::loadTerrainData(const std::string& texturePath) {
    int channels; //number of color channels in Terrain Image
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, STBI_grey);
    if (!data) {
        std::cerr << "ERROR: Failed to load heightmap!" << std::endl;
        return false;
    }

    vertices.clear();
    indices.clear();
    heights.resize(width * height);
    maxHeight = 0.0f;

    // Calculate base dimensions
    float scaleMultiplier = 1.0f; // Adjusted to 1.0f for consistent scaling
    float totalWidth = width * horizontalScale * scaleMultiplier;
    float totalDepth = height * horizontalScale * scaleMultiplier;

    //moves the terrain center to (0,0,0)
    glm::vec3 centerOffset(
        -(totalWidth * 0.5f),
        0.0f,
        -(totalDepth * 0.5f)
    );

    // Generate vertices with adjusted scaling
    //iterates through all rows z and columns x, each pixel is a vertex in terrain mesh
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float heightValue = data[z * width + x] / 255.0f; //normalize pixel value to 0 to 1, z*width starting row index, x is correct column
            float y = heightValue * heightScale;

            //Computes the 3D world-space position for the current vertex.
            glm::vec3 position(
                x * horizontalScale * scaleMultiplier,
                y,
                z * horizontalScale * scaleMultiplier
            );

            position += centerOffset;
            vertices.push_back(position); //Adds the vertex position to the vertices array

            //location in the 1D heights array using row - major indexing.
            heights[z * width + x] = y; //Stores the height value in the heights array.
            maxHeight = std::max(maxHeight, y); //Updates the maximum height encountered.
        }
    }

    // Generate indices for triangles for meshing
    //height-1; triangles need two rows to form
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = z * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * width + x;
            int bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    stbi_image_free(data); //Releases the memory used by the loaded heightmap image.
    calculateNormals();
    setupTerrainVAO();

    std::cout << "INFO: Terrain loaded with max height: " << maxHeight << std::endl;
    return true;
}


float Terrain::getMaxHeight() const {
    return maxHeight;
}

void Terrain::calculateNormals() {

    //Resizes the normals array to match the number of vertices
    normals.resize(vertices.size(), glm::vec3(0.0f)); //initializing zero vector for all vertices

    //The loop processes each triangle in the mesh.
    for (size_t i = 0; i < indices.size(); i += 3) {

        //idx0, idx1, idx2 correspond to the three corners of the triangle.
        //Retrieves the vertex indices for the current triangle
        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];

        //v0, v1, v2 represent the positions of the triangle's corners
        glm::vec3& v0 = vertices[idx0];
        glm::vec3& v1 = vertices[idx1];
        glm::vec3& v2 = vertices[idx2];

        //define the triangle's plane
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        //glm::cross(edge1, edge2) computes the vector perpendicular to the triangle's plane.
        //nsures the normal vector has a length of 1.
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        //Adds the triangle's normal to the normals of its three vertices
        normals[idx0] += normal;
        normals[idx1] += normal;
        normals[idx2] += normal;
    }

    //Normalizes all accumulated vertex normals
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
}

void Terrain::setupTerrainVAO() {

    //Combines vertex positions and normals into a single vertexData
    std::vector<float> vertexData;
    vertexData.reserve(vertices.size() * 6); // 3 for position, 3 for normal

    for (size_t i = 0; i < vertices.size(); ++i) {
        // Position
        vertexData.push_back(vertices[i].x);
        vertexData.push_back(vertices[i].y);
        vertexData.push_back(vertices[i].z);

        // Normal
        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);
    }

    //clean up previous data
    if (terrainVAO != 0) {
        glDeleteVertexArrays(1, &terrainVAO);
        glDeleteBuffers(1, &terrainVBO);
        glDeleteBuffers(1, &terrainEBO);
    }
    //generated VAO, VBO, EBO
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    // Update Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Upload indice data for Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    //loc,(x,y,z), dataType, not normalized, stride(pos+normal), offset
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    std::cout << "INFO: Terrain VAO setup complete." << std::endl;
}

void Terrain::render(const glm::mat4& model, const glm::mat4& view,
    const glm::mat4& projection, const glm::vec3& cameraPosition) {
    if (!terrainShader.isLoaded()) {
        std::cerr << "ERROR: Terrain shader not loaded!" << std::endl;
        return;
    }

    terrainShader.use();

    terrainShader.setMat4("model", model);
    terrainShader.setMat4("view", view);
    terrainShader.setMat4("projection", projection);
    terrainShader.setVec3("viewPos", cameraPosition);
    terrainShader.setVec3("light.color", glm::vec3(1.0f));  // Set light color

    //prepare OpenGL for vertex and indices rendaring
    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

float Terrain::getHeightAtPosition(float x, float z) const {

    // Convert world coordinates to local terrain coordinates
    //Offsets the terrain's origin to the center of the grid in world space,
    // divided by horizontalScale convert world space to terrain grid
    float localX = (x + (width * horizontalScale * 0.5f)) / horizontalScale;
    float localZ = (z + (height * horizontalScale * 0.5f)) / horizontalScale;

    // Clamp coordinates to terrain bounds
    //Limits localX and localZ to the range [0, width-1] and [0, height-1].
    localX = glm::clamp(localX, 0.0f, static_cast<float>(width - 1));
    localZ = glm::clamp(localZ, 0.0f, static_cast<float>(height - 1));

    // Get grid cell coordinates
    int x0 = static_cast<int>(localX);
    int z0 = static_cast<int>(localZ);
    int x1 = glm::min(x0 + 1, width - 1);
    int z1 = glm::min(z0 + 1, height - 1);

    // Calculate interpolation factors
    float fx = localX - x0;
    float fz = localZ - z0;

    // Get height values at grid points
    float h00 = heights[z0 * width + x0];
    float h10 = heights[z0 * width + x1];
    float h01 = heights[z1 * width + x0];
    float h11 = heights[z1 * width + x1];

    // Bilinear interpolation
    float h0 = glm::mix(h00, h10, fx);
    float h1 = glm::mix(h01, h11, fx);

    return glm::mix(h0, h1, fz);
}

void Terrain::cleanup() {
    if (terrainVAO) {
        glDeleteVertexArrays(1, &terrainVAO);
        glDeleteBuffers(1, &terrainVBO);
        glDeleteBuffers(1, &terrainEBO);
    }
    terrainVAO = 0;
    terrainVBO = 0;
    terrainEBO = 0;

    vertices.clear();
    indices.clear();
    normals.clear();
    heights.clear();
}

// Getters
int Terrain::getWidth() const { return width; }
int Terrain::getHeight() const { return height; }
Shader& Terrain::getShader() { return terrainShader; }
float Terrain::getHeightScale() const { return heightScale; }
float Terrain::getHorizontalScale() const { return horizontalScale; }

// Setters
void Terrain::setHeightScale(float scale) { heightScale = scale; }
void Terrain::setHorizontalScale(float scale) { horizontalScale = scale; }
