//
// Created by Hugo Zheng on 2021-08-12.
//

#include "Puzzle.h"
#include <algorithm>

extern float unitFlatCubeVertices[];

Puzzle::Puzzle(std::vector<Cube *> &bricks) : numberCubes(bricks) {
  updateNumberCubePositions();
}

Puzzle::Puzzle(std::vector<Material *> &materials) {
  numberCubes.clear();
  numberCubes.reserve(8);
  for (int i = 0; i < 8; i++) {
    numberCubes.push_back(new Cube(*materials[i], unitFlatCubeVertices));
  }
  updateNumberCubePositions();

}

void Puzzle::move(Movement movement) {
  auto iter = std::find(state, state + 9, 0);
  auto zeroIndex = std::distance(state, iter);

  if (movement == Movement::UP) {
    // switch 0 with the element below 0
    if (zeroIndex / 3 < 2) {
      // there is a number below 0
      auto indexBelow = zeroIndex + 3;
      std::swap(state[zeroIndex], state[indexBelow]);
    }

  } else if (movement == Movement::DOWN) {
    // switch 0 with the element above 0
    if (zeroIndex / 3 > 0) {
      // there is a number above 0
      auto indexAbove = zeroIndex - 3;
      std::swap(state[zeroIndex], state[indexAbove]);
    }

  } else if (movement == Movement::LEFT) {
    // switch 0 with the element to the right of 0
    if (zeroIndex % 3 < 2) {
      // there is a number to the right of 0
      auto indexRight = zeroIndex + 1;
      std::swap(state[zeroIndex], state[indexRight]);
    }

  } else if (movement == Movement::RIGHT) {
    // switch 0 with the element to the left of 0
    if (zeroIndex % 3 > 0) {
      // there is a number to the left of 0
      auto indexLeft = zeroIndex - 1;
      std::swap(state[zeroIndex], state[indexLeft]);
    }
  }
  updateNumberCubePositions();

}

void Puzzle::updateNumberCubePositions() {
  for (size_t i = 0; i < 9; i++) {
    // i = location index
    // state[i] - 1 = brick index at location i
    glm::vec3 brickPosition((float) (-1 + (int) i % 3), (float) (1 - (int) i / 3), 0.0f);
    if (state[i] != 0) {
      numberCubes[state[i] - 1]->setPosition(brickPosition);
    }
  }
}

void Puzzle::updateModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position));
  model = model * glm::mat4_cast(quaternion);
  modelMatrix = model;
}

void Puzzle::draw(Shader &shader, bool hasTexture) {
  for (size_t i = 0; i < 8; i++) {
    numberCubes[i]->setParentModelMatrix(parentModelMatrix * modelMatrix);
    numberCubes[i]->draw(shader, hasTexture);
  }
}

void Puzzle::setPosition(glm::vec3 newPosition) {
  this->position = newPosition;
  updateModelMatrix();
}

void Puzzle::setQuaternion(glm::quat _quaternion) {
  this->quaternion = _quaternion;
  updateModelMatrix();
}

void Puzzle::setParentModelMatrix(glm::mat4 _parentModelMatrix) {
  this->parentModelMatrix = _parentModelMatrix;
}

