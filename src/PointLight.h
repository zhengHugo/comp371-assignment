//
// Created by LittleRedRidingU  on 2021/7/27.
//

#ifndef COMP371_ASSIGNMENT2_POINTLIGHT_H
#define COMP371_ASSIGNMENT2_POINTLIGHT_H
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

class PointLight {
 public:
  PointLight(glm::vec3 _position, glm::vec3 _angles,glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f));
  glm::vec3 position;
  glm::vec3 angles;
  glm::vec3 direction = glm::vec3(0, 0, 1.0f);
  glm::vec3 color;

  float constant;
  float linear;
  float quadratic;
};


#endif //COMP371_ASSIGNMENT2_POINTLIGHT_H
