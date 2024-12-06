
#version 330 core

out vec4 FragColor;
// Output variable for the final fragment color (RGBA)

uniform vec4 color;
// Uniform variable representing the particle's color, passed from the application

void main()
{
    float dist = length(gl_PointCoord - vec2(0.5));
    // Calculates the distance from the current fragment to the center of the point
    // - `gl_PointCoord`: Built-in variable that gives the fragment's position in the particle (range [0, 1])
    // - `vec2(0.5)`: Center of the particle in normalized coordinates

    if (dist > 0.5)
        discard; // Make particles circular
    // Discards fragments outside a radius of 0.5, making the particle appear circular

    FragColor = color;
    // Sets the fragment color to the provided `color` uniform
}
