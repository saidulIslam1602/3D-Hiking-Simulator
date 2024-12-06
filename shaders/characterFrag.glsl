#version 330 core


out vec4 FragColor;
// Defines the output variable for the fragment's final color (RGBA)

in vec3 Normal;
// Interpolated normal vector from the vertex shader

in vec3 FragPos;
// Interpolated fragment position in world space from the vertex shader

uniform vec3 objectColor;
// The base color of the object

uniform vec3 lightPosition;
// The position of the light source in world space

uniform vec3 lightColor;
// The color or intensity of the light source

uniform vec3 viewPos;
// The position of the camera or viewer in world space

uniform vec3 materialAmbient;
// Ambient reflection coefficient of the material

uniform vec3 materialDiffuse;
// Diffuse reflection coefficient of the material

uniform vec3 materialSpecular;
// Specular reflection coefficient of the material

uniform float materialShininess;
// Shininess factor controlling the size and sharpness of specular highlights

void main()
{
    // Ambient Lighting
    // Computes the ambient lighting component
    // Multiplies the light color/intensity by the material's ambient reflectivity

    vec3 ambient = lightColor * materialAmbient;


    // Diffuse Lighting
    // Normalizes the normal vector to ensure unit length
    vec3 norm = normalize(Normal);


    // Computes the normalized direction from the fragment to the light source
    vec3 lightDir = normalize(lightPosition - FragPos);



    // Calculates the cosine of the angle between the normal and light direction
    // Uses `max` to ensure no negative values (light from behind the surface does not contribute)
    float diff = max(dot(norm, lightDir), 0.0);

   
    // Computes the diffuse lighting component
    // Multiplies the light color, diffuse intensity, and material's diffuse reflectivity
    vec3 diffuse = lightColor * diff * materialDiffuse;


    // Specular Lighting
    float specularStrength = 0.5;


    // Strength multiplier for the specular component
    vec3 viewDir = normalize(viewPos - FragPos);


    // Computes the normalized direction from the fragment to the viewer
    vec3 reflectDir = reflect(-lightDir, norm);


    // Computes the reflection direction of the light relative to the surface normal
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);


    // Calculates the specular intensity based on the shininess factor
    // Raises the dot product of the view direction and reflected direction to the power of shininess
    vec3 specular = lightColor * spec * materialSpecular;


    // Computes the specular lighting component
    // Multiplies the light color, specular intensity, and material's specular reflectivity
    // Combine Lighting Components
    vec3 result = (ambient + diffuse + specular) * objectColor;

    

    FragColor = vec4(result, 1.0);
    // Outputs the final fragment color
    // Sets the alpha value to 1.0 (fully opaque)
}
