#version 330 core


layout(location = 0) in vec3 aPos;
// Input vertex attribute at location 0, representing the vertex position (x, y, z)

out vec3 FragPos;
// Output variable to pass the fragment's world-space position to the fragment shader


out vec3 Normal;
// Output variable to pass the fragment's normal vector to the fragment shader

uniform mat4 model;
// Uniform matrix for transforming object space to world space


uniform mat4 view;
// Uniform matrix for transforming world space to camera (view) space


uniform mat4 projection;
// Uniform matrix for transforming camera (view) space to clip space

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Transforms the input vertex position from object space to world space
    // Multiplies the model matrix by the vertex position and assigns the result to `FragPos`

    Normal = mat3(transpose(inverse(model))) * aPos;
    // Calculates the transformed normal vector
    // The normal is transformed by the inverse transpose of the model matrix
    // Uses `mat3` to extract the upper-left 3x3 submatrix of the transformed model matrix
    // Ensures the normal remains accurate under non-uniform scaling

    gl_Position = projection * view * vec4(FragPos, 1.0);
    // Transforms the vertex position from world space to clip space
    // Combines projection and view matrices and applies them to `FragPos`
    // `gl_Position` is a built-in variable that determines the final position of the vertex
}
