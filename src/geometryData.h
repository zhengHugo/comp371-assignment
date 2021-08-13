//
// Created by Hugo Zheng on 2021-08-12.
//

#ifndef COMP371_PROJECT_SRC_GEOMETRYDATA_H_
#define COMP371_PROJECT_SRC_GEOMETRYDATA_H_

#include <vector>
#include <glm/glm.hpp>
// Geometry data
// -----------------------------------
float unitCubeVertices[] = {
    // unit cube vertices
    // Back face
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // Bottom-left
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // bottom-right
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top-left
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // bottom-left
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // Bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //  bottom-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //top-left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // bottom-left
    // Left face
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,// bottom-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,// bottom-left
    // Right face
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // top-left
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // bottom-right
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top-left
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
    // Top face
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // top-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
};

float unitFlatCubeVertices[] = {
    // unit Rubik Sheet vertices
    // Back face
    0.5f, -0.5f, -0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // Bottom-left
    -0.5f, -0.5f, -0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // bottom-right
    -0.5f, 0.5f, -0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.05f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, -0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top-left
    0.5f, -0.5f, -0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // bottom-left
    // Front face
    -0.5f, -0.5f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // Bottom-left
    0.5f, -0.5f, 0.05f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //  bottom-right
    0.5f, 0.5f, 0.05f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top-right
    0.5f, 0.5f, 0.05f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top-right
    -0.5f, 0.5f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //top-left
    -0.5f, -0.5f, 0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // bottom-left
    // Left face
    -0.5f, -0.5f, -0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, 0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// bottom-right
    -0.5f, 0.5f, 0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top-right
    -0.5f, 0.5f, 0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top-left
    -0.5f, -0.5f, -0.05f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// bottom-left
    // Right face
    0.5f, -0.5f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
    0.5f, 0.5f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top-right
    0.5f, 0.5f, -0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top-right
    0.5f, 0.5f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // top-left
    0.5f, -0.5f, 0.05f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    0.5f, -0.5f, -0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
    0.5f, -0.5f, 0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top-right
    0.5f, -0.5f, 0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top-right
    -0.5f, -0.5f, 0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // top-left
    -0.5f, -0.5f, -0.05f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    // Top face
    -0.5f, 0.5f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
    0.5f, 0.5f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
    0.5f, 0.5f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // top-right
    0.5f, 0.5f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // top-right
    -0.5f, 0.5f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // top-left
    -0.5f, 0.5f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
};

float unitGroundVertices[] = {
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left

    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left

    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left

    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left

    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left

    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
    -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
    -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left

};

float axisVertices[] = {
    0.0f, 0.0f, 0.0f, 0.20f, 0.20f, 0.20f, // pos * 3, color * 3
    4.3f, 0.0f, 0.0f, 0.696f, 0.298f, 0.192f,
    4.3f, 0.0f, 0.0f, 1.0f, 0.298f, 0.192f,
    5.0f, 0.0f, 0.0f, 1.0f, 0.298f, 0.192f,

    0.0f, 0.0f, 0.0f, 0.20f, 0.20f, 0.20f,
    0.0f, 4.3f, 0.0f, 0.235f, 0.594f, 0.286f,
    0.0f, 4.3f, 0.0f, 0.235f, 0.894f, 0.286f,
    0.0f, 5.0f, 0.0f, 0.235f, 0.894f, 0.286f,

    0.0f, 0.0f, 0.0f, 0.20f, 0.20f, 0.20f,
    0.0f, 0.0f, 4.3f, 0.235f, 0.494f, 0.586f,
    0.0f, 0.0f, 4.3f, 0.235f, 0.494f, 0.986f,
    0.0f, 0.0f, 5.0f, 0.235f, 0.494f, 0.986f,
};

