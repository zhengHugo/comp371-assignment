//
// Created by LittleRedRidingU on 2021/7/27.
//

#include "Material.h"

Material::Material(unsigned int diffuse, unsigned int specular,
                   glm::vec3 ambient, float shininess) :
    diffuse(diffuse),
    specular(specular),
    ambient(ambient),
    shininess(shininess) {

}

#include "Material.h"
