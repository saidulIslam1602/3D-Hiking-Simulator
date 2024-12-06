#version 330 core
// Specifies the GLSL version (OpenGL 3.3 core profile)



layout(location = 0) in vec3 aPos;
// Input attribute at location 0, representing the vertex position (x, y, z)



layout(location = 1) in vec3 aColor;
// Input attribute at location 1, representing the vertex color (R, G, B)



out vec3 vertexColor;
// Output variable to pass the vertex color to the fragment shader



uniform mat4 model;
// Uniform matrix for transforming object space to world space



uniform mat4 view;
// Uniform matrix for transforming world space to camera (view) space



uniform mat4 projection;
// Uniform matrix for transforming camera (view) space to clip space



void main() {
    // Pass the input vertex color to the fragment shader
    vertexColor = aColor;



    // Transform the vertex position from object space to clip space
    // Combines the model, view, and projection matrices
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    // - `aPos`: The input vertex position
    // - `1.0`: Homogeneous coordinate for transformations
}
