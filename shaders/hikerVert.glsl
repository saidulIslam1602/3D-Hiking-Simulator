#version 410 core


layout(location = 0) in vec3 aPos;
// Input attribute at location 0, representing the vertex position (x, y, z)

uniform mat4 model;
// Uniform matrix for transforming object space to world space


uniform mat4 view;
// Uniform matrix for transforming world space to camera (view) space


uniform mat4 projection;
// Uniform matrix for transforming camera (view) space to clip space


uniform float heightOffset;
// Uniform float to offset the height (y-coordinate) of the vertices

void main() {
    vec4 position = vec4(aPos.x, aPos.y + heightOffset, aPos.z, 1.0);
    // Adds the height offset to the y-coordinate of the vertex position
    // Constructs the final vertex position in homogeneous coordinates (x, y + heightOffset, z, w)

    gl_Position = projection * view * model * position;
    // Transforms the vertex position from object space to clip space
    // Combines the projection, view, and model matrices and applies them to the position
    // `gl_Position` is the built-in variable determining the vertex's final position in clip space
}
