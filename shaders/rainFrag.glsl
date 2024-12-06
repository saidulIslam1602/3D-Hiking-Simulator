// shaders/rainFrag.glsl
#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform float time;
uniform vec2 resolution;

const float PI = 3.1415926538;

// Pseudo-random function
float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// Function to simulate raindrop shapes
float raindrop(vec2 uv, float scale)
{
    uv *= scale;
    uv.y += time * scale * 2.0; // Fall speed
    uv.x += sin(uv.y * 0.05 + time) * 0.5; // Wind effect

    vec2 p = fract(uv) - 0.5;
    float y = p.y * 6.0; // Stretch vertically
    float d = length(vec2(p.x, y));
    float drop = smoothstep(0.2, 0.0, d); // Increase size

    return drop;
}

void main()
{
    vec2 uv = TexCoords * resolution.xy / resolution.y;

    float c = 0.0;

    // Number of layers for depth effect
    int layers = 15; // Increase layers for density
    for (int i = 0; i < layers; i++)
    {
        float scale = float(i + 1) * 3.0; // Adjust scale for size
        float n = raindrop(uv, scale);
        c += n * (1.0 / float(layers));
    }

    c = clamp(c, 0.0, 1.0);
    float alpha = c * 0.7; // Increase alpha for visibility

    FragColor = vec4(vec3(0.6, 0.6, 0.8), alpha); // Light blue raindrops
}
