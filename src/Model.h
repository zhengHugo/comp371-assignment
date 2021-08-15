//
// Created by Yuguo Zheng on 2021-07-16.
//

#ifndef COMP371_PROJECT_MODEL_H
#define COMP371_PROJECT_MODEL_H

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>

enum class ModelMovement {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  FORWARD,
  BACKWARD
};

// default parameter
const float scaleSpeed = 0.5f;
const float moveSpeed = 5.0f;

class Model {
 private:
  glm::vec3 defaultBasePosition;

  glm::vec3 basePosition;

  std::vector<glm::vec3> relativePositions;

  float Scale;

  glm::quat quaternion;

  std::vector<glm::mat4> modelMatrices;

  void updateModelMatrices();

 public:

  Model(glm::vec3 basePosition, std::vector<glm::vec3> &relativePositions);

  void setBasePosition(glm::vec3 basePosition);

  void resetPosition();

  void resetOrientation();

  void scaleUp(float deltaTime);

  void scaleDown(float deltaTime);

  void move(ModelMovement direction, float deltaTime);

  void rotate(float angleInRadian, glm::vec3 axis, float deltaTime);

  glm::mat4 getModelMatrix(unsigned int index);

  unsigned int size();

  glm::vec3 getModelPosition();
};


#endif //COMP371_PROJECT_MODEL_H
