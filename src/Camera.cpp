//
// Created by Hugo Zheng on 2021-07-14.
//

#include "Camera.h"

Camera::Camera() :
    Position(DEFAULT_POS),
    Target(DEFAULT_TARGET),
    Up(glm::vec3(0.0f, 1.0f, 0.0f)) {}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(glm::vec3(Position), glm::vec3(Target), Up);
}

void Camera::rotate(glm::vec3 axis, float deltaTime) {
  glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), ANGULAR_SPEED * deltaTime, axis);
  Position = matrix * Position;
  Target = matrix * Target;
  Up = glm::vec3(matrix * glm::vec4(Up, 0.0f));
}

void Camera::goHome() {
  Position = DEFAULT_POS;
  Target = DEFAULT_TARGET;
  Up = glm::vec3(0.0f, 1.f, 0.f);
}

void Camera::processMouseScroll(float yOffset) {
  Fov -= (float) yOffset;
  if (Fov < 1.0f) Fov = 1.0f;
  if (Fov > 60.0f) Fov = 60.0f;
}

void Camera::pan(float offset, float deltaTime) {
  float movementLength = SPEED * offset * deltaTime;
  glm::vec3 front = glm::normalize(glm::vec3(Target - Position));
  glm::vec3 right = glm::normalize(glm::cross(front, Up));
  Position += glm::vec4(movementLength * right, 0.0f);
  Target += glm::vec4(movementLength * right, 0.0f);
}

void Camera::tilt(float offset, float deltaTime) {
  float tiltAngle = ANGULAR_SPEED * offset * deltaTime;
  glm::vec3 front = glm::normalize(glm::vec3(Target - Position));
  glm::vec3 right = glm::normalize(glm::cross(front, Up));
  glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -tiltAngle, right);
  Target = Position + rotationMatrix * (Target - Position);
  Up = rotationMatrix * glm::vec4(Up, 0.0f);
}