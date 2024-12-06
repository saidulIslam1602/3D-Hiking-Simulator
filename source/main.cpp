#include <glad/glad.h> 
#include <GLFW/glfw3.h> 
#include "WindowManager.h"
#include "HikingSimulator.h" 
#include "log.h"
#include <iostream>
#include <string> 

// Create a logging instance for logging messages to a file
Logger logger("application.log");

// Set window dimensions
const unsigned int WIDTH = 1280; // Width of the window
const unsigned int HEIGHT = 720; // Height of the window

// Callback function for handling window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // If either dimension is zero, log a warning and return
    if (width == 0 || height == 0) {
        logger.log("WARNING: Framebuffer size callback received zero dimensions. Ignoring.");
        return;
    }

    // Update the OpenGL viewport to match the new window dimensions
    glViewport(0, 0, width, height);

    // Log the updated dimensions
    logger.log("INFO: Framebuffer size callback triggered. Width: " + std::to_string(width) + ", Height: " + std::to_string(height));

    // Get the HikingSimulator instance from the window's user pointer
    HikingSimulator* simulator = static_cast<HikingSimulator*>(glfwGetWindowUserPointer(window));
    if (simulator) {
        // Update the simulator's internal window dimensions
        simulator->setWindowDimensions(width, height);
    }
}

// Callback function for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Get the HikingSimulator instance from the window's user pointer
    HikingSimulator* simulator = static_cast<HikingSimulator*>(glfwGetWindowUserPointer(window));
    if (simulator) {
        // Pass mouse position to the simulator for processing
        simulator->processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

// Function to handle global input, such as ESC to close the window
void processGlobalInput(GLFWwindow* window) {
    // Check if the ESC key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        // Close the window
        glfwSetWindowShouldClose(window, true);
        // Log the action
        logger.log("INFO: ESC key pressed. Closing the window.");
    }
}

int main() {
    // Log the start of the application
    logger.log("INFO: Starting application");

    // Initialize the WindowManager with specified dimensions and title
    WindowManager windowManager(WIDTH, HEIGHT, "3D Hiking Simulator");
    GLFWwindow* window = windowManager.getWindow(); // Get the GLFW window instance

    // Set the callback for handling window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize the HikingSimulator
    HikingSimulator simulator;
    simulator.setWindowDimensions(WIDTH, HEIGHT); // Pass initial window dimensions to the simulator
    if (!simulator.initialize()) { // Check if initialization was successful
        logger.log("ERROR: Failed to initialize Hiking Simulator");
        return -1; // Exit with an error code if initialization failed
    }

    // Set the simulator instance as the user pointer for the window
    glfwSetWindowUserPointer(window, &simulator);

    // Set the mouse movement callback
    glfwSetCursorPosCallback(window, mouse_callback);

    // Variables to manage frame timing
    float lastTime = static_cast<float>(glfwGetTime()); // Time of the last frame
    float deltaTime = 0.0f; // Time difference between frames

    // Log the start of the render loop
    logger.log("INFO: Starting main render loop");

    // Main rendering loop
    while (!windowManager.shouldClose()) { // Continue while the window is not closing
        // Calculate deltaTime (time elapsed since the last frame)
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime; // Time difference between frames
        lastTime = currentTime; // Update last frame time

        // Clear the screen for the new frame
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color to dark gray
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

        // Handle global input (e.g., ESC key)
        processGlobalInput(window);

        // Handle simulator-specific input
        simulator.processInput(window, deltaTime);

        // Render the simulator scene
        simulator.render(deltaTime); // Pass deltaTime for time-dependent updates

        // Swap front and back buffers
        windowManager.swapBuffers();

        // Poll for events (e.g., input, resizing)
        windowManager.pollEvents();
    }

    // Clean up simulator resources
    simulator.cleanup();

    // Log successful termination
    logger.log("INFO: Program terminated successfully");
    return 0; // Exit the program
}
