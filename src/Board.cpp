//
// Created by Hugo Zheng on 2021-08-12.
//

#include "Board.h"
Board::Board(std::vector<Puzzle *> &_puzzles) : puzzles(_puzzles) {
  /*             3
   *       /----------/
   *      /     2    /|
   *     /----------/ |
   *     |          |1|
   *  4  |    0     | |
   *     |          | /
   *     |----------|/
   *          5
   */
  puzzles[0]->setPosition(glm::vec3(0.0f, 0.0f, 1.5f));
  puzzles[0]->setQuaternion(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
  puzzles[1]->setPosition(glm::vec3(1.5f, 0.0f, 0.0f));
  puzzles[1]->setQuaternion(glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f)));
  puzzles[2]->setPosition(glm::vec3(0.0f, 1.5f, 0.0f));
  puzzles[2]->setQuaternion(glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f)));
  puzzles[3]->setPosition(glm::vec3(0.0f, 0.0f, -1.5f));
  puzzles[3]->setQuaternion(glm::quat(glm::vec3(0.0f, glm::radians(180.0f), 0.0f)));
  puzzles[4]->setPosition(glm::vec3(-1.5f, 0.0f, 0.0f));
  puzzles[4]->setQuaternion(glm::quat(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f)));
  puzzles[5]->setPosition(glm::vec3(0.0f, -1.5f, 0.0f));
  puzzles[5]->setQuaternion(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f)));
}

void Board::draw(Shader &shader, bool shaderHasTexture) {
  for (auto &puzzle : puzzles) {
    puzzle->setParentModelMatrix(modelMatrix);
    puzzle->draw(shader, shaderHasTexture);
  }
}

void Board::updateModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position));
  model = model * glm::mat4_cast(quaternion);
  modelMatrix = model;
}

void Board::setQuaternion(glm::quat _quaternion) {
  this->quaternion = _quaternion;
  updateModelMatrix();
}

void Board::setPosition(glm::vec3 newPosition) {
  this->position = newPosition;
  updateModelMatrix();
}
std::vector<Puzzle *> &Board::getPuzzles() {
  return puzzles;
}
