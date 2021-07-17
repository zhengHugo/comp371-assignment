//
// Created by Hugo Zheng on 2021-07-14.
//

#ifndef COMP371_LABS_CAMERA_H
#define COMP371_LABS_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// default camera data
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 8.0f;
const float SENSITIVITY = 0.05f;
const float FOV = 60.0f;

class Camera {
 public:

  glm::vec3 Position;

  // directions
  glm::vec3 Front{glm::vec3(0.0f, 0.0f, -1.0f)};
  glm::vec3 Up{glm::vec3(0.0f, 1.0f, 0.0f)};
  glm::vec3 Right{glm::vec3(1.0f, 0.0f, 0.0f)};
  glm::vec3 WorldUp{glm::vec3(0.0f, 1.0f, 0.0f)};

  // Euler angles
  float Yaw{YAW};
  float Pitch{PITCH};

  float MovementSpeed{SPEED};
  float MouseSensitivity{SENSITIVITY};
  float Fov{FOV};

  /**
   * Camera constructor
   * @param position camera position in world space
   * @param up up direction in world space, usually (0.0, 1.0, 0.0)
   * @param yaw yaw angle of camera
   * @param pitch pitch angle of camera
   */
  explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
                  float yaw = YAW,
                  float pitch = PITCH);

  /**
   * Camera constructor
   * @param posX x value of camera position
   * @param posY y value of camera position
   * @param posZ z value of camera position
   * @param upX x value of up vector in world space
   * @param upY y value of up vector in world space
   * @param upZ z value of up vector in world space
   * @param yaw yaw angle of camera
   * @param pitch pitch angle of camera
   */
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

  /**
   * Computer view matrix from Euler angles and the LookAt matrix
   * @return the view matrix
   */
  glm::mat4 getViewMatrix() const;

  /**
   * Process input from a keyboard
   * @param direction camera movement direction. Must be one of @c FORWARD, @c BACKWARD,@c LEFT, or @c RIGHT
   * @param deltaTime time passed since the last frame
   */
  void move(CameraMovement direction, float deltaTime);

  /**
   * Process input received from a mouse movement.
   * @param xOffset offset value in the x direction
   * @param yOffset offset value in the y direction
   * @param constraintPitch set true to constrain pitch value in [-89.0, 89.0]
   */
  void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

  /**
   * Process input received from a mouse scroll-wheel
   * @param yOffset
   */
  void processMouseScroll(float yOffset);

 private:

  /**
   * Calculate the camera basis vectors from the updated Euler angles
   */
  void updateCameraVectors();
};

#endif //COMP371_LABS_CAMERA_H
