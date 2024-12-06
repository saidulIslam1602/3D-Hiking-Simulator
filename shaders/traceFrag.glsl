#version 330 core
// Specifies the GLSL version (OpenGL 3.3 core profile)



in vec3 vertexColor;
// Input variable passed from the vertex shader containing the color of the vertex (RGB)



out vec4 FragColor;
// Output variable for the final fragment color (RGBA)



void main() {
    // Set the output fragment color to the input vertex color with full opacity
    FragColor = vec4(vertexColor, 1.0);
    // - `vertexColor`: RGB color from the vertex shader
    // - `1.0`: Alpha value for full opacity
}
