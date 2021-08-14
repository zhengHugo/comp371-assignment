//
// Created by LittleRedRidingU  on 2021/7/27.
//

#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 angles, glm::vec3 color) :
    position(position),
    angles(angles),
    color(color) {
  // some value defined by some smart guys
  constant = 1.0f;
  linear = 0.022f;
  quadratic = 0.0019f;
}
