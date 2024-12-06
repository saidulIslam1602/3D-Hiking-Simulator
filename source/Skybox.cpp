// Skybox.cpp

#include "Skybox.h"
#include "../Linker/include/stb/stb_image.h"
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//ensure only one instance of the skybox in program
Skybox* Skybox::instance = nullptr;


Skybox& Skybox::getInstance() {
    if (!instance) { //ensures no skybox object created
        instance = new Skybox(); 
    }
    return *instance;
}

// Constructor
Skybox::Skybox()
    : VAO(0), VBO(0), cubemapTexture(0), cubemapLoaded(false),
    skyboxShader("shaders/skyboxVert.glsl", "shaders/skyboxFrag.glsl") {}

// Destructor
Skybox::~Skybox() {
    cleanup();
}

bool Skybox::initialize(const std::string& directory) {
    // Cleanup existing resources if any
    if (cubemapLoaded) {
        cleanup(); //avoid memory leaks or duplicate data
    }

    std::cout << "INFO: Initializing Skybox VAO, VBO, and cubemap textures." << std::endl;

    // Validate and fix the directory path
    std::string correctedDirectory = directory;
    if (directory.back() != '/' && directory.back() != '\\') {
        correctedDirectory += '/';
    }

    // Define the cubemap faces in the correct order
    std::vector<std::string> faces = {
        correctedDirectory + "px.png", // Right
        correctedDirectory + "nx.png", // Left
        correctedDirectory + "py.png", // Top
        correctedDirectory + "ny.png", // Bottom
        correctedDirectory + "pz.png", // Front
        correctedDirectory + "nz.png"  // Back
    };

    // Debugging: Print paths to verify correctness
    std::cout << "INFO: Constructed cubemap faces paths:" << std::endl;
    for (const auto& face : faces) {
        std::cout << face << std::endl;
    }

    // Load the cubemap textures and bind them to texture object
    cubemapTexture = loadCubemap(faces);
    if (cubemapTexture == 0) {
        std::cerr << "ERROR: Failed to load cubemap textures. Skybox initialization aborted." << std::endl;
        return false;
    }

    // Define skybox vertices (cube)
    float skyboxVertices[] = {
        // Positions          
        // Back face
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,

         // Front face
         -1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,

         -1.0f, -1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,

         // Left face
         -1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,

         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,

         // Right face
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,

          1.0f, -1.0f, -1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,

          // Bottom face
          -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,

          -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f,  1.0f,

          // Top face
          -1.0f,  1.0f, -1.0f,
           1.0f,  1.0f , 1.0f,
           1.0f,  1.0f, -1.0f,

          -1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f
    };

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &VAO); //storing vertex attribute configuaraion
    glGenBuffers(1, &VBO); //store skyboxIndices

    glBindVertexArray(VAO); //activation VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // VBO to Gl array buffer target
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW); //updates the vertex to GPU

    // Position attribute
    glEnableVertexAttribArray(0);
    //index, size(x,y,z), type,no normalization, 3 floats per vertex, offfset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0); 

    // Verify if shader is loaded
    if (!skyboxShader.isLoaded()) {
        std::cerr << "ERROR: Skybox shader failed to load." << std::endl;
        return false;
    }

    // Set the texture unit for the skybox shader
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    cubemapLoaded = true;
    std::cout << "INFO: Skybox initialized successfully." << std::endl;
    return true;
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!cubemapLoaded) return;

    glDepthFunc(GL_LEQUAL); //allows fragments to render if their depth is less than or equal to the depth buffer value.
    skyboxShader.use();

    // Remove translation from the view matrix, stopping moving skybox with camera
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

    // Set uniforms
    skyboxShader.setMat4("view", skyboxView); //orient the skybox
    skyboxShader.setMat4("projection", projection); //ensured perspective
     
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0); //Activates texture unit 0 for subsequent texture binding.
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture); //Binds the cubemap texture to the active texture unit
    glDrawArrays(GL_TRIANGLES, 0, 36); //geometry shape, index, number of vertices
    glBindVertexArray(0); //accidentally modifications

    glDepthFunc(GL_LESS); //restoreDefaultDepthFunction
}


void Skybox::cleanup() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (cubemapTexture) {
        glDeleteTextures(1, &cubemapTexture);
        cubemapTexture = 0;
    }
    cubemapLoaded = false;
    std::cout << "INFO: Skybox resources cleaned up." << std::endl;
}


GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    // Create a GLuint to hold the texture ID
    GLuint textureID;

    // Generate a texture object for the cubemap
    glGenTextures(1, &textureID);

    // Bind the generated texture ID to the cubemap target
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Variables to store image width, height, and number of channels
    int width, height, nrChannels;

    // Ensure the images are not flipped vertically (necessary for cubemaps)
    stbi_set_flip_vertically_on_load(false);

    // Load each face of the cubemap
    for (GLuint i = 0; i < faces.size(); i++) {
        // Load the image data for the current face
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            // Determine the image format based on the number of channels
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED; // Grayscale image
            else if (nrChannels == 3)
                format = GL_RGB; // RGB image
            else if (nrChannels == 4)
                format = GL_RGBA; // RGBA image
            else {
                // If the number of channels is unknown, log an error and clean up
                std::cerr << "ERROR: Unknown number of channels in texture: " << faces[i] << std::endl;
                stbi_image_free(data); // Free the loaded image data
                glDeleteTextures(1, &textureID); // Delete the texture object
                return 0; // Return 0 to indicate failure
            }

            // Assign the loaded image to the appropriate face of the cubemap
            // Parameters:
            // - GL_TEXTURE_CUBE_MAP_POSITIVE_X + i: Specifies the face of the cubemap
            // - 0: Level of detail (base level)
            // - format: Internal format of the texture
            // - width, height: Dimensions of the texture
            // - 0: Border (must be 0)
            // - format: Format of the pixel data
            // - GL_UNSIGNED_BYTE: Data type of the pixel data
            // - data: Pointer to the pixel data
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            // Free the CPU-side image data now that it is uploaded to the GPU
            stbi_image_free(data);
        }
        else {
            // Log an error if the texture failed to load, with the failure reason
            std::cerr << "ERROR: Cubemap texture failed to load at path: " << faces[i]
                << " with reason: " << stbi_failure_reason() << std::endl;

            // Free any partially loaded texture and return 0
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }
    }

    // Set texture parameters for the cubemap
    // Minification filter: Linear filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Magnification filter: Linear filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap mode for the S (horizontal) axis: Clamp texture coordinates to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    // Wrap mode for the T (vertical) axis: Clamp texture coordinates to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Wrap mode for the R (depth) axis: Clamp texture coordinates to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Return the generated cubemap texture ID
    return textureID;
}
