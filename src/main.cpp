#define GLEW_STATIC 1   //This allows linking with Static Library on Windows, without DLL
#define STB_IMAGE_IMPLEMENTATION // Enable stb library

#include <GL/glew.h>   // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Material.h"
#include "PointLight.h"

#pragma region Declare static functions

static void clearError();

static void checkError();

static void processInput(GLFWwindow *window);

static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void updateModelPosition();

static unsigned int loadTexture(const char *path);

static void processInput();

static void readFloatArrayFromFile(const std::string &path);

static void renderScene(Shader &shader);

#pragma endregion // declare functions

#pragma region Declare global variables

Camera *camera;
Model *model1;
Model *model2;
Model *model3;
Model *model4;
Model *model5;

Model *wall1;
Model *wall2;
Model *wall3;
Model *wall4;
Model *wall5;

Model *lightBoxModel;
Model *groundModel;
Model *currentWall;
Model *currentModel;
std::vector<Model *> cornerObjects;

//toggle value
int isGlow = 0;
int isTexture = 1;
int isShadow = 1;

// window width & height
int scrWidth = 1024;
int scrHeight = 768;
glm::vec3 *cornerPositions;

// true when mouse callback is called for the first time; false otherwise
bool firstMouse = true;
float lastX = (float) scrWidth / 2.0f;
float lastY = (float) scrHeight / 2.0f;

float deltaTime; // Time between current frame and last frame
float lastFrame; // Time of last frame

#pragma endregion // declare global variables

