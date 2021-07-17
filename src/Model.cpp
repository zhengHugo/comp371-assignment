//
// Created by Hugo Zheng on 2021-07-16.
//

#include "Model.h"

Model::Model(glm::vec3 basePosition, std::vector<glm::vec3> &relativePositions) :
    basePosition(basePosition),
    relativePositions(relativePositions),
    Scale(1.0f),
    modelMatrices(std::vector<glm::mat4>(relativePositions.size(), glm::mat4(1.0f))) {
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
    basePosition += moveSpeed * deltaTime * glm::vec3(0.0f, 0.0f, -1.0f);
  } else if (direction == ModelMovement::DOWN) {
    basePosition += moveSpeed * deltaTime * glm::vec3(0.0f, 0.0f, 1.0f);
  } else if (direction == ModelMovement::LEFT) {
    basePosition += moveSpeed * deltaTime * glm::vec3(-1.0f, 0.0f, 0.0f);
  } else if (direction == ModelMovement::RIGHT) {
    basePosition += moveSpeed * deltaTime * glm::vec3(1.0f, 0.0f, 0.0f);
  }
  updateModelMatrices();

}

glm::mat4 Model::getModelMatrix(int index) {
  return modelMatrices[index];
}

void Model::updateModelMatrices() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(Scale, Scale, Scale));
  for (int i = 0; i < relativePositions.size(); i++) {
    modelMatrices[i] = glm::translate(model, glm::vec3(basePosition + relativePositions[i]));
  }
}
