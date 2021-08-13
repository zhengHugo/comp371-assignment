//
// Created by LittleRedRidingU  on 2021/7/27.
//

#ifndef COMP371_PROJECT_MATERIAL_H
#define COMP371_PROJECT_MATERIAL_H
#include "Shader.h"
#include <glm/glm.hpp>

class Material {
 public:
  unsigned int diffuse;
  unsigned int specular;
  glm::vec3 ambient;
  float shininess;

  Material(unsigned int diffuse,
           unsigned int specular,
           glm::vec3 ambient,
           float shininess);
};

#endif //COMP371_PROJECT_MATERIAL_H