int main(int argc, char *argv[]) {

  FT_Library ft;
  if (FT_Init_FreeType(&ft))
  {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return -1;
  }

  // Initializations
  // --------------------------

  // Initialize GLFW and OpenGL version
  glfwInit();

#if __APPLE__
  // On apple, enable OpenGL core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  // On windows, we set OpenGL version to 2.1, to support more hardware
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
  //Anti Aliasing
  glfwWindowHint(GLFW_SAMPLES, 8);
  // Create Window and rendering context using GLFW, resolution is 800x600
  GLFWwindow *window = glfwCreateWindow(scrWidth,
                                        scrHeight,
                                        "Comp371 - Assignment 01",
                                        nullptr,
                                        nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // bind input callbacks
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetKeyCallback(window, keyCallback);

  // tell GLFW to capture our mouse
//  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to create GLEW" << std::endl;
    glfwTerminate();
    return -1;
  }

  // enable MULTISAMPLE
  glEnable(GL_MULTISAMPLE);
  // enable depth info
  glEnable(GL_DEPTH_TEST);

  camera = new Camera();

  // Geometry data
  // -----------------------------------
  float unitCubeVertices[] = {
      // unit cube vertices
      // TODO: read data from file
      // Back face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      // Front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
      // Left face
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,// bottom-left
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// bottom-right
      // Right face
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //  top-left
      // Bottom face
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom-left
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom-left
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
      // Top face
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f // top-left
  };

  float unitGroundVertices[] = {
      -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,  // top-left
      50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,  // top-right
      50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
      50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,  // bottom-right
      -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
      -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f  // top-left
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

  std::vector<glm::vec3> relativeCubePositions1 = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 2.0f, 0.0f),
      glm::vec3(0.0f, 3.0f, 0.0f),
      glm::vec3(1.0f, 3.0f, 0.0f),
      glm::vec3(2.0f, 3.0f, 0.0f),
      glm::vec3(2.0f, 4.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, 0.0f),
      glm::vec3(1.0f, 5.0f, 0.0f),
      glm::vec3(0.0f, 5.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeCubePositions2 = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, -1.0f),
      glm::vec3(0.0f, 0.0f, -2.0f),
      glm::vec3(0.0f, 0.0f, -3.0f),
      glm::vec3(2.0f, 0.0f, 0.0f),
      glm::vec3(3.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 2.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeCubePositions3 = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 2.0f, 0.0f),
      glm::vec3(0.0f, 3.0f, 0.0f),
      glm::vec3(0.0f, 4.0f, 0.0f),
      glm::vec3(-1.0f, 3.0f, 0.0f),
      glm::vec3(1.0f, 3.0f, 0.0f),
      glm::vec3(-2.0f, 2.0f, 0.0f),
      glm::vec3(2.0f, 2.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeCubePositions4 = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 0.0f, 0.0f),
      glm::vec3(3.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 2.0f, 0.0f),
      glm::vec3(1.0f, 2.0f, 0.0f),
      glm::vec3(2.0f, 2.0f, 0.0f),
      glm::vec3(3.0f, 2.0f, 0.0f),
      glm::vec3(3.0f, 2.0f, 1.0f),
      glm::vec3(3.0f, 2.0f, 2.0f),
      glm::vec3(2.0f, 2.0f, 2.0f),
      glm::vec3(1.0f, 2.0f, 2.0f)
  };

  std::vector<glm::vec3> relativeCubePositions5 = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 0.0f, 0.0f),
      glm::vec3(3.0f, 0.0f, 0.0f),

      glm::vec3(3.0f, 1.0f, 0.0f),
      glm::vec3(3.0f, 2.0f, 0.0f),

      glm::vec3(2.0f, 2.0f, 0.0f),
      glm::vec3(1.0f, 2.0f, 0.0f),
      glm::vec3(0.0f, 2.0f, 0.0f),

      glm::vec3(0.0f, 3.0f, 0.0f),
      glm::vec3(0.0f, 4.0f, 0.0f),

      glm::vec3(1.0f, 4.0f, 0.0f),
      glm::vec3(2.0f, 4.0f, 0.0f),
      glm::vec3(3.0f, 4.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeWallPositions1 = {
      glm::vec3(-1.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(1.0f, -1.0f, 0.0f),
      glm::vec3(2.0f, -1.0f, 0.0f),
      glm::vec3(3.0f, -1.0f, 0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 0.0f, 0.0f),
      glm::vec3(3.0f, 0.0f, 0.0f),
      glm::vec3(-1.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(2.0f, 1.0f, 0.0f),
      glm::vec3(3.0f, 1.0f, 0.0f),
      glm::vec3(-1.0f, 2.0f, 0.0f),
      glm::vec3(1.0f, 2.0f, 0.0f),
      glm::vec3(2.0f, 2.0f, 0.0f),
      glm::vec3(3.0f, 2.0f, 0.0f),
      glm::vec3(-1.0f, 3.0f, 0.0f),
      glm::vec3(3.0f, 3.0f, 0.0f),
      glm::vec3(-1.0f, 4.0f, 0.0f),
      glm::vec3(0.0f, 4.0f, 0.0f),
      glm::vec3(1.0f, 4.0f, 0.0f),
      glm::vec3(3.0f, 4.0f, 0.0f),
      glm::vec3(-1.0f, 5.0f, 0.0f),
      glm::vec3(3.0f, 5.0f, 0.0f),
      glm::vec3(-1.0f, 6.0f, 0.0f),
      glm::vec3(0.0f, 6.0f, 0.0f),
      glm::vec3(1.0f, 6.0f, 0.0f),
      glm::vec3(2.0f, 6.0f, 0.0f),
      glm::vec3(3.0f, 6.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeWallPositions2 = {
      glm::vec3(-1.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(1.0f, -1.0f, 0.0f),
      glm::vec3(2.0f, -1.0f, 0.0f),
      glm::vec3(3.0f, -1.0f, 0.0f),
      glm::vec3(4.0f, -1.0f, 0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(4.0f, 0.0f, 0.0f),
      glm::vec3(-1.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(2.0f, 1.0f, 0.0f),
      glm::vec3(3.0f, 1.0f, 0.0f),
      glm::vec3(4.0f, 1.0f, 0.0f),
      glm::vec3(-1.0f, 2.0f, 0.0f),
      glm::vec3(1.0f, 2.0f, 0.0f),
      glm::vec3(2.0f, 2.0f, 0.0f),
      glm::vec3(3.0f, 2.0f, 0.0f),
      glm::vec3(4.0f, 2.0f, 0.0f),
      glm::vec3(-1.0f, 3.0f, 0.0f),
      glm::vec3(0.0f, 3.0f, 0.0f),
      glm::vec3(1.0f, 3.0f, 0.0f),
      glm::vec3(2.0f, 3.0f, 0.0f),
      glm::vec3(3.0f, 3.0f, 0.0f),
      glm::vec3(4.0f, 3.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeWallPositions3 = {
      glm::vec3(-3.0f, -1.0f, 0.0f),
      glm::vec3(-2.0f, -1.0f, 0.0f),
      glm::vec3(-1.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(1.0f, -1.0f, 0.0f),
      glm::vec3(2.0f, -1.0f, 0.0f),
      glm::vec3(3.0f, -1.0f, 0.0f),
      glm::vec3(-3.0f, 0.0f, 0.0f),
      glm::vec3(-2.0f, 0.0f, 0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 0.0f, 0.0f),
      glm::vec3(3.0f, 0.0f, 0.0f),
      glm::vec3(-3.0f, 1.0f, 0.0f),
      glm::vec3(-2.0f, 1.0f, 0.0f),
      glm::vec3(-1.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(2.0f, 1.0f, 0.0f),
      glm::vec3(3.0f, 1.0f, 0.0f),
      glm::vec3(-3.0f, 2.0f, 0.0f),
      glm::vec3(-1.0f, 2.0f, 0.0f),
      glm::vec3(1.0f, 2.0f, 0.0f),
      glm::vec3(3.0f, 2.0f, 0.0f),
      glm::vec3(-3.0f, 3.0f, 0.0f),
      glm::vec3(-2.0f, 3.0f, 0.0f),
      glm::vec3(2.0f, 3.0f, 0.0f),
      glm::vec3(3.0f, 3.0f, 0.0f),
      glm::vec3(-3.0f, 4.0f, 0.0f),
      glm::vec3(-2.0f, 4.0f, 0.0f),
      glm::vec3(-1.0f, 4.0f, 0.0f),
      glm::vec3(1.0f, 4.0f, 0.0f),
      glm::vec3(2.0f, 4.0f, 0.0f),
      glm::vec3(3.0f, 4.0f, 0.0f),
      glm::vec3(-3.0f, 5.0f, 0.0f),
      glm::vec3(-2.0f, 5.0f, 0.0f),
      glm::vec3(-1.0f, 5.0f, 0.0f),
      glm::vec3(0.0f, 5.0f, 0.0f),
      glm::vec3(1.0f, 5.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, 0.0f),
      glm::vec3(3.0f, 5.0f, 0.0f),
  };

  std::vector<glm::vec3> relativeWallPositions4 = {
      glm::vec3(-1.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(1.0f, -1.0f, 0.0f),
      glm::vec3(2.0f, -1.0f, 0.0f),
      glm::vec3(3.0f, -1.0f, 0.0f),
      glm::vec3(4.0f, -1.0f, 0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(4.0f, 0.0f, 0.0f),
      glm::vec3(-1.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(2.0f, 1.0f, 0.0f),
      glm::vec3(3.0f, 1.0f, 0.0f),
      glm::vec3(4.0f, 1.0f, 0.0f),
      glm::vec3(-1.0f, 2.0f, 0.0f),
      glm::vec3(4.0f, 2.0f, 0.0f),
      glm::vec3(-1.0f, 3.0f, 0.0f),
      glm::vec3(0.0f, 3.0f, 0.0f),
      glm::vec3(1.0f, 3.0f, 0.0f),
      glm::vec3(2.0f, 3.0f, 0.0f),
      glm::vec3(3.0f, 3.0f, 0.0f),
      glm::vec3(4.0f, 3.0f, 0.0f)
  };

  std::vector<glm::vec3> relativeWallPositions5 = {
      glm::vec3(-1.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(1.0f, -1.0f, 0.0f),
      glm::vec3(2.0f, -1.0f, 0.0f),
      glm::vec3(3.0f, -1.0f, 0.0f),
      glm::vec3(4.0f, -1.0f, 0.0f),

      glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(4.0f, 0.0f, 0.0f),

      glm::vec3(-1.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(2.0f, 1.0f, 0.0f),
      glm::vec3(4.0f, 1.0f, 0.0f),

      glm::vec3(-1.0f, 2.0f, 0.0f),
      glm::vec3(4.0f, 2.0f, 0.0f),

      glm::vec3(-1.0f, 3.0f, 0.0f),
      glm::vec3(1.0f, 3.0f, 0.0f),
      glm::vec3(2.0f, 3.0f, 0.0f),
      glm::vec3(3.0f, 3.0f, 0.0f),
      glm::vec3(4.0f, 3.0f, 0.0f),

      glm::vec3(-1.0f, 4.0f, 0.0f),
      glm::vec3(4.0f, 4.0f, 0.0f),

      glm::vec3(-1.0f, 5.0f, 0.0f),
      glm::vec3(0.0f, 5.0f, 0.0f),
      glm::vec3(1.0f, 5.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, 0.0f),
      glm::vec3(3.0f, 5.0f, 0.0f),
      glm::vec3(4.0f, 5.0f, 0.0f),

  };

  cornerPositions = new glm::vec3[]{
      glm::vec3(-30.0, 3.0f, 30.0f),
      glm::vec3(30.0f, 3.0f, 30.0f),
      glm::vec3(30.0f, 3.0f, -30.0f),
      glm::vec3(-30.0f, 3.0f, -30.0f),
  };

  std::vector<glm::vec3> relativeLightBoxPosition{
      glm::vec3(0, 0, 0)
  };
  std::vector<glm::vec3> relativeGroundPosition{
      glm::vec3(0, 0, 0)
  };

  glm::vec3 groundPosition(0, 0, 0);
  glm::vec3 pointLightPosition(3.0f, 30.0f, 6.0f);
//  glm::vec3 pointLightPosition(20.0f, 20.0f, 20.0f);
  glm::vec3 baseCubePosition(2.0f, 3.0f, 5.0f);
  glm::vec3 baseWallPosition(2.0f, 3.0f, 2.0f);

  // build models
  // --------------------------------------------------------
  model1 = new Model(baseCubePosition, relativeCubePositions1);
  model2 = new Model(baseCubePosition, relativeCubePositions2);
  model3 = new Model(baseCubePosition, relativeCubePositions3);
  model4 = new Model(baseCubePosition, relativeCubePositions4);
  model5 = new Model(baseCubePosition, relativeCubePositions5);
  wall1 = new Model(baseWallPosition, relativeWallPositions1);
  wall2 = new Model(baseWallPosition, relativeWallPositions2);
  wall3 = new Model(baseWallPosition, relativeWallPositions3);
  wall4 = new Model(baseWallPosition, relativeWallPositions4);
  wall5 = new Model(baseWallPosition, relativeWallPositions5);
  lightBoxModel = new Model(pointLightPosition, relativeLightBoxPosition);
  groundModel = new Model(groundPosition, relativeGroundPosition);
  // set model position
  currentModel = model1;
  currentWall = wall1;
  cornerObjects = {model2, wall2, model3, wall3, model4, wall4, model5, wall5};
  updateModelPosition();

  // configure light
  // ---------------------------------------
  PointLight pointLight = PointLight(pointLightPosition,
                                     glm::vec3(glm::radians(0.0f), 0, 0),
                                     glm::vec3(1.0f, 1.0f, 0.88f) * 80.0f
  );


  // build and compile shader
  // -----------------------------------------------------
  Shader cubeShader("res/shader/CubeVertex.shader",
                    "res/shader/CubeFragment.shader");
  Shader lineShader("res/shader/LineVertex.shader",
                    "res/shader/LineFragment.shader");
  Shader depthMappingShader("res/shader/DepthMappingVertex.shader",
                            "res/shader/DepthMappingFragment.shader");

  // create and bind depth FBO
  // -----------------------------------------------
  unsigned int depthMapFbo;
  glGenFramebuffers(1, &depthMapFbo);
  // create a texture to store the depth info
  const unsigned int SHADOW_WIDTH = 1024;
  const unsigned int SHADOW_HEIGHT = 1024;
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth map to the frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE); // do not render color data
  glReadBuffer(GL_NONE); // do not render color data
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  // Binding geometry data
  // ------------------------------------
  unsigned int cubeVao, axisVao, groundVao, cubeVbo, axisVbo, groundVbo;
  glGenVertexArrays(1, &cubeVao);
  glGenVertexArrays(1, &axisVao);
  glGenVertexArrays(1, &groundVao);
  glGenBuffers(1, &cubeVbo);
  glGenBuffers(1, &axisVbo);
  glGenBuffers(1, &groundVbo);

  // bind cube data
  glBindVertexArray(cubeVao);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(unitCubeVertices), unitCubeVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  // bind ground data
  glBindVertexArray(groundVao);

  glBindBuffer(GL_ARRAY_BUFFER, groundVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(unitGroundVertices), unitGroundVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  // bind axis data
  glBindVertexArray(axisVao);
  glBindBuffer(GL_ARRAY_BUFFER, axisVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // configure materials
  Material metal(cubeShader,
                 loadTexture("res/texture/metal.png"),
                 loadTexture("res/texture/metal_specular.png"),
                 glm::vec3(0.2f, 0.2f, 0.2f),
                 8.0f);

  Material brick(cubeShader,
                 loadTexture("res/texture/brick.png"),
                 loadTexture("res/texture/brick_specular.png"),
                 glm::vec3(1.0f, 1.0f, 1.0f),
                 64.0f);

  Material lightBox(cubeShader,
                    loadTexture("res/texture/sea_lantern.png"),
                    loadTexture("res/texture/sea_lantern.png"),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    1.0f);

  Material tile(cubeShader,
                loadTexture("res/texture/tile.png"),
                loadTexture("res/texture/tile_specular.png"),
                glm::vec3(1.0f, 1.0f, 1.0f),
                128.0f);

  unsigned int emissionMap;
  emissionMap = loadTexture("res/texture/Emission.png");

  //Anisotropic texture filtering
  //get the maximum Anisotropic filtering level your PC supports.
  GLfloat fLargest;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
  // turn on "AF"
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

  //Cull_Face
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  // Main Loop
  // ----------------------------------
  while (!glfwWindowShouldClose(window)) {
    clearError();
    // update time
    auto currentFrame = (float) glfwGetTime();
    float timeValueForColor = sin(currentFrame) / 2.0f + 0.5f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // process input
    processInput(window);
    // Each frame, reset color of each pixel to glClearColor
    glClearColor(0.035f, 0.149f, 0.1098f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render depth scene to texture from the light point
    // ------------------------------------------------------
    float nearPlane = 5.0f;
    float farPlane = 40.0f;
    glm::mat4 lightProjection = glm::perspective(glm::radians(85.0f),
                                                 (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT,
                                                 nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(pointLight.position, baseCubePosition,
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    // render the scene
    depthMappingShader.use();
    depthMappingShader.setFloat("nearPlane", nearPlane);
    depthMappingShader.setFloat("farPlane", farPlane);
    depthMappingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    // draw model and wall
    glBindVertexArray(cubeVao);
    for (size_t i = 0; i < currentModel->size(); i++) {
      glm::mat4 cubeModelMatrix = currentModel->getModelMatrix(i);
      depthMappingShader.setMat4("model", cubeModelMatrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    for (size_t i = 0; i < currentWall->size(); i++) {
      glm::mat4 wallModelMatrix = currentWall->getModelMatrix(i);
      depthMappingShader.setMat4("model", wallModelMatrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // draw ground
    glBindVertexArray(groundVao);
    depthMappingShader.setMat4("model", groundModel->getModelMatrix(0));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // render the normal scene
    // -----------------------------------------------------------------------
    // get camera matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera->Fov),
                                            (float) scrWidth / (float) scrHeight,
                                            0.1f,
                                            100.f);
    glm::mat4 view = camera->getViewMatrix();

    // draw model
    // ---------------------------------
    // load shader uniforms
    cubeShader.use();
    cubeShader.setVec3("cameraPos", camera->Position);
    cubeShader.setMat4("view", view);
    cubeShader.setMat4("projection", projection);
    cubeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    cubeShader.setVec3("ambientColor", glm::vec3(0.3f));
    cubeShader.setVec3("pointLight.pos", pointLight.position);
    cubeShader.setVec3("pointLight.lightDir", pointLight.direction);
    cubeShader.setVec3("pointLight.color", pointLight.color);
    cubeShader.setFloat("pointLight.constant", pointLight.constant);
    cubeShader.setFloat("pointLight.linear", pointLight.linear);
    cubeShader.setFloat("pointLight.quadratic", pointLight.quadratic);
    cubeShader.setInt("emissionMap", 2);
    cubeShader.setInt("shadowMap", 3);
    cubeShader.setFloat("timeValue", timeValueForColor);
    cubeShader.setInt("toggleTexture", isTexture);
    cubeShader.setInt("toggleShadow", isShadow);
    glBindVertexArray(cubeVao);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    for (size_t i = 0; i < currentModel->size(); i++) {
      // assign cube texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, metal.diffuse);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, metal.specular);
      cubeShader.setInt("material.diffuse", 0);
      cubeShader.setInt("material.specular", 1);
      //glow effect: can be placed in any cube draw process
      cubeShader.setInt("toggleGlow", isGlow);
      cubeShader.setFloat("material.shininess", metal.shininess);
      cubeShader.setVec3("material.ambient", metal.ambient);
      // calculate cube model matrix
      glm::mat4 cubeModelMatrix = currentModel->getModelMatrix(i);
      cubeShader.setMat4("model", cubeModelMatrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    cubeShader.setInt("toggleGlow", 0);

    // draw wall
    for (size_t i = 0; i < currentWall->size(); i++) {
      // assign wall texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, brick.diffuse);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, brick.specular);
      cubeShader.setInt("material.diffuse", 0);
      cubeShader.setInt("material.specular", 1);
      cubeShader.setFloat("material.shininess", brick.shininess);
      cubeShader.setVec3("material.ambient", brick.ambient);
      // calculate the model matrix for wall
      glm::mat4 wallModelMatrix = currentWall->getModelMatrix(i);
      cubeShader.setMat4("model", wallModelMatrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // draw corner models
    for (int j = 0; j < (cornerObjects.size() / 2); j++) {
      // draw model (at cornerObjects[] even indices)
      for (size_t i = 0; i < cornerObjects[2 * j]->size(); i++) {
        // assign cube texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metal.diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metal.specular);
        cubeShader.setInt("material.diffuse", 0);
        cubeShader.setInt("material.specular", 1);
        cubeShader.setFloat("material.shininess", metal.shininess);
        cubeShader.setVec3("material.ambient", metal.ambient);
        glm::mat4 cubeModelMatrix = cornerObjects[2 * j]->getModelMatrix(i);
        cubeShader.setMat4("model", cubeModelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
      // draw wall (at cornerObjects[] odd indices)
      for (size_t i = 0; i < cornerObjects[2 * j + 1]->size(); i++) {
        // assign wall texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brick.diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, brick.specular);
        cubeShader.setInt("material.diffuse", 0);
        cubeShader.setInt("material.specular", 1);
        cubeShader.setFloat("material.shininess", brick.shininess);
        cubeShader.setVec3("material.ambient", brick.ambient);
        glm::mat4 wallModelMatrix = cornerObjects[2 * j + 1]->getModelMatrix(i);
        cubeShader.setMat4("model", wallModelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
    // draw a light box to indicate light position
    // assign light box texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lightBox.diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, lightBox.specular);
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    //not going to change
    cubeShader.setInt("toggleLightBox", 1);
    cubeShader.setFloat("material.shininess", lightBox.shininess);
    // calculate the model matrix for wall
    glm::mat4 cubeModelMatrix = lightBoxModel->getModelMatrix(0);
    cubeShader.setMat4("model", cubeModelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeShader.setInt("toggleLightBox", 0);
    // end of light box



    //draw ground
    glBindVertexArray(groundVao);
    // assign ground texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tile.diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tile.specular);
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setFloat("material.shininess", tile.shininess);
    cubeShader.setVec3("material.ambient", tile.ambient);
    cubeShader.setMat4("model", groundModel->getModelMatrix(0));
    glDrawArrays(GL_TRIANGLES, 0, 6);


    // draw axis
#if __APPLE__
    // line width is only available on window
#else
    // On windows, set line width to get a better view
    glLineWidth(5.0f);
#endif
    glBindVertexArray(axisVao);
    lineShader.use();
    lineShader.setMat4("projection", projection);
    lineShader.setMat4("view", view);
    lineShader.setMat4("model", glm::mat4(1.0f));
    for (int i = 0; i < 10; i += 2) {
      glDrawArrays(GL_LINES, i, 2);
      glDrawArrays(GL_LINES, i + 2, 2);
    }
    glLineWidth(1.0f);

    // End frame
    glfwSwapBuffers(window);
    // Detect inputs
    glfwPollEvents();

  }

  // deallocate resources
  glDeleteVertexArrays(1, &cubeVao);
  glDeleteVertexArrays(1, &axisVao);
  glDeleteVertexArrays(1, &groundVao);
  glDeleteBuffers(1, &cubeVbo);
  glDeleteBuffers(1, &axisVbo);
  glDeleteBuffers(1, &groundVbo);

  // camera
  delete camera;
  // models
  delete model1;
  delete model2;
  delete model3;
  delete model4;
  delete model5;
  delete lightBoxModel;
  delete groundModel;
  // walls
  delete wall1;
  delete wall2;
  delete wall3;
  delete wall4;
  delete wall5;
  // positions
  delete cornerPositions;
  // Shutdown GLFW
  glfwTerminate();
  return 0;
}

#pragma region Helper functions

static void clearError() {
  while (glGetError() != GL_NO_ERROR);
}

static void checkError() {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec << ")"
              << std::endl;
  }
}

static unsigned int loadTexture(const char *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format = GL_RGB;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, (int) format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

static void processInput(GLFWwindow *window) {
  // Escape
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  // 1-5: switch models
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    currentModel = model1;
    currentWall = wall1;
    cornerObjects = {model2, wall2, model3, wall3, model4, wall4, model5, wall5};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    currentModel = model2;
    currentWall = wall2;
    cornerObjects = {model1, wall1, model3, wall3, model4, wall4, model5, wall5};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
    currentModel = model3;
    currentWall = wall3;
    cornerObjects = {model1, wall1, model2, wall2, model4, wall4, model5, wall5};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
    currentModel = model4;
    currentWall = wall4;
    cornerObjects = {model1, wall1, model2, wall2, model3, wall3, model5, wall5};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
    currentModel = model5;
    currentWall = wall5;
    cornerObjects = {model1, wall1, model2, wall2, model3, wall3, model4, wall4};
    updateModelPosition();
  }

  // u: scale up model
  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
    currentModel->scaleUp(deltaTime);
  }

  // j: scale down model
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
    currentModel->scaleDown(deltaTime);
  }

  // =: scale up model and wall
  if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
    currentWall->scaleUp(deltaTime);
    currentModel->scaleUp(deltaTime);
  }

  // -: scale down model and wall
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
    currentWall->scaleDown(deltaTime);
    currentModel->scaleDown(deltaTime);
  }

  // shift + w: move up model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
      || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    currentModel->move(ModelMovement::UP, deltaTime);
  }

  // shift + s: move down model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
      || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    currentModel->move(ModelMovement::DOWN, deltaTime);
  }

  // shift + a: move left model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
      || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
  }


  // a
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      // shift + a: move left model
      currentModel->move(ModelMovement::LEFT, deltaTime);
    } else {
      // a: rotate left about y-axis
      currentModel->rotate(glm::radians(-90.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f),
                           deltaTime);
    }
  }

  // d
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      // shift + d: move right model
      currentModel->move(ModelMovement::RIGHT, deltaTime);
    } else {
      // d: rotate right about y-axis
      currentModel->rotate(glm::radians(90.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f),
                           deltaTime);
    }
  }

  // w
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      // shift + w: move left model
      currentModel->move(ModelMovement::UP, deltaTime);
    } else {
      // w: rotate left about x-axis
      currentModel->rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f), deltaTime);
    }
  }

  // s
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      // shift + s: move left model
      currentModel->move(ModelMovement::DOWN, deltaTime);
    } else {
      // s: rotate left about x-axis
      currentModel->rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f), deltaTime);
    }
  }

  // q
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      // shift + q: move forward model
      currentModel->move(ModelMovement::FORWARD, deltaTime);
    } else {
      // q: rotate left about z-axis
      currentModel->rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f), deltaTime);
    }
  }

  // e
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
      // shift + e: move left model
      currentModel->move(ModelMovement::BACKWARD, deltaTime);
    } else {
      // e: rotate left about z-axis
      currentModel->rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f), deltaTime);
    }
  }

  // left: world rotate x -> camera rotate -x
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    camera->rotate(glm::vec3(-1.0f, 0.0f, 0.0f), deltaTime);
  }

  // right: world rotate -x -> camera rotate x
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    camera->rotate(glm::vec3(1.0f, 0.0f, 0.0f), deltaTime);
  }

  // up: world rotate y -> camera rotate -y
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    camera->rotate(glm::vec3(0.0f, -1.0f, 0.0f), deltaTime);
  }

  // down: world rotate -y -> camera rotate y
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    camera->rotate(glm::vec3(0.0f, 1.0f, 0.0f), deltaTime);
  }

  // p
  if (glfwGetKey(window, GLFW_KEY_P)
      == GLFW_PRESS) {         //translate to point view
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glPointSize(5.0f);
  }
    // l
  else if (glfwGetKey(window, GLFW_KEY_L)
      == GLFW_PRESS) {  //translate to line view
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
    // t
  else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {  // reset
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  }
  // home: go home
  if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {
    camera->goHome();
  }
  // space: Repositioning the Model
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    updateModelPosition();
  }

}

static void updateModelPosition() {
  currentModel->resetPosition();
  currentWall->resetPosition();
  currentModel->resetOrientation();
  currentWall->resetOrientation();
  for (int i = 0; i < 8; i++) {
    cornerObjects[i]->setBasePosition(cornerPositions[i / 2]);
  }
}

static void renderScene(const Shader &shader) {
  shader.use();

}

//unsigned int cubeVao = 0;
//unsigned int cubeVbo = 0;
//static void drawCube() {
//  if (cubeVao == 0) {
//    float unitCubeVertices[] = {
//        // unit cube vertices
//        // TODO: read data from file
//        // Back face
//        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
//        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
//        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
//        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
//        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
//        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
//        // Front face
//        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
//        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
//        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
//        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
//        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
//        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
//        // Left face
//        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
//        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
//        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
//        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,// bottom-left
//        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
//        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// bottom-right
//        // Right face
//        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
//        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
//        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
//        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
//        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
//        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //  top-left
//        // Bottom face
//        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
//        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom-left
//        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
//        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom-left
//        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
//        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
//        // Top face
//        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
//        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
//        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
//        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
//        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
//        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f // top-left
//    };
//    glGenVertexArrays(1, &cubeVao);
//    glGenBuffers(1, &cubeVbo);
//    // fill buffer
//    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(unitCubeVertices), unitCubeVertices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
//
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glEnableVertexAttribArray(2);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//  }
//  glBindVertexArray(cubeVao);
//  glDrawArrays(GL_TRIANGLES, 0, 36);
//  glBindVertexArray(0);
//
//}

#pragma endregion // helper functions

#pragma region Window callback functions

static void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos) {
  if (firstMouse) {
    lastX = (float) xPos;
    lastY = (float) yPos;
    firstMouse = false;
  }

  float xOffset = (float) xPos - lastX;
  float yOffset = (float) yPos - lastY;

  lastX = (float) xPos;
  lastY = (float) yPos;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    camera->pan((float) xOffset, deltaTime);
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    camera->tilt((float) yOffset, deltaTime);
  }

}

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
  camera->processMouseScroll((float) yOffset);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  // g: toggle glow
  if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    if (isGlow == 1) {
      isGlow = 0;
    } else {
      isGlow = 1;
    }
  }

  // x: toggle texture
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    if (isTexture == 1) {
      isTexture = 0;
    } else {
      isTexture = 1;
    }
  }

  // b: toggle shadow
  if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    if (isShadow == 1) {
      isShadow = 0;
    } else {
      isShadow = 1;
    }
  }
}

#pragma endregion // Window callback functions