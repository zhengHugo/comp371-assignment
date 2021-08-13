//
// Created by Hugo Zheng on 2021-08-12.
//

#ifndef COMP371_PROJECT_SRC_PUZZLE_H_
#define COMP371_PROJECT_SRC_PUZZLE_H_

#include <vector>
#include "Cube.h"

enum class Movement {
  UP, DOWN, LEFT, RIGHT
};

class Puzzle {
 private:
  // to represent the state of this puzzle; 0 represents the blank
  int state[9]{0, 1, 2, 3, 4, 5, 6, 7, 8};

  std::vector<Cube> bricks;

  glm::vec3 position{glm::vec3(0.0f)};
  glm::quat quaternion{glm::quat(glm::vec3(0.0f))};
  glm::mat4 modelMatrix{glm::mat4(1.0f)};

  /**
   * Update the position of each brick by current state
   */
  void updateBrickPositions();

  /**
   * Update the model matrix of this puzzle
   */
  void updateModelMatrix();

 public:

  explicit Puzzle(std::vector<Cube> &bricks);

  /**
   * A movement applied on this puzzle will change the state
   * @param movement up, down, left, or right
   */
  void move(Movement movement);


  /**
   * Draw this puzzle
   * @param shader
   * @param shaderHasMaterial whether the shader requires a material
   */
  void draw(Shader &shader, bool shaderHasMaterial);

};

#endif //COMP371_PROJECT_SRC_PUZZLE_H_
