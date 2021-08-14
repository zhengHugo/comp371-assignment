#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
//#include <gtc/matrix_transform.hpp>

class DirectionalLight {
 public:
  DirectionalLight(glm::vec3 _position,
                   glm::vec3 _angles,
                   glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f));
  void UpdateDirection();
  glm::vec3 position;
  glm::vec3 angles;
  glm::vec3 direction = glm::vec3(0, 0, 1.0f);
  glm::vec3 color;
};
