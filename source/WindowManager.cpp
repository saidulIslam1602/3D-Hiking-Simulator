#include "WindowManager.h"
#include <iostream>

// Constructor for the WindowManager class
WindowManager::WindowManager(int width, int height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        // Log an error message and exit if GLFW initialization fails
        std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configure GLFW to use OpenGL version 3.3 and core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Major version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Minor version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile for modern OpenGL

    // Create a GLFW window with the specified dimensions and title
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        // Log an error and terminate if the window creation fails
        std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // Make the created window the current OpenGL context
    glfwMakeContextCurrent(window);

    // Set the framebuffer size callback to adjust the viewport when the window is resized
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLAD to load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // Log an error, destroy the window, and terminate if GLAD fails to initialize
        std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Enable depth testing in OpenGL for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Retrieve the framebuffer dimensions
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    // Set the OpenGL viewport to match the framebuffer dimensions
    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

// Destructor for the WindowManager class
WindowManager::~WindowManager() {
    // Destroy the GLFW window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Check if the window should close (e.g., user clicked the close button)
bool WindowManager::shouldClose() {
    return glfwWindowShouldClose(window);
}

// Swap the front and back buffers for double buffering
void WindowManager::swapBuffers() {
    glfwSwapBuffers(window);
}

// Poll for and process events (e.g., keyboard, mouse input)
void WindowManager::pollEvents() {
    glfwPollEvents();
}

// Return the pointer to the GLFW window
GLFWwindow* WindowManager::getWindow() const {
    return window;
}

// Callback function to handle framebuffer size changes (e.g., when the window is resized)
void WindowManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // If either dimension is zero, do nothing to avoid setting a zero-sized viewport
    if (width == 0 || height == 0) {
        return;
    }
    // Adjust the OpenGL viewport to the new window dimensions
    glViewport(0, 0, width, height);
}
