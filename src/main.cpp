#define GLEW_STATIC 1   //This allows linking with Static Library on Windows, without DLL

#include <GL/glew.h>   // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


static void clearError() {
  while (glGetError() != GL_NO_ERROR);
}

static void checkError() {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL Error] (0x" << std::hex << error << std::dec << ")" << std::endl;
  }
}


static void processInput(GLFWwindow *window);

static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera *camera;
Model *model;

// for mouse initialization
bool firstMouse = true; // true when mouse callback is called for the first time; false otherwise
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float deltaTime;    // Time between current frame and last frame
float lastFrame; // Time of last frame

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

  // Create Window and rendering context using GLFW, resolution is 800x600
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Comp371 - Assignment 01", nullptr, nullptr);
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

  // enable depth info
  glEnable(GL_DEPTH_TEST);

  // draw lines only
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  camera = new Camera(glm::vec3(0.0f, 0.0f, 10.0f));

  // Initialize geometry data
  // -----------------------------------
    float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

            -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
            0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  std::vector<glm::vec3> relativeCubePositions = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(1.0f, 2.0f, 0.0f),
      glm::vec3(2.0f, 2.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, -1.0f),
      glm::vec3(1.0f, 0.0f, -1.0f),
      glm::vec3(1.0f, 1.0f, -1.0f),
      glm::vec3(1.0f, 2.0f, -1.0f),
      glm::vec3(2.0f, 2.0f, -1.0f),
  };

  glm::vec3 baseCubePosition(0.0f, 0.0f, 0.0f);

  model = new Model(baseCubePosition, relativeCubePositions);

  // bind geometry data
  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // point attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
//  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),((void*)(3 * sizeof(float))));
  glEnableVertexAttribArray(2);


  // build and compile shader
  // ------------------------
  Shader shader("res/shader/Vertex.shader", "res/shader/Fragment.shader");
  shader.use();
  // import texture:
  unsigned int TexBufferA;

  glGenTextures(1, &TexBufferA);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, TexBufferA);

  int width, height, nrChannel;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load("res/texture/texture.png", &width, &height, &nrChannel, 0);
  if (data) {
      std::cout << "data reade" << std::endl;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
  } else {
      printf("load image failed.");
  }
  stbi_image_free(data);
  // Entering Main Loop
  while (!glfwWindowShouldClose(window)) {
    // update deltaTime
    auto currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // process input
    processInput(window);

    // Each frame, reset color of each pixel to glClearColor
    glClearColor(0.27f, 0.05f, 0.19f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    // GOING 3D
    glm::mat4 projection = glm::perspective(glm::radians(camera->Fov), (float) SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.f);
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view = camera->getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // active & bind texture
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TexBufferA);

    // render
    glBindVertexArray(vao);
    for (int i = 0; i < relativeCubePositions.size(); i++) {
      // calculate the model matrix for each object
      glUniform1i(glGetUniformLocation(shader.id, "cubeTexture"), 0);
      glm::mat4 modelMatrix = model->getModelMatrix(i);
      glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // End frame
    glfwSwapBuffers(window);

    // Detect inputs
    glfwPollEvents();

  }
  // deallocate resources
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  delete camera;
  delete model;

  // Shutdown GLFW
  glfwTerminate();

  return 0;
}

static void processInput(GLFWwindow *window) {
  // Escape
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  // u: scale up model
  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
    model->scaleUp(deltaTime);
  }

  // j: scale down model
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
    model->scaleDown(deltaTime);
  }

  // shift + w: move up model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
       || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    model->move(ModelMovement::UP, deltaTime);
  }

  // shift + s: move down model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
       || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    model->move(ModelMovement::DOWN, deltaTime);
  }

  // shift + a: move left model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
       || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    model->move(ModelMovement::LEFT, deltaTime);
  }

  // shift + d: move right model
  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
       || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
      && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    model->move(ModelMovement::RIGHT, deltaTime);
  }

  // a: rotate left about y-axis
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    model->rotate(glm::radians(-5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  }

  // d: rotate right about y-axis
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    model->rotate(glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  }

}

// callback function on window size changed
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

  camera->processMouseMovement(xOffset, yOffset);
}

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
  camera->processMouseScroll((float) yOffset);
}