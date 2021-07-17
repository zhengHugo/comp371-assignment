//
// Created by Hugo Zheng on 2021-07-16.
//

#ifndef COMP371_ASSIGNMENT1_MODEL_H
#define COMP371_ASSIGNMENT1_MODEL_H

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum class ModelMovement {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

// default parameter
const float scaleSpeed = 0.5f;
const float moveSpeed = 5.0f;

class Model {
 private:

  glm::vec3 basePosition;


  float Scale;
  std::vector<glm::mat4> modelMatrices;
  std::vector<glm::vec3> relativePositions;

  void updateModelMatrices();

 public:

  Model(glm::vec3 basePosition, std::vector<glm::vec3> &relativePositions);

  void scaleUp(float deltaTime);

  void scaleDown(float deltaTime);

  void move(ModelMovement direction, float deltaTime);

  glm::mat4 getModelMatrix(int index);

};


#endif //COMP371_ASSIGNMENT1_MODEL_H
