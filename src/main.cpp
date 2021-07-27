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

static void updateModelPosition();

static unsigned int loadTexture(const char *path);

static void processInput();

static void readFloatArrayFromFile(const std::string &path);

#pragma endregion // declare functions

#pragma region Declare global variables

Camera *camera;
Model *model1;
Model *model2;
Model *model3;
Model *model4;
Model *wall1;
Model *wall2;
Model *wall3;
Model *wall4;
Model *currentModel;
Model *currentWall;
std::vector<Model *> cornerObjects;

// window width & height
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
glm::vec3 *cornerPositions;

// true when mouse callback is called for the first time; false otherwise
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float deltaTime; // Time between current frame and last frame
float lastFrame; // Time of last frame

#pragma endregion // declare global variables

int main(int argc, char *argv[]) {

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
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH,
                                        SCR_HEIGHT,
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

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
  };

  float unitLineVertices[] = {
      -1.0f, 0.0f, 0.0f, 0.365f, 0.224f, 0.122f, // pos * 3, color * 3
      1.0f, 0.0f, 0.0f, 0.365f, 0.224f, 0.122f
  };

  float axisVertices[] = {
      0.0f, 0.0f, 0.0f, 0.20f, 0.20f, 0.20f, // pos * 3, color * 3
      5.0f, 0.0f, 0.0f, 1.00f, 0.20f, 0.20f,
      0.0f, 0.0f, 0.0f, 0.20f, 0.20f, 0.20f,
      0.0f, 5.0f, 0.0f, 0.20f, 1.00f, 0.20f,
      0.0f, 0.0f, 0.0f, 0.20f, 0.20f, 0.20f,
      0.0f, 0.0f, 5.0f, 0.20f, 0.20f, 1.00f
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

  cornerPositions = new glm::vec3[]{
      glm::vec3(-30.0, 3.0f, 30.0f),
      glm::vec3(30.0f, 3.0f, 30.0f),
      glm::vec3(30.0f, 3.0f, -30.0f),
  };

  glm::vec3 baseCubePosition(2.0f, 3.0f, 5.0f);
  glm::vec3 baseWallPosition(2.0f, 3.0f, 2.0f);

  // build models
  model1 = new Model(baseCubePosition, relativeCubePositions1);
  model2 = new Model(baseCubePosition, relativeCubePositions2);
  model3 = new Model(baseCubePosition, relativeCubePositions3);
  model4 = new Model(baseCubePosition, relativeCubePositions4);
  wall1 = new Model(baseWallPosition, relativeWallPositions1);
  wall2 = new Model(baseWallPosition, relativeWallPositions2);
  wall3 = new Model(baseWallPosition, relativeWallPositions3);
  wall4 = new Model(baseWallPosition, relativeWallPositions4);

  currentModel = model1;
  currentWall = wall1;
  cornerObjects = {model2, wall2, model3, wall3, model4, wall4};
  updateModelPosition();

  // light declarations
  PointLight pointLight = PointLight(
      glm::vec3(2.0f, 10.0f, 10.0f),
      glm::vec3(glm::radians(0.0f), 0, 0),
      glm::vec3(5.0f)
  );


  // build and compile shader
  Shader cubeShader
      ("res/shader/CubeVertex.shader", "res/shader/CubeFragment.shader");
  Shader lineShader
      ("res/shader/LineVertex.shader", "res/shader/LineFragment.shader");

  clearError();
  // Binding geometry data
  // ------------------------------------
  unsigned int cubeVao, gridVao, axisVao, cubeVbo, gridVbo, axisVbo;
  glGenVertexArrays(1, &cubeVao);
  glGenVertexArrays(1, &gridVao);
  glGenVertexArrays(1, &axisVao);
  glGenBuffers(1, &cubeVbo);
  glGenBuffers(1, &gridVbo);
  glGenBuffers(1, &axisVbo);

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

  // bind grid data
  glBindVertexArray(gridVao);
  glBindBuffer(GL_ARRAY_BUFFER, gridVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(unitLineVertices), unitLineVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // bind axis data
  glBindVertexArray(axisVao);
  glBindBuffer(GL_ARRAY_BUFFER, axisVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  auto *material = new Material(cubeShader,
                                loadTexture("res/texture/container2.png"),
                                loadTexture("res/texture/container2_specular.png"),
                                glm::vec3(0.5f, 0.5f, 0.5f),
                                32.0f);

  // load texture
//  stbi_set_flip_vertically_on_load(true);
//  unsigned int cubeTexBuffer;
//  cubeTexBuffer = loadTexture("res/texture/cube-texture.png");
//  unsigned int wallTexBuffer;
//  wallTexBuffer = loadTexture("res/texture/wall-texture.png");


  // Entering Main Loop
  while (!glfwWindowShouldClose(window)) {
    // update time
    auto currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // process input
    processInput(window);
    // Each frame, reset color of each pixel to glClearColor
    glClearColor(0.035f, 0.149f, 0.1098f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // get camera matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera->Fov),
                                            (float) SCR_WIDTH / SCR_HEIGHT,
                                            0.1f,
                                            100.f);
    glm::mat4 view = camera->getViewMatrix();
    // import cameraPos
    cubeShader.use();
    cubeShader.setVec3("cameraPos", camera->Position);
    // draw model
    glBindVertexArray(cubeVao);
    cubeShader.setMat4("view", view);
    cubeShader.setMat4("projection", projection);

    for (size_t i = 0; i < currentModel->size(); i++) {
      //load shader for light
      cubeShader.setVec3("ambientColor", glm::vec3(0.3f));
      cubeShader.setVec3("pointLight.pos", pointLight.position);
      cubeShader.setVec3("pointLight.lightDir", pointLight.direction);
      cubeShader.setVec3("pointLight.color", pointLight.color);
      cubeShader.setFloat("pointLight.constant", pointLight.constant);
      cubeShader.setFloat("pointLight.linear", pointLight.linear);
      cubeShader.setFloat("pointLight.quadratic", pointLight.quadratic);

      // assign cube texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, material->diffuse);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, material->specular);
      cubeShader.setInt("material.diffuse", 0);
      cubeShader.setInt("material.specular", 1);
      cubeShader.setFloat("material.shininess", material->shininess);

      // calculate cube model matrix
      glm::mat4 cubeModelMatrix = currentModel->getModelMatrix(i);
      cubeShader.setMat4("model", cubeModelMatrix);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
//    // draw wall
//    for (size_t i = 0; i < currentWall->size(); i++) {
//      // assign wall texture
//      glActiveTexture(GL_TEXTURE1);
//      glBindTexture(GL_TEXTURE_2D, wallTexBuffer);
//      glUniform1i(cubeTexLocation, 1);
//      // calculate the model matrix for wall
//      glm::mat4 wallModelMatrix = currentWall->getModelMatrix(i);
//      glUniformMatrix4fv(cubeModelLocation,
//                         1,
//                         GL_FALSE,
//                         glm::value_ptr(wallModelMatrix));
//      glDrawArrays(GL_TRIANGLES, 0, 36);
//    }
//
//    // draw corner models
//    for (int j = 0; j < 3; j++) {
//      // draw model (at cornerObjects[] even indices)
//      for (size_t i = 0; i < cornerObjects[2 * j]->size(); i++) {
//        // assign cube texture
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, cubeTexBuffer);
//        glUniform1i(cubeTexLocation, 0);
//        glm::mat4 cubeModelMatrix = cornerObjects[2 * j]->getModelMatrix(i);
//        glUniformMatrix4fv(cubeModelLocation,
//                           1,
//                           GL_FALSE,
//                           glm::value_ptr(cubeModelMatrix));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//      }
//      // draw wall (at cornerObjects[] odd indices)
//      for (size_t i = 0; i < cornerObjects[2 * j + 1]->size(); i++) {
//        // assign wall texture
//        glUniform1i(cubeTexLocation, 1);
//        glm::mat4 wallModelMatrix = cornerObjects[2 * j + 1]->getModelMatrix(i);
//        glUniformMatrix4fv(cubeModelLocation,
//                           1,
//                           GL_FALSE,
//                           glm::value_ptr(wallModelMatrix));
//        glDrawArrays(GL_TRIANGLES, 0, 36);
//      }
//    }

    // draw grid
    glBindVertexArray(gridVao);
    lineShader.use();
    lineShader.setMat4("projection", projection);
    lineShader.setMat4("view", view);

    int gridModelLocation = glGetUniformLocation(lineShader.id, "model");
    for (int i = 0; i < 100; i++) {
      // horizontal lines
      glm::mat4 gridModelMatrix = glm::translate(glm::mat4(1.0f),
                                                 glm::vec3(0.0f, 0.0f, -50.0f + (float) i));
      gridModelMatrix = glm::scale(gridModelMatrix, glm::vec3(50.0f, 1.0f, 1.0f));
      lineShader.setMat4("model", gridModelMatrix);
      glDrawArrays(GL_LINES, 0, 2);

      // vertical lines
      gridModelMatrix = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(-50.0f + (float) i, 0.0f, 0.0f));
      gridModelMatrix = glm::rotate(gridModelMatrix,
                                    glm::radians(90.0f),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
      gridModelMatrix = glm::scale(gridModelMatrix, glm::vec3(50.0f, 1.0f, 1.0f));
      lineShader.setMat4("model", gridModelMatrix);
      glDrawArrays(GL_LINES, 0, 2);
    }

    // draw axis
    glLineWidth(5.0f);
    glBindVertexArray(axisVao);
    lineShader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_LINES, 0, 2);
    glDrawArrays(GL_LINES, 2, 2);
    glDrawArrays(GL_LINES, 4, 2);
    glLineWidth(1.0f);

    // End frame
    glfwSwapBuffers(window);
    // Detect inputs
    glfwPollEvents();

    checkError();
  }

  // deallocate resources
  glDeleteVertexArrays(1, &cubeVao);
  glDeleteVertexArrays(1, &axisVao);
  glDeleteVertexArrays(1, &gridVao);
  glDeleteBuffers(1, &cubeVbo);
  glDeleteBuffers(1, &axisVbo);
  glDeleteBuffers(1, &gridVbo);

  // camera
  delete camera;
  // models
  delete model1;
  delete model2;
  delete model3;
  delete model4;
  // walls
  delete wall1;
  delete wall2;
  delete wall3;
  delete wall4;
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
    cornerObjects = {model2, wall2, model3, wall3, model4, wall4};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    currentModel = model2;
    currentWall = wall2;
    cornerObjects = {model1, wall1, model3, wall3, model4, wall4};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
    currentModel = model3;
    currentWall = wall3;
    cornerObjects = {model1, wall1, model2, wall2, model4, wall4};
    updateModelPosition();
  } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
    currentModel = model4;
    currentWall = wall4;
    cornerObjects = {model1, wall1, model2, wall2, model3, wall3};
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
  for (int i = 0; i < 6; i++) {
    cornerObjects[i]->setBasePosition(cornerPositions[i / 2]);
  }
}

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

#pragma endregion // Window callback functions