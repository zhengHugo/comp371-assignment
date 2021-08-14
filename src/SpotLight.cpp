#include <iostream>
#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color):
    position(_position), 
    angles(_angles), 
    color(_color) 
{
  UpdateDirection();
  // some value defined by some smart guys
  constant = 1.0f;
  linear = 0.09f;
  quadratic = 0.032f;
}

void SpotLight::UpdateDirection() {
  direction = glm::vec3(0, 0, 1.0f);
  direction = glm::rotateZ(direction, angles.z);
  direction = glm::rotateX(direction, angles.x);
  direction = glm::rotateY(direction, angles.y);
  direction = -1.0f * direction;
}

float SpotLight::getCutOffAngle() const{
  return this->DEFAULT_OUTER;
}

void SpotLight::setCutOffAngle(float deltaTime){
  DEFAULT_INNER *= deltaTime;
  if (DEFAULT_INNER>40){
    DEFAULT_INNER = 40;
  }
  if (DEFAULT_INNER<2){
    DEFAULT_INNER = 2;
  }
  DEFAULT_OUTER = DEFAULT_INNER * 2.2f;
  this->innerCosPhy = cos(glm::radians(DEFAULT_INNER));
  this->outerCosPhy = cos(glm::radians(DEFAULT_OUTER));
}