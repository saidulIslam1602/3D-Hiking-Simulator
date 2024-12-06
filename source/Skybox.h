// Skybox.h

#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"
#include <glad/glad.h>


class Skybox {
public:
   
    static Skybox& getInstance();
    bool initialize(const std::string& directory);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void cleanup();

private:
    // Private Constructor and Destructor for Singleton
    Skybox();
    ~Skybox();

    // Delete copy constructor and assignment operator
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    // Member Variables
    GLuint VAO, VBO;              ///< Vertex Array Object and Vertex Buffer Object.
    GLuint cubemapTexture;        ///< Cubemap Texture ID.
    bool cubemapLoaded;           ///< Flag indicating if the cubemap was loaded successfully.
    Shader skyboxShader;          ///< Shader program for the Skybox.

    static Skybox* instance;      ///< Singleton instance.

    GLuint loadCubemap(const std::vector<std::string>& faces);
};

#endif // SKYBOX_H

