//
// Created by LittleRedRidingU  on 2021/7/27.
//

#include "PointLight.h"

PointLight::PointLight(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color):
    position(_position),
    angles(_angles),
    color(_color)
{
  // some value defined by some smart guys
  constant = 1.0f;
  linear = 0.09f;
  quadratic = 0.032f;
}
