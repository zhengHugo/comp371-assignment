//
// Created by Hugo Zheng on 2021-08-12.
//

#include "Puzzle.h"

Puzzle::Puzzle(std::vector<Cube*> &bricks) : bricks(bricks) {
  updateBrickPositions();
}

void Puzzle::move(Movement movement) {

}

void Puzzle::updateBrickPositions() {
  for (size_t i = 0; i < 9; i++) {
    // i = location index
    // state[i] - 1 = brick index at location i
    glm::vec3 brickPosition((float) (-1 + (int)i % 3), (float) (1 - (int)i / 3), 0.0f);
    if (state[i] != 0) {
      bricks[state[i] - 1]->setPosition(brickPosition);
    }
  }
}

void Puzzle::updateModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position));
  model = glm::mat4_cast(quaternion) * model;
  modelMatrix = model;
}

void Puzzle::draw(Shader &shader, bool hasTexture) {
  for(size_t i = 0; i < 8; i++){
    bricks[i]->setParentModelMatrix(modelMatrix);
    bricks[i]->draw(shader, hasTexture);
  }
}

void Puzzle::setPosition(glm::vec3 newPosition) {
  this->position = newPosition;
  updateModelMatrix();
}

