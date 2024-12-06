// WindowManager.h

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

class WindowManager {
public:
  
    WindowManager(int width, int height, const char* title);
    ~WindowManager();
    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    GLFWwindow* getWindow() const;
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* window; ///< Pointer to the GLFW window.
};

#endif // WINDOWMANAGER_H
