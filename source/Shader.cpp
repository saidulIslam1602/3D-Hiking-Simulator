// Shader.cpp

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

// Debugging flag
constexpr bool SHADER_DEBUG = true;

// Constructor
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : programID(0), loaded(false) {
    if (SHADER_DEBUG) {
        std::cout << "INFO::SHADER::CREATING_SHADER: Vertex(" << vertexPath
            << ") Fragment(" << fragmentPath << ")\n";
    }

    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_LOADED_CORRECTLY\n";
        return;
    }

    GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    if (!vertexShader || !fragmentShader) {
        std::cerr << "ERROR::SHADER::SHADER_CREATION_FAILED\n";
        return;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    checkCompileErrors(programID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    loaded = true;
    if (SHADER_DEBUG) {
        std::cout << "INFO::SHADER::PROGRAM_CREATED_SUCCESSFULLY\n";
    }
}

// Destructor
Shader::~Shader() {
    if (programID > 0) {
        glDeleteProgram(programID);
    }
}

// Use shader
void Shader::use() const {
    if (programID > 0) {
        glUseProgram(programID);
    }
    else {
        std::cerr << "ERROR::SHADER::PROGRAM_NOT_INITIALIZED\n";
    }
}

// Accessor for programID
GLuint Shader::getProgramID() const {
    return programID;
}

// Accessor for loaded flag
bool Shader::isLoaded() const {
    return loaded;
}

// Set uniform 4x4 matrix (view and projection, matrix)
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        //uniform loc, number of matrice, no transposition, convert mat into a raw pointer
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

// Set uniform vec3 (lightColor and lightPos, value)
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        //uniform loc, number of vectors, converts into a pointer
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

// Set uniform vec2
void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

// Set uniform float
void Shader::setFloat(const std::string& name, float value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

// Set uniform int
void Shader::setInt(const std::string& name, int value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

std::string Shader::loadShaderSource(const std::string& filepath) {

    // Create an input file stream to read the shader source file
    std::ifstream file(filepath);
    // Create a string stream buffer to store the file contents
    std::stringstream buffer;

    // Check if the file was successfully opened
    if (file.is_open()) {
        // Read the entire contents of the file into the buffer
        buffer << file.rdbuf();

        // If debugging is enabled, log the success of loading the shader file
        if (SHADER_DEBUG) {
            std::cout << "INFO::SHADER::LOADED_SOURCE_FROM: " << filepath << "\n";
        }
    }
    else {
        // If the file could not be opened, log an error message
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filepath << "\n";
        // Return an empty string to indicate failure
        return "";
    }

    // Convert the contents of the buffer to a std::string and return it
    return buffer.str();
}


// Compile shader
GLuint Shader::compileShader(const char* source, GLenum type) {

    //vertex or fragment
    GLuint shader = glCreateShader(type);

    //shaderObject, numberOfSourceCodeString, nullTerminatedStrings
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];

        //// Retrieve the error log from OpenGL
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
            << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n"
            << infoLog << "\n";
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Check errors
void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    // Variable to store the success status (compile or link status)
    GLint success;
    // Create a buffer to store error or warning logs (1024 characters max)
    std::vector<GLchar> infoLog(1024);

    // If the type is not "PROGRAM", check for shader compilation errors
    if (type != "PROGRAM") {
        // Get the compilation status of the shader
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        // If the shader compilation failed
        if (!success) {
            // Retrieve the error log for the shader
            glGetShaderInfoLog(
                shader,
                static_cast<GLsizei>(infoLog.size()), // Maximum size of the log
                nullptr,                              // Length of the actual log (not needed here)
                infoLog.data()                        // Pointer to the log data
            );

            // Output the shader compilation error with its type (e.g., VERTEX or FRAGMENT)
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog.data() << "\n";
        }
    }
    // If the type is "PROGRAM", check for program linking errors
    else {
        // Get the link status of the program
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        // If the program linking failed
        if (!success) {
            // Retrieve the error log for the program
            glGetProgramInfoLog(
                shader,
                static_cast<GLsizei>(infoLog.size()), // Maximum size of the log
                nullptr,                              // Length of the actual log (not needed here)
                infoLog.data()                        // Pointer to the log data
            );

            // Output the program linking error
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n"
                << infoLog.data() << "\n";
        }
    }
}


// Get uniform location
GLint Shader::getUniformLocation(const std::string& name) const {
    // Check if the uniform location is already cached
    auto it = uniformCache.find(name);
    if (it != uniformCache.end()) {
        // If the uniform is found in the cache, return the cached location
        return it->second;
    }

    // Query OpenGL for the location of the uniform variable in the shader program
    GLint location = glGetUniformLocation(programID, name.c_str());

    // If the uniform is not found in the shader program
    if (location == -1 && SHADER_DEBUG) {
        // Log a warning message for debugging purposes
        std::cerr << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << "\n";
    }

    // Cache the uniform location for future use
    uniformCache[name] = location;

    // Return the uniform location (or -1 if not found)
    return location;
}


void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}