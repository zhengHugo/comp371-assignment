#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

class SpotLight {
 private:
  float DEFAULT_INNER = 10.0f;
  float DEFAULT_OUTER = 22.0f;
 public:
  SpotLight(glm::vec3 _position, glm::vec3 _angles,
             glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f));
  void UpdateDirection();
  glm::vec3 position;
  glm::vec3 angles;
  glm::vec3 direction = glm::vec3(0, 0, 1.0f);
  glm::vec3 color;

  float constant;
  float linear;
  float quadratic;
  // the cutoff angle of spotlight
  float innerCosPhy = cos(glm::radians(DEFAULT_INNER));
  float outerCosPhy = cos(glm::radians(DEFAULT_OUTER));

  void setCutOffAngle(float deltaTime);
  float getCutOffAngle() const;

};
