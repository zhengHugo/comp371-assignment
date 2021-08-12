//
// Created by Hugo Zheng on 2021-08-12.
//

#ifndef COMP371_PROJECT_SRC_CUBE_H_
#define COMP371_PROJECT_SRC_CUBE_H_

#include "Material.h"
#include <glm/gtc/quaternion.hpp>

class Cube {
 private:
  const long vertexArrayLength = 288;

  Material material;
  float *vertices;

  glm::vec3 position;
  glm::quat quaternion;
  float scale;

  glm::mat4 modelMatrix;

  void updateModelMatrix();

 public:
  explicit Cube(const Material &material);

  Cube(float *vertices, Material &material);

  glm::mat4 getModelMatrix();

  void draw(Shader &shader, bool shaderHasMaterial, bool shouldGlow);

  void draw(Shader &shader, bool shaderHasMaterial);
};

#endif //COMP371_PROJECT_SRC_CUBE_H_
