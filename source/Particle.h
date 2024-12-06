// Particle.h

#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life; // Remaining life of the particle
    float size;
    glm::vec4 color;

    Particle()
        : position(0.0f), velocity(0.0f), life(0.0f), size(1.0f), color(1.0f) { }
};

#endif // PARTICLE_H
