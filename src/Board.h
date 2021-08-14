//
// Created by Hugo Zheng on 2021-08-12.
//

#ifndef COMP371_PROJECT_SRC_BOARD_H_
#define COMP371_PROJECT_SRC_BOARD_H_

#include "Puzzle.h"
class Board {
 private:
  std::vector<Puzzle *> puzzles;
  glm::vec3 position{glm::vec3(0.0f)};
  glm::quat quaternion{glm::quat(glm::vec3(0.0f))};
  glm::mat4 modelMatrix{glm::mat4(1.0f)};

  void updateModelMatrix();

 public:
  explicit Board(std::vector<Puzzle *> &_puzzles);

  void setPosition(glm::vec3 newPosition);
  void setQuaternion(glm::quat _quaternion);
  std::vector<Puzzle *> &getPuzzles();
  void draw(Shader &shader, bool shaderHasTexture);
};

#endif //COMP371_PROJECT_SRC_BOARD_H_