//std::vector<glm::vec3> relativeCubePositions1 = {
//    glm::vec3(0.0f, 0.0f, 0.0f),
//    glm::vec3(0.0f, 2.0f, 0.0f),
//    glm::vec3(0.0f, 3.0f, 0.0f),
//    glm::vec3(1.0f, 3.0f, 0.0f),
//    glm::vec3(2.0f, 3.0f, 0.0f),
//    glm::vec3(2.0f, 4.0f, 0.0f),
//    glm::vec3(2.0f, 5.0f, 0.0f),
//    glm::vec3(1.0f, 5.0f, 0.0f),
//    glm::vec3(0.0f, 5.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeCubePositions2 = {
//    glm::vec3(0.0f, 0.0f, 0.0f),
//    glm::vec3(1.0f, 0.0f, 0.0f),
//    glm::vec3(0.0f, 0.0f, -1.0f),
//    glm::vec3(0.0f, 0.0f, -2.0f),
//    glm::vec3(0.0f, 0.0f, -3.0f),
//    glm::vec3(2.0f, 0.0f, 0.0f),
//    glm::vec3(3.0f, 0.0f, 0.0f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    glm::vec3(0.0f, 2.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeCubePositions3 = {
//    glm::vec3(0.0f, 0.0f, 0.0f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    glm::vec3(0.0f, 2.0f, 0.0f),
//    glm::vec3(0.0f, 3.0f, 0.0f),
//    glm::vec3(0.0f, 4.0f, 0.0f),
//    glm::vec3(-1.0f, 3.0f, 0.0f),
//    glm::vec3(1.0f, 3.0f, 0.0f),
//    glm::vec3(-2.0f, 2.0f, 0.0f),
//    glm::vec3(2.0f, 2.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeCubePositions4 = {
//    glm::vec3(0.0f, 0.0f, 0.0f),
//    glm::vec3(1.0f, 0.0f, 0.0f),
//    glm::vec3(2.0f, 0.0f, 0.0f),
//    glm::vec3(3.0f, 0.0f, 0.0f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    glm::vec3(0.0f, 2.0f, 0.0f),
//    glm::vec3(1.0f, 2.0f, 0.0f),
//    glm::vec3(2.0f, 2.0f, 0.0f),
//    glm::vec3(3.0f, 2.0f, 0.0f),
//    glm::vec3(3.0f, 2.0f, 1.0f),
//    glm::vec3(3.0f, 2.0f, 2.0f),
//    glm::vec3(2.0f, 2.0f, 2.0f),
//    glm::vec3(1.0f, 2.0f, 2.0f)
//};
//
//std::vector<glm::vec3> relativeCubePositions5 = {
//    glm::vec3(0.0f, 0.0f, 0.0f),
//    glm::vec3(1.0f, 0.0f, 0.0f),
//    glm::vec3(2.0f, 0.0f, 0.0f),
//    glm::vec3(3.0f, 0.0f, 0.0f),
//
//    glm::vec3(3.0f, 1.0f, 0.0f),
//    glm::vec3(3.0f, 2.0f, 0.0f),
//
//    glm::vec3(2.0f, 2.0f, 0.0f),
//    glm::vec3(1.0f, 2.0f, 0.0f),
//    glm::vec3(0.0f, 2.0f, 0.0f),
//
//    glm::vec3(0.0f, 3.0f, 0.0f),
//    glm::vec3(0.0f, 4.0f, 0.0f),
//
//    glm::vec3(1.0f, 4.0f, 0.0f),
//    glm::vec3(2.0f, 4.0f, 0.0f),
//    glm::vec3(3.0f, 4.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeWallPositions1 = {
//    glm::vec3(-1.0f, -1.0f, 0.0f),
//    glm::vec3(0.0f, -1.0f, 0.0f),
//    glm::vec3(1.0f, -1.0f, 0.0f),
//    glm::vec3(2.0f, -1.0f, 0.0f),
//    glm::vec3(3.0f, -1.0f, 0.0f),
//    glm::vec3(-1.0f, 0.0f, 0.0f),
//    glm::vec3(1.0f, 0.0f, 0.0f),
//    glm::vec3(2.0f, 0.0f, 0.0f),
//    glm::vec3(3.0f, 0.0f, 0.0f),
//    glm::vec3(-1.0f, 1.0f, 0.0f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    glm::vec3(1.0f, 1.0f, 0.0f),
//    glm::vec3(2.0f, 1.0f, 0.0f),
//    glm::vec3(3.0f, 1.0f, 0.0f),
//    glm::vec3(-1.0f, 2.0f, 0.0f),
//    glm::vec3(1.0f, 2.0f, 0.0f),
//    glm::vec3(2.0f, 2.0f, 0.0f),
//    glm::vec3(3.0f, 2.0f, 0.0f),
//    glm::vec3(-1.0f, 3.0f, 0.0f),
//    glm::vec3(3.0f, 3.0f, 0.0f),
//    glm::vec3(-1.0f, 4.0f, 0.0f),
//    glm::vec3(0.0f, 4.0f, 0.0f),
//    glm::vec3(1.0f, 4.0f, 0.0f),
//    glm::vec3(3.0f, 4.0f, 0.0f),
//    glm::vec3(-1.0f, 5.0f, 0.0f),
//    glm::vec3(3.0f, 5.0f, 0.0f),
//    glm::vec3(-1.0f, 6.0f, 0.0f),
//    glm::vec3(0.0f, 6.0f, 0.0f),
//    glm::vec3(1.0f, 6.0f, 0.0f),
//    glm::vec3(2.0f, 6.0f, 0.0f),
//    glm::vec3(3.0f, 6.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeWallPositions2 = {
//    glm::vec3(-1.0f, -1.0f, 0.0f),
//    glm::vec3(0.0f, -1.0f, 0.0f),
//    glm::vec3(1.0f, -1.0f, 0.0f),
//    glm::vec3(2.0f, -1.0f, 0.0f),
//    glm::vec3(3.0f, -1.0f, 0.0f),
//    glm::vec3(4.0f, -1.0f, 0.0f),
//    glm::vec3(-1.0f, 0.0f, 0.0f),
//    glm::vec3(4.0f, 0.0f, 0.0f),
//    glm::vec3(-1.0f, 1.0f, 0.0f),
//    glm::vec3(1.0f, 1.0f, 0.0f),
//    glm::vec3(2.0f, 1.0f, 0.0f),
//    glm::vec3(3.0f, 1.0f, 0.0f),
//    glm::vec3(4.0f, 1.0f, 0.0f),
//    glm::vec3(-1.0f, 2.0f, 0.0f),
//    glm::vec3(1.0f, 2.0f, 0.0f),
//    glm::vec3(2.0f, 2.0f, 0.0f),
//    glm::vec3(3.0f, 2.0f, 0.0f),
//    glm::vec3(4.0f, 2.0f, 0.0f),
//    glm::vec3(-1.0f, 3.0f, 0.0f),
//    glm::vec3(0.0f, 3.0f, 0.0f),
//    glm::vec3(1.0f, 3.0f, 0.0f),
//    glm::vec3(2.0f, 3.0f, 0.0f),
//    glm::vec3(3.0f, 3.0f, 0.0f),
//    glm::vec3(4.0f, 3.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeWallPositions3 = {
//    glm::vec3(-3.0f, -1.0f, 0.0f),
//    glm::vec3(-2.0f, -1.0f, 0.0f),
//    glm::vec3(-1.0f, -1.0f, 0.0f),
//    glm::vec3(0.0f, -1.0f, 0.0f),
//    glm::vec3(1.0f, -1.0f, 0.0f),
//    glm::vec3(2.0f, -1.0f, 0.0f),
//    glm::vec3(3.0f, -1.0f, 0.0f),
//    glm::vec3(-3.0f, 0.0f, 0.0f),
//    glm::vec3(-2.0f, 0.0f, 0.0f),
//    glm::vec3(-1.0f, 0.0f, 0.0f),
//    glm::vec3(1.0f, 0.0f, 0.0f),
//    glm::vec3(2.0f, 0.0f, 0.0f),
//    glm::vec3(3.0f, 0.0f, 0.0f),
//    glm::vec3(-3.0f, 1.0f, 0.0f),
//    glm::vec3(-2.0f, 1.0f, 0.0f),
//    glm::vec3(-1.0f, 1.0f, 0.0f),
//    glm::vec3(1.0f, 1.0f, 0.0f),
//    glm::vec3(2.0f, 1.0f, 0.0f),
//    glm::vec3(3.0f, 1.0f, 0.0f),
//    glm::vec3(-3.0f, 2.0f, 0.0f),
//    glm::vec3(-1.0f, 2.0f, 0.0f),
//    glm::vec3(1.0f, 2.0f, 0.0f),
//    glm::vec3(3.0f, 2.0f, 0.0f),
//    glm::vec3(-3.0f, 3.0f, 0.0f),
//    glm::vec3(-2.0f, 3.0f, 0.0f),
//    glm::vec3(2.0f, 3.0f, 0.0f),
//    glm::vec3(3.0f, 3.0f, 0.0f),
//    glm::vec3(-3.0f, 4.0f, 0.0f),
//    glm::vec3(-2.0f, 4.0f, 0.0f),
//    glm::vec3(-1.0f, 4.0f, 0.0f),
//    glm::vec3(1.0f, 4.0f, 0.0f),
//    glm::vec3(2.0f, 4.0f, 0.0f),
//    glm::vec3(3.0f, 4.0f, 0.0f),
//    glm::vec3(-3.0f, 5.0f, 0.0f),
//    glm::vec3(-2.0f, 5.0f, 0.0f),
//    glm::vec3(-1.0f, 5.0f, 0.0f),
//    glm::vec3(0.0f, 5.0f, 0.0f),
//    glm::vec3(1.0f, 5.0f, 0.0f),
//    glm::vec3(2.0f, 5.0f, 0.0f),
//    glm::vec3(3.0f, 5.0f, 0.0f),
//};
//
//std::vector<glm::vec3> relativeWallPositions4 = {
//    glm::vec3(-1.0f, -1.0f, 0.0f),
//    glm::vec3(0.0f, -1.0f, 0.0f),
//    glm::vec3(1.0f, -1.0f, 0.0f),
//    glm::vec3(2.0f, -1.0f, 0.0f),
//    glm::vec3(3.0f, -1.0f, 0.0f),
//    glm::vec3(4.0f, -1.0f, 0.0f),
//    glm::vec3(-1.0f, 0.0f, 0.0f),
//    glm::vec3(4.0f, 0.0f, 0.0f),
//    glm::vec3(-1.0f, 1.0f, 0.0f),
//    glm::vec3(1.0f, 1.0f, 0.0f),
//    glm::vec3(2.0f, 1.0f, 0.0f),
//    glm::vec3(3.0f, 1.0f, 0.0f),
//    glm::vec3(4.0f, 1.0f, 0.0f),
//    glm::vec3(-1.0f, 2.0f, 0.0f),
//    glm::vec3(4.0f, 2.0f, 0.0f),
//    glm::vec3(-1.0f, 3.0f, 0.0f),
//    glm::vec3(0.0f, 3.0f, 0.0f),
//    glm::vec3(1.0f, 3.0f, 0.0f),
//    glm::vec3(2.0f, 3.0f, 0.0f),
//    glm::vec3(3.0f, 3.0f, 0.0f),
//    glm::vec3(4.0f, 3.0f, 0.0f)
//};
//
//std::vector<glm::vec3> relativeWallPositions5 = {
//    glm::vec3(-1.0f, -1.0f, 0.0f),
//    glm::vec3(0.0f, -1.0f, 0.0f),
//    glm::vec3(1.0f, -1.0f, 0.0f),
//    glm::vec3(2.0f, -1.0f, 0.0f),
//    glm::vec3(3.0f, -1.0f, 0.0f),
//    glm::vec3(4.0f, -1.0f, 0.0f),
//
//    glm::vec3(-1.0f, 0.0f, 0.0f),
//    glm::vec3(4.0f, 0.0f, 0.0f),
//
//    glm::vec3(-1.0f, 1.0f, 0.0f),
//    glm::vec3(0.0f, 1.0f, 0.0f),
//    glm::vec3(1.0f, 1.0f, 0.0f),
//    glm::vec3(2.0f, 1.0f, 0.0f),
//    glm::vec3(4.0f, 1.0f, 0.0f),
//
//    glm::vec3(-1.0f, 2.0f, 0.0f),
//    glm::vec3(4.0f, 2.0f, 0.0f),
//
//    glm::vec3(-1.0f, 3.0f, 0.0f),
//    glm::vec3(1.0f, 3.0f, 0.0f),
//    glm::vec3(2.0f, 3.0f, 0.0f),
//    glm::vec3(3.0f, 3.0f, 0.0f),
//    glm::vec3(4.0f, 3.0f, 0.0f),
//
//    glm::vec3(-1.0f, 4.0f, 0.0f),
//    glm::vec3(4.0f, 4.0f, 0.0f),
//
//    glm::vec3(-1.0f, 5.0f, 0.0f),
//    glm::vec3(0.0f, 5.0f, 0.0f),
//    glm::vec3(1.0f, 5.0f, 0.0f),
//    glm::vec3(2.0f, 5.0f, 0.0f),
//    glm::vec3(3.0f, 5.0f, 0.0f),
//    glm::vec3(4.0f, 5.0f, 0.0f),
//
//};
#endif //COMP371_PROJECT_SRC_GEOMETRYDATA_H_
