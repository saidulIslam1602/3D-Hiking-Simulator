#version 330 core

layout (location = 0) in vec3 aPos;
// Input attribute at location 0, representing the particle's position (x, y, z)

uniform mat4 projection;
// Uniform matrix for transforming view space to clip space

uniform mat4 view;
// Uniform matrix for transforming world space to view space

uniform vec3 offset;
// Uniform vector to specify the particle's position offset in world space

uniform float size;
// Uniform float to specify the particle's size

void main()
{
    gl_Position = projection * view * vec4(aPos + offset, 1.0);
    // Transforms the particle's position from world space to clip space
    // - Adds the `offset` to the particle's base position
    // - Multiplies the resulting position by the combined projection and view matrices
    // - Outputs the result to `gl_Position` for further processing in the graphics pipeline

    gl_PointSize = size;
    // Sets the size of the point (particle) for rendering
    // - This determines how large the particle appears on the screen
}
