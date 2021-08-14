//
// Created by Hugo Zheng on 2021-08-12.
//

#include "Cube.h"
#include <iostream>
//#include "geometryData.h"

//Cube::Cube() {
//}

extern float unitCubeVertices[];
/**
 * Initialize a unit cube with a provided material
 * @param material Material of the cube
 */
Cube::Cube(const Material &material) : material(material) {
  vertices = unitCubeVertices;
  updateModelMatrix();
}

/**
 * Initialize a cube with provided vertex data and material
 * @param vertices Float array of vertices. Format: [coords * 3, normal * 3, texCoords * 2] * 6 * 6
 * @param material
 */
Cube::Cube(Material &material, float *vertices) :
    material(material),
    vertices(vertices) {
  updateModelMatrix();
}

void Cube::updateModelMatrix() {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(position));
  model = glm::mat4_cast(quaternion) * model;
  model = glm::scale(model, glm::vec3(scale));
  modelMatrix = model;
}

glm::mat4 Cube::getModelMatrix() {
  return modelMatrix;
}

void Cube::draw(Shader &shader, bool shaderHasMaterial, bool isGlowingOn) {
  if (vao == 0) {
    unsigned int vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexArrayLength * (long) sizeof(float),
                 vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void *) (6 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
  }
  glBindVertexArray(vao);
  shader.use();
  shader.setMat4("model", getModelMatrix());
  if (shaderHasMaterial) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material.specular);
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setFloat("material.shininess", material.shininess);
    shader.setVec3("material.ambient", material.ambient);
    //glow effect: can be placed in any cube draw process
    shader.setBool("shouldGlow", isGlowingOn);
  }
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Cube::draw(Shader &shader, bool shaderHasMaterial) {
  draw(shader, shaderHasMaterial, false);
}

void Cube::setPosition(glm::vec3 newPosition) {
  this->position = newPosition;
  updateModelMatrix();
}

void Cube::setScale(float newScale){
  this->scale = newScale;
  updateModelMatrix();
}


