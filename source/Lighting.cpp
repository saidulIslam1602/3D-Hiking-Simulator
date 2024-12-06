#include "Lighting.h"
#include <glm/gtc/type_ptr.hpp>

//position of light 3D Space, color of the light
Lighting::Lighting(const glm::vec3& pos, const glm::vec3& col)
    : position(pos), color(col) {
} // initialization of position and color


//applying light effect to shader object
void Lighting::apply(Shader& shader) const {
    shader.setVec3("lightPos", position);
    shader.setVec3("lightColor", color);

    // Add enhanced lighting parameters
    shader.setVec3("light.position", position); //light position in 3D space
    shader.setVec3("light.ambient", color * 0.3f); //indirect illumination, reducing makes it softer ambient effect
    shader.setVec3("light.diffuse", color); //comes directly from light source and scatters upon hitting a surface.
    shader.setVec3("light.specular", color * 0.5f); //the bright spot of light that, realistic effect
}
