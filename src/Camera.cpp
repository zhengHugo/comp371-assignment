//
// Created by Hugo Zheng on 2021-07-14.
//

#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up) :
    Position(glm::vec4(position, 1.0f)),
    Target(glm::vec4(target, 1.0f)),
    Up(up) {}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(glm::vec3(Position), glm::vec3(Target), Up);
}

void Camera::rotate(glm::vec3 axis, float deltaTime) {
  glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), ANGULAR_SPEED * deltaTime, axis);
  Position = matrix * (Position - Target) + Target;
  Up = glm::vec3(matrix * glm::vec4(Up, 0.0f));
  glm::vec3 front = Target - Position;
}

void Camera::processMouseScroll(float yOffset) {
  Fov -= (float) yOffset;
  if (Fov < 1.0f) Fov = 1.0f;
  if (Fov > 60.0f) Fov = 60.0f;
}

