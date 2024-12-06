#version 410 core


out vec4 FragColor;
// Output variable for the fragment's final color (RGBA)

in vec3 TexCoords;
// Input variable containing the texture coordinates for the skybox
// Passed from the vertex shader, in 3D since the skybox is a cube

uniform samplerCube skybox;
// Uniform sampler for accessing the cube map texture
// Represents the skybox texture

void main() {    
    FragColor = texture(skybox, TexCoords);
    // Samples the cube map texture using the provided 3D texture coordinates
    // - `skybox`: The cube map texture
    // - `TexCoords`: The direction vector used to fetch the corresponding texel
    // Sets the sampled color as the fragment's final color
}
