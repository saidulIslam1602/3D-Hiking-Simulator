// ParticleSystem.cpp

#include "ParticleSystem.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp> // For glm::value_ptr
#include <iostream>
#include <cstdlib> // For rand()
#include <ctime>   // For time()

ParticleSystem::ParticleSystem(unsigned int maxParticles)
    : maxParticles(maxParticles), lastUsedParticle(0),
    particleShader("shaders/particleVert.glsl", "shaders/particleFrag.glsl")
{
    particles.resize(maxParticles);
    init();

    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)));
}

void ParticleSystem::init() {
    // Configure VAO/VBO for point rendering
    float particleQuad[] = {
        0.0f, 0.0f, 0.0f //define the vertex data at origin
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime, const glm::vec3& cameraPos, const Terrain& terrain) {
    // Add new particles
    unsigned int newParticles = maxParticles / 20; // Determine the number of new particles to spawn (5% of maxParticles)
    for (unsigned int i = 0; i < newParticles; ++i) {
        int unusedParticle = firstUnusedParticle(); // Find an unused particle (reuse a dead one or find a free slot)
        respawnParticle(particles[unusedParticle], cameraPos); // Respawn the particle near the camera position
    }

    // Update all particles
    for (unsigned int i = 0; i < maxParticles; ++i) {
        Particle& p = particles[i]; // Reference the current particle for updates
        p.life -= deltaTime; // Reduce the particle's life based on the elapsed time
        if (p.life > 0.0f) { // Only update active particles (those with remaining life)
            // Apply gravity
            p.velocity += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime; // Add gravity to the particle's velocity
            p.position += p.velocity * deltaTime; // Update the particle's position based on its velocity

            // Check collision with terrain
            float terrainHeight = terrain.getHeightAtPosition(p.position.x, p.position.z); // Get the terrain height at the particle's (x, z) position
            if (p.position.y <= terrainHeight) { // If the particle is below the terrain height
                p.life = 0.0f; // Mark the particle as "dead" by setting its life to 0
            }
        }
    }
}


void ParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
    glEnable(GL_BLEND); //blend for transparency

    //aplha value for transparency, compliment source alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Disable writing to the depth buffer

    particleShader.use();
    particleShader.setMat4("view", view); //view matrix to the shader for camera positioning
    particleShader.setMat4("projection", projection); //for perspective projection

    glBindVertexArray(VAO);
    for (Particle& particle : particles) {
        if (particle.life > 0.0f) {
            particleShader.setVec3("offset", particle.position); //// Pass the particle's position as an offset to the shader
            particleShader.setVec4("color", particle.color);
            particleShader.setFloat("size", particle.size);
            glDrawArrays(GL_POINTS, 0, 1); //point primitive, starting from first vertex, render one instance of vertex
        }
    }

    // Unbind the VAO to clean up the state
    glBindVertexArray(0);

    glDepthMask(GL_TRUE); // Re-enable depth buffer writing

    // Disable blending to clean up the state
    glDisable(GL_BLEND);
}


unsigned int ParticleSystem::firstUnusedParticle() {
    // Search from the last used particle for an available (dead) particle
    for (unsigned int i = lastUsedParticle; i < maxParticles; ++i) {
        // If the particle's life is 0 or less, it is considered unused (dead)
        if (particles[i].life <= 0.0f) {
            lastUsedParticle = i; // Update the last used particle index for optimization
            return i; // Return the index of the unused particle
        }
    }

    // If no unused particle was found in the previous loop, search from the beginning
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        // Check if the particle's life is 0 or less
        if (particles[i].life <= 0.0f) {
            lastUsedParticle = i; // Update the last used particle index
            return i; // Return the index of the unused particle
        }
    }

    // If all particles are alive (no unused particles found), overwrite the first one
    lastUsedParticle = 0; // Reset the last used particle index to 0
    return 0; // Return the index of the first particle
}


void ParticleSystem::respawnParticle(Particle& particle, const glm::vec3& cameraPos) {
    float spread = 50.0f; //can spawn25 units in x and z directions
    
    //-.5f symmetrical offset
    glm::vec3 randomOffset = glm::vec3(
        ((rand() % 100) / 100.0f - 0.5f) * spread,
        0.0f,
        ((rand() % 100) / 100.0f - 0.5f) * spread
    );

    particle.position = cameraPos + randomOffset + glm::vec3(0.0f, 30.0f, 0.0f); // Spawn 30 units above camera
    particle.velocity = glm::vec3(0.0f, -50.0f, 0.0f); // Fast downward velocity
    particle.life = 20.0f;
    particle.size = 50000.0f;
    particle.color = glm::vec4(0.2f, 0.3f, 0.8f, 0.8f);

}


void ParticleSystem::cleanup() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}
