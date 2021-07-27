//
// Created by LittleRedRidingU  on 2021/7/27.
//

#ifndef COMP371_ASSIGNMENT2_MATERIAL_H
#define COMP371_ASSIGNMENT2_MATERIAL_H
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material {
 public:
  Shader shader;
  unsigned int diffuse;
  unsigned int specular;
  glm::vec3 ambient;
  float shininess;

  Material(Shader &shader, unsigned int diffuse, unsigned int specular, glm::vec3 ambient, float shininess);
};


#endif //COMP371_ASSIGNMENT2_MATERIAL_H
