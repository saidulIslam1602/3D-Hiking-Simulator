// TextureLoader.cpp

#include "TextureLoader.h"
#include "../Linker/include/stb/stb_image.h"
#include <iostream>

GLuint TextureLoader::loadTexture(const char* path) {
    // Create a texture ID and generate one OpenGL texture object
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Variables to store texture dimensions and number of color components
    int width, height, nrComponents;

    // Load the texture data using stb_image library
    // Parameters:
    // - path: Path to the texture file
    // - width, height: Output variables for texture dimensions
    // - nrComponents: Output variable for number of color components (e.g., RGB, RGBA)
    // - 0: Force stb_image to keep the original number of components
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    // Check if the texture data was successfully loaded
    if (data) {
        // Determine the texture format based on the number of components
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED; // Grayscale
        else if (nrComponents == 3)
            format = GL_RGB; // RGB format
        else if (nrComponents == 4)
            format = GL_RGBA; // RGBA format

        // Handle unsupported texture formats
        if (format == 0) {
            std::cerr << "ERROR: Unknown number of channels in texture: " << path << std::endl;
            stbi_image_free(data); // Free the texture data
            glDeleteTextures(1, &textureID); // Delete the generated texture
            return 0; // Return 0 to indicate failure
        }

        // Bind the texture object to the GL_TEXTURE_2D target
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Specify the texture image using the loaded data
        // Parameters:
        // - GL_TEXTURE_2D: Target texture type
        // - 0: Level of detail (0 for the base level)
        // - format: Internal format of the texture
        // - width, height: Dimensions of the texture
        // - 0: Border (must be 0)
        // - format: Format of the pixel data
        // - GL_UNSIGNED_BYTE: Data type of the pixel data
        // - data: Pointer to the pixel data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Generate mipmaps for the texture
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping parameters for the S (horizontal) axis
        // Use GL_CLAMP_TO_EDGE for RGBA textures to avoid artifacts
        // Use GL_REPEAT for other formats
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        // Set texture wrapping parameters for the T (vertical) axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
            format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        // Set texture filtering parameters
        // GL_LINEAR_MIPMAP_LINEAR: Use trilinear filtering for minification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // GL_LINEAR: Use bilinear filtering for magnification
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Free the texture data as it is now stored in the GPU memory
        stbi_image_free(data);
    }
    else {
        // Log an error message if the texture could not be loaded
        std::cout << "Texture failed to load at path: " << path << std::endl;
        // Free the data pointer (even though it's null, this is safe)
        stbi_image_free(data);
    }

    // Return the generated texture ID
    return textureID;
}



GLuint TextureLoader::loadCubemap(const std::vector<std::string>& faces) {
    // Generate a texture ID and create an OpenGL texture object
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind the texture object to the cube map target
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Variables to store texture dimensions and channel information
    int width, height, nrChannels;

    // Ensure textures are not flipped vertically for cubemaps
    stbi_set_flip_vertically_on_load(false);

    // Load each texture face of the cubemap
    for (GLuint i = 0; i < faces.size(); i++) {
        // Load the texture data for the current face
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            // Determine the format of the texture based on the number of channels
            GLenum format = 0;
            if (nrChannels == 1)
                format = GL_RED; // Grayscale
            else if (nrChannels == 3)
                format = GL_RGB; // RGB format
            else if (nrChannels == 4)
                format = GL_RGBA; // RGBA format

            // Handle unsupported formats
            if (format == 0) {
                std::cerr << "ERROR: Unknown number of channels in cubemap texture: " << faces[i] << std::endl;
                stbi_image_free(data); // Free the texture data
                glDeleteTextures(1, &textureID); // Delete the texture object
                return 0; // Return 0 to indicate failure
            }

            // Upload the texture data to the specific cubemap face
            // Parameters:
            // - GL_TEXTURE_CUBE_MAP_POSITIVE_X + i: Target cubemap face (e.g., +X, -X, +Y, etc.)
            // - 0: Mipmap level (base level)
            // - format: Internal format of the texture
            // - width, height: Dimensions of the texture
            // - 0: Border (must be 0)
            // - format: Format of the pixel data
            // - GL_UNSIGNED_BYTE: Data type of the pixel data
            // - data: Pointer to the pixel data
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                format, width, height, 0, format,
                GL_UNSIGNED_BYTE, data);

            // Free the CPU-side texture data after uploading to the GPU
            stbi_image_free(data);
        }
        else {
            // Log an error if the texture data could not be loaded
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;

            // Free any partially loaded texture and return 0
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }
    }

    // Set texture parameters for the cubemap
    // Minification filter: Linear filtering with mipmaps
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Magnification filter: Linear filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrap mode for S axis: Clamp texture coordinates to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    // Wrap mode for T axis: Clamp texture coordinates to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Wrap mode for R axis: Clamp texture coordinates to the edge
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Return the generated cubemap texture ID
    return textureID;
}
