#version 330 core
// Specifies the GLSL version (OpenGL 3.3 core profile)



layout(location = 0) in vec3 aPos;
// Input attribute at location 0, representing the vertex position (x, y, z)



layout(location = 1) in vec3 aNormal;
// Input attribute at location 1, representing the vertex normal vector (x, y, z)



out vec3 FragPos;
// Output variable to pass the fragment's world-space position to the fragment shader



out vec3 Normal;
// Output variable to pass the fragment's transformed normal vector to the fragment shader



uniform mat4 model;
// Uniform matrix for transforming object space to world space



uniform mat4 view;
// Uniform matrix for transforming world space to view (camera) space



uniform mat4 projection;
// Uniform matrix for transforming view space to clip space



void main()
{
    // Transform the vertex position from object space to world space
    FragPos = vec3(model * vec4(aPos, 1.0));



    // Transform the vertex normal vector to world space
    // - `inverse(model)`: Accounts for non-uniform scaling
    // - `transpose`: Ensures correct transformation of normals
    Normal = mat3(transpose(inverse(model))) * aNormal;



    // Transform the vertex position to clip space
    // Combines the projection, view, and model transformations
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
