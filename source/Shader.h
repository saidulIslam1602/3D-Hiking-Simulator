// Shader.h

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>

class Shader {
public:
    // Constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Destructor
    ~Shader();

    // Use/activate the shader
    void use() const;

    // Accessor for programID
    GLuint getProgramID() const;

    // Check if shader is loaded successfully
    bool isLoaded() const;

    // Utility uniform functions
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const; // Added setVec2 method
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
private:
    // Program ID
    GLuint programID;

    // Flag to check if shader is loaded
    bool loaded;

    // Uniform location cache
    mutable std::unordered_map<std::string, GLint> uniformCache;

    // Load shader source code
    std::string loadShaderSource(const std::string& filepath);

    // Compile shader
    GLuint compileShader(const char* source, GLenum type);

    // Check for compilation/linking errors
    void checkCompileErrors(GLuint shader, const std::string& type);

    // Get uniform location
    GLint getUniformLocation(const std::string& name) const;
};

#endif // SHADER_H
