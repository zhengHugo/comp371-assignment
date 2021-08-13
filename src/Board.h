//
// Created by Hugo Zheng on 2021-08-12.
//

#ifndef COMP371_PROJECT_SRC_BOARD_H_
#define COMP371_PROJECT_SRC_BOARD_H_

#include "Puzzle.h"
class Board {
 private:
  Puzzle puzzles[6];

 public:
  void draw();
};

#endif //COMP371_PROJECT_SRC_BOARD_H_
