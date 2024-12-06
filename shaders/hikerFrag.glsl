#version 410 core
// Specifies the GLSL version (OpenGL 4.1 core profile)

out vec4 FragColor;
// Output variable for the fragment's final color (RGBA)

uniform vec3 pathColor;
// Uniform variable representing the color of the path, provided by the CPU

void main() {
    FragColor = vec4(pathColor, 0.8);
    // Sets the fragment's color to the value of `pathColor` with an alpha of 0.8
    // - `pathColor`: RGB color passed from the application
    // - `0.8`: Alpha value for transparency (semi-transparent)
}
