#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>


class TextureLoader {
public:
    static GLuint loadTexture(const char* path);
    static GLuint loadCubemap(const std::vector<std::string>& faces);
};
