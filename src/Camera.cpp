//
// Created by Hugo Zheng on 2021-07-14.
//

#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) :
    Position(position),
    WorldUp(worldUp),
    Yaw(yaw),
    Pitch(pitch) {
  updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    Position(glm::vec3(posX, posY, posZ)),
    WorldUp(glm::vec3(upX, upY, upZ)),
    Yaw(yaw),
    Pitch(pitch) {
  updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(Position, Position + Front, Up);
}

void Camera::move(CameraMovement direction, float deltaTime) {
  float movementLength = MovementSpeed * deltaTime;
  if (direction == CameraMovement::FORWARD) Position += movementLength * Front;
  if (direction == CameraMovement::BACKWARD) Position -= movementLength * Front;
  if (direction == CameraMovement::LEFT) Position -= movementLength * Right;
  if (direction == CameraMovement::RIGHT) Position += movementLength * Right;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
  xOffset *= MouseSensitivity;
  yOffset *= MouseSensitivity;

  Yaw += xOffset;
  Pitch -= yOffset;

  if (constrainPitch) {
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
  Fov -= (float) yOffset;
  if (Fov < 1.0f) Fov = 1.0f;
  if (Fov > 60.0f) Fov = 60.0f;
}

void Camera::updateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
  front.y = sin(glm::radians(Pitch));
  front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
  Front = glm::normalize(front);
  Right = glm::normalize(glm::cross(Front, WorldUp));
  Up = glm::normalize(glm::cross(Right, Front));
}

