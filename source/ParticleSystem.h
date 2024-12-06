// ParticleSystem.h

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include "Particle.h"
#include "Shader.h"
#include "Terrain.h" // Assuming you have a Terrain class

class ParticleSystem {
public:
    ParticleSystem(unsigned int maxParticles);

    void update(float deltaTime, const glm::vec3& cameraPos, const Terrain& terrain);
    void render(const glm::mat4& view, const glm::mat4& projection);

    void cleanup();

private:
    std::vector<Particle> particles;
    unsigned int maxParticles;
    unsigned int lastUsedParticle;
    Shader particleShader;
    unsigned int VAO, VBO;

    void init();
    void respawnParticle(Particle& particle, const glm::vec3& cameraPos);
    unsigned int firstUnusedParticle();
};

#endif // PARTICLE_SYSTEM_H
