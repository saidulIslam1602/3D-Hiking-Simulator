// shaders/effectVert.glsl
#version 330 core
// Specifies the GLSL version (OpenGL 3.3 core profile)

layout(location = 0) in vec2 aPos;
// Input attribute at location 0, representing the vertex position in 2D space (x, y)

layout(location = 1) in vec2 aTexCoords;
// Input attribute at location 1, representing the texture coordinates (u, v)

out vec2 TexCoords;
// Output variable to pass texture coordinates to the fragment shader

void main()
{
    TexCoords = aTexCoords;
    // Passes the input texture coordinates to the fragment shader

    gl_Position = vec4(aPos.xy, 0.0, 1.0);
    // Sets the vertex position in clip space
    // - `aPos.xy`: 2D position from the input
    // - `0.0`: Z-coordinate (set to 0 since it's a 2D effect)
    // - `1.0`: Homogeneous W-coordinate (required for proper projection)
}
