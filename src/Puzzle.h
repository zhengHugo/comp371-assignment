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
  int *state;

  int state0[9]{3, 1, 2, 4, 0, 8, 6, 5, 7};
  int state1[9]{1, 4, 2, 6, 3, 5, 0, 7, 8};
  int state2[9]{5, 6, 4, 1, 0, 3, 7, 8, 2};
  int state3[9]{4, 1, 0, 2, 5, 3, 6, 7, 8};
  int state4[9]{7, 0, 2, 5, 1, 8, 4, 3, 6};
  int state5[9]{1, 2, 3, 4, 5, 6, 7, 0, 8};
  int *states[6]={ state0 , state1 , state2 , state3 , state4, state5};

  int winState[9]{ 1, 2, 3, 4, 5, 6, 7, 8, 0};
  
  int initialStateNum = 0;
  bool isWin = false;
  //int* ptr = states[0];

  std::vector<Cube *> numberCubes;

  glm::vec3 position{glm::vec3(0.0f)};
  glm::quat quaternion{glm::quat(glm::vec3(0.0f))};
  glm::mat4 modelMatrix{glm::mat4(1.0f)};

  glm::mat4 parentModelMatrix{glm::mat4(1.0f)};

  /**
* Update the position of each brick by current state
*/
  void updateNumberCubePositions();

  /**
   * Update the model matrix of this puzzle
   */
  void updateModelMatrix();

 public:

  explicit Puzzle(std::vector<Cube *> &bricks);

  explicit Puzzle(std::vector<Material *> &materials,int layoutNum);

  float puzzleSec = 0.f;
  int puzzleMin = 0;
  int puzzleStep = 0;


  /**
   * A movement applied on this puzzle will change the state
   * @param movement up, down, left, or right
   */
  bool move(Movement movement);

  /**
   * Draw this puzzle
   * @param shader
   * @param shaderHasMaterial whether the shader requires a material
   */
  void draw(Shader &shader, bool shaderHasMaterial);

  void setPosition(glm::vec3 newPosition);

  void setQuaternion(glm::quat _quaternion);

  void setParentModelMatrix(glm::mat4 _parentModelMatrix);  
  
  void setInitialPos(int state[9]);

  bool getWinBool();

  void setWinBool();

  void resetAll();
 
};

#endif //COMP371_PROJECT_SRC_PUZZLE_H_
