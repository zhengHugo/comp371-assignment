//
// Created by Hugo Zheng on 2021-07-16.
//

#include "Model.h"

Model::Model(glm::vec3 basePosition, std::vector<glm::vec3> &relativePositions) :
    defaultBasePosition(basePosition),
    basePosition(basePosition),
    relativePositions(relativePositions),
    Scale(1.0f),
    modelMatrices(std::vector<glm::mat4>(relativePositions.size(), glm::mat4(1.0f))),
    quaternion(glm::quat(glm::vec3(0.0, 0.0, 0.0))) {
  updateModelMatrices();

}

void Model::setBasePosition(glm::vec3 newBasePosition) {
  this->basePosition = newBasePosition;
  updateModelMatrices();
}

void Model::resetPosition() {
  this->basePosition = defaultBasePosition;
  updateModelMatrices();
}

void Model::resetOrientation() {
  this->quaternion = glm::quat(glm::vec3(0.0, 0.0, 0.0));
  updateModelMatrices();
}

void Model::scaleUp(float deltaTime) {
  Scale += scaleSpeed * deltaTime;
  updateModelMatrices();
}

void Model::scaleDown(float deltaTime) {
  Scale -= scaleSpeed * deltaTime;
  if (Scale < 0.1f) Scale = 0.1f;
  updateModelMatrices();
}

void Model::move(ModelMovement direction, float deltaTime) {
  if (direction == ModelMovement::UP) {
    basePosition += moveSpeed * deltaTime * glm::vec3(0.0f, 1.0f, 0.0f);
  } else if (direction == ModelMovement::DOWN) {
    basePosition += moveSpeed * deltaTime * glm::vec3(0.0f, -1.0f, 0.0f);
  } else if (direction == ModelMovement::LEFT) {
    basePosition += moveSpeed * deltaTime * glm::vec3(-1.0f, 0.0f, 0.0f);
  } else if (direction == ModelMovement::RIGHT) {
    basePosition += moveSpeed * deltaTime * glm::vec3(1.0f, 0.0f, 0.0f);
  } else if (direction == ModelMovement::FORWARD) {
    basePosition += moveSpeed * deltaTime * glm::vec3(0.0f, 0.0f, -1.0f);
  } else if (direction == ModelMovement::BACKWARD) {
    basePosition += moveSpeed * deltaTime * glm::vec3(0.0f, 0.0f, 1.0f);
  }
  updateModelMatrices();

}

void Model::rotate(float angleInRadian, glm::vec3 axis, float deltaTime) {
  quaternion = glm::angleAxis(angleInRadian * deltaTime, axis) * quaternion;
  updateModelMatrices();
}

glm::mat4 Model::getModelMatrix(unsigned int index) {
  return modelMatrices[index];
}

void Model::updateModelMatrices() {
  glm::mat4 model = glm::mat4(1.0f);
  for (int i = 0; i < relativePositions.size(); i++) {
    modelMatrices[i] = glm::translate(model, glm::vec3(basePosition));
    modelMatrices[i] = modelMatrices[i] * glm::mat4_cast(quaternion);
    modelMatrices[i] = glm::scale(modelMatrices[i], glm::vec3(Scale, Scale, Scale));
    modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3(relativePositions[i]));
  }
}

unsigned int Model::size() {
  return relativePositions.size();
}
glm::vec3 Model::getModelPosition() {
    return this->basePosition;

}


