//
// Created by Yuguo Zheng on 2021-07-14.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT
};

// default camera data

class Camera {
 private:

  const float ANGULAR_SPEED = 0.5f;
  const float SPEED = 1.0f;
  const float FOV = 60.0f;
  const glm::vec4 DEFAULT_POS = glm::vec4(2.0f, 5.0f, 20.0f, 1.0f);
  const glm::vec4 DEFAULT_TARGET = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

 public:

  glm::vec4 Position;
  glm::vec4 Target;

  // directions
  glm::vec3 Up;

  float Fov{FOV};


  /**
   * Camera constructor
   * @param position camera position in world space
   * @param up up direction in world space, usually (0.0, 1.0, 0.0)
   * @param yaw yaw angle of camera
   * @param pitch pitch angle of camera
   */
  Camera();

//  /**
//   * Camera constructor
//   * @param posX x value of camera position
//   * @param posY y value of camera position
//   * @param posZ z value of camera position
//   * @param upX x value of up vector in world space
//   * @param upY y value of up vector in world space
//   * @param upZ z value of up vector in world space
//   * @param yaw yaw angle of camera
//   * @param pitch pitch angle of camera
//   */
//  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

  /**
   * Computer view matrix from Euler angles and the LookAt matrix
   * @return the view matrix
   */
  glm::mat4 getViewMatrix() const;

  void rotate(glm::vec3 axis, float deltaTime);

  void goHome();

  /**
   * Pan camera left-right
   * @param offset mouse movement offset
   * @param deltaTime time of a frame
   */
  void pan(float offset, float deltaTime);

  /**
   * Tilt camera up-down
   * @param offset mouse movement offset
   * @param deltaTime
   */
  void tilt(float offset, float deltaTime);

//  /**
//   * Process input received from a mouse movement.
//   * @param xOffset offset value in the x direction
//   * @param yOffset offset value in the y direction
//   * @param constraintPitch set true to constrain pitch value in [-89.0, 89.0]
//   */
//  void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

  /**
   * Process input received from a mouse scroll-wheel
   * @param yOffset
   */
  void processMouseScroll(float yOffset);
};

#endif // CAMERA_H
