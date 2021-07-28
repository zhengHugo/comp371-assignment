//
// Created by LittleRedRidingU on 2021/7/27.
//

#include "Material.h"

Material::Material(Shader &shader, unsigned int diffuse, unsigned int specular,
                   glm::vec3 ambient, float shininess) :
    shader(shader),
    diffuse(diffuse),
    specular(specular),
    ambient(ambient),
    shininess(shininess) {

}

#include "Material.h"
