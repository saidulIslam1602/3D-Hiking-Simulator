
#version 410 core



out vec4 FragColor;
// Output variable for the final fragment color (RGBA)



in vec3 FragPos;
// Input variable for the fragment's position in world space, passed from the vertex shader

in vec3 Normal;
// Input variable for the interpolated surface normal at the fragment, passed from the vertex shader



// Structure defining light properties
struct Light {
    vec3 position;  // Position of the light source in world space
    vec3 color;     // Base color of the light
    vec3 ambient;   // Ambient light intensity
    vec3 diffuse;   // Diffuse light intensity
    vec3 specular;  // Specular light intensity
};



// Structure defining material properties
struct Material {
    vec3 ambient;   // Ambient reflection coefficient of the material
    vec3 diffuse;   // Diffuse reflection coefficient of the material
    vec3 specular;  // Specular reflection coefficient of the material
    float shininess; // Shininess factor controlling the size of specular highlights
};



// Uniforms for light, material, and camera position
uniform Light light;
// Uniform variable holding the properties of the light source

uniform Material material;
// Uniform variable holding the properties of the material

uniform vec3 viewPos;
// Uniform variable for the camera's position in world space



void main() {

    // Normalize the surface normal for accurate lighting calculations
    vec3 norm = normalize(Normal);



    // Compute the light direction vector from the fragment to the light source
    vec3 lightDir = normalize(light.position - FragPos);



    // Compute the view direction vector from the fragment to the camera
    vec3 viewDir = normalize(viewPos - FragPos);



    // Compute the reflection direction of the light on the surface
    vec3 reflectDir = reflect(-lightDir, norm);



    // Calculate the ambient lighting component
    vec3 ambient = light.ambient * material.ambient;



    // Calculate the diffuse lighting component
    float diff = max(dot(norm, lightDir), 0.0);
    // Diffuse intensity is proportional to the angle between the normal and light direction
    vec3 diffuse = light.diffuse * (diff * material.diffuse);



    // Calculate the specular lighting component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Specular intensity depends on the alignment of the view direction and reflected light direction
    vec3 specular = light.specular * (spec * material.specular);



    // Add height-based coloring for additional terrain detail
    float height = FragPos.y / 200.0;
    // Normalize the height for mixing colors
    vec3 heightColor = mix(vec3(0.2, 0.4, 0.2), vec3(0.8, 0.8, 0.8), height);
    // Blend between green (low altitude) and gray (high altitude) based on the fragment's height



    // Combine ambient, diffuse, and specular lighting with height-based coloring
    vec3 result = (ambient + diffuse + specular) * heightColor;



    // Ensure minimum brightness for very dark areas
    result = max(result, vec3(0.1));



    // Output the final color of the fragment
    FragColor = vec4(result, 1.0);
    // Set alpha to 1.0 for full opacity
}
