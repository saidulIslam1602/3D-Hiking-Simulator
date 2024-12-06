#version 410 core

layout (location = 0) in vec3 aPos;
// Input attribute at location 0, representing the position of the skybox vertex (x, y, z)

out vec3 TexCoords;
// Output variable to pass the texture coordinates to the fragment shader
// Since the skybox is a cube map, 3D coordinates are used

uniform mat4 projection;
// Uniform matrix for transforming view space to clip space

uniform mat4 view;
// Uniform matrix for transforming world space to view space

void main() {
    TexCoords = aPos;
    // Passes the input position directly as the texture coordinates
    // Since the skybox vertices are in normalized cube coordinates

    vec4 pos = projection * view * vec4(aPos, 1.0);
    // Transforms the skybox vertex position from world space to clip space
    // Combines the projection and view matrices for the transformation

    gl_Position = pos.xyww;
    // Sets the vertex position for rendering
    // - `pos.xy`: Uses the X and Y coordinates for screen position
    // - `ww`: Ensures the Z-coordinate is set to the maximum depth (far plane)
    // - This keeps the skybox always rendered behind all other objects
}
