#define GLEW_STATIC 1   //This allows linking with Static Library on Windows, without DLL
#define STB_IMAGE_IMPLEMENTATION // Enable stb library

#include <GL/glew.h>   // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "stb_image.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <irrKlang/irrKlang.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Material.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Cube.h"
#include "Puzzle.h"


#include "geometryData.h"

#include "OBJloader.h"  //For loading .obj files
#include "OBJloaderV2.h"  //For loading .obj files using a polygon list format

#pragma region Declare static functions

static void clearError();

static void checkError();

static void processInput(GLFWwindow *window);

static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

static unsigned int loadTexture(const char *path);

static void processInput();

static void renderScene(Shader &shader);

//Sets up a model using an Element Buffer Object to refer to vertex data
GLuint setupModelEBO(std::string path, int& vertexCount);

#pragma endregion // declare functions

#pragma region Declare global variables

Camera *camera;

Model *lightBoxModel;

bool isGlowingOn = false;
bool isTextureOn = true;
bool isShadowOn = true;

// window width & height
int scrWidth = 1024;
int scrHeight = 768;
//glm::vec3 *cornerPositions;

// true when mouse callback is called for the first time; false otherwise
bool firstMouse = true;
float lastX = (float) scrWidth / 2.0f;
float lastY = (float) scrHeight / 2.0f;

float deltaTime; // Time between current frame and last frame
float lastFrame; // Time of last frame

#pragma endregion // declare global variables

int main(int argc, char *argv[]) {

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return -1;
  }

  irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

  if (!engine)
    return 0; // error starting up the engine

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

  std::vector<glm::vec3> relativeLightBoxPosition{
      glm::vec3(0, 0, 0)
  };
  glm::vec3 pointLightPosition(0.0f, 3.0f, -45.0f);
  glm::vec3 spotLightPosition(0.0f, 49.0f, 1.0f);
  glm::vec3 directionalLightPosition(-49.0f, 49.0f, 49.0f);

  // build models
  // --------------------------------------------------------
  lightBoxModel = new Model(pointLightPosition, relativeLightBoxPosition);

  // set model position

  // configure light
  // ---------------------------------------
  PointLight pointLight = PointLight(pointLightPosition,
                                     glm::vec3(glm::radians(0.0f), 0, 0),
                                     glm::vec3(1.0f, 1.0f, 0.88f) * 50.0f
  );

  DirectionalLight directionalLight = DirectionalLight(
      directionalLightPosition,
      glm::vec3(glm::radians(45.0f),glm::radians(145.0f), 0),
      glm::vec3(0.55f, 0.45f, 0.53f)*0.8f
  );

  SpotLight spotLight = SpotLight(
      spotLightPosition,
      //angle: Forward(0,0,0), Down(-90,0,0)
      glm::vec3(glm::radians(-90.0f), 0, 0),
      glm::vec3(0.6f, 0.55f, 0.40f)*180.0f
  );


  // import 3d model
  int objVertices;
  GLuint objVAO = setupModelEBO("res/object/heracles.obj", objVertices);


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
  unsigned int axisVao;
  unsigned int axisVbo;
  glGenVertexArrays(1, &axisVao);
  glGenBuffers(1, &axisVbo);

  // bind axis data
  glBindVertexArray(axisVao);
  glBindBuffer(GL_ARRAY_BUFFER, axisVbo);
  glBufferData(GL_ARRAY_BUFFER, 12 * 6 * sizeof(float), axisVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // configure materials
  Material metal(loadTexture("res/texture/metal.png"),
                 loadTexture("res/texture/metal_specular.png"),
                 glm::vec3(0.2f, 0.2f, 0.2f),
                 8.0f);

  Material brick(loadTexture("res/texture/brick.png"),
                 loadTexture("res/texture/brick_specular.png"),
                 glm::vec3(0.3f, 0.3f, 0.3f),
                 64.0f);

  Material lightBoxMaterial(loadTexture("res/texture/sea_lantern.png"),
                            loadTexture("res/texture/sea_lantern.png"),
                            glm::vec3(1.0f, 1.0f, 1.0f),
                            1.0f);


  Material tile(loadTexture("res/texture/tile.png"),
                loadTexture("res/texture/tile_specular.png"),
                glm::vec3(0.45f, 0.45f, 0.45f),
                128.0f);

  std::vector<Material *> numberMaterials;
  numberMaterials.reserve(8);
  for (int i = 0; i < 8; i++) {
    std::string address1 = "res/texture/numbers/number" + std::to_string((int) i) + ".png";
    std::string address2 = "res/texture/numbers/number" + std::to_string((int) i) + "_specular.png";
    numberMaterials.push_back(new Material(loadTexture(address1.c_str()),
                                           loadTexture(address2.c_str()),
                                           glm::vec3(1.0f, 1.0f, 1.0f),
                                           64.0f));
  }

  std::vector<Cube *> bricks;
  bricks.reserve(8);
  for (int i = 0; i < 8; i++) {
    bricks.push_back(new Cube(*numberMaterials[i]));
  }

  Puzzle puzzle(bricks);

  Cube ground(tile, unitWorldVertices);
  ground.setScale(100.0f);
  ground.setPosition(glm::vec3(0,50.0f,0));

  Cube pointLightCube(lightBoxMaterial);
  pointLightCube.setPosition(pointLight.position);
  Cube  directionalLightCube(lightBoxMaterial);
  directionalLightCube.setPosition(directionalLight.position);
  Cube spotLightCube(lightBoxMaterial);
  spotLightCube.setPosition(spotLight.position);


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
    float farPlane = 200.0f;
//    glm::mat4 lightProjection = glm::perspective(glm::radians(85.0f),
//                                                 (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT,
//                                                 nearPlane, farPlane);
//    glm::mat4 lightView = glm::lookAt(pointLight.position, glm::vec3(0.0f),
//                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::perspective(glm::radians(39.31f),
                                       (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT,
                                       nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(spotLight.position, glm::vec3(0.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    depthMappingShader.use();
//    depthMappingShader.setFloat("nearPlane", nearPlane);
//    depthMappingShader.setFloat("farPlane", farPlane);
    depthMappingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
//    glCullFace(GL_FRONT);

    // draw objects
    puzzle.draw(depthMappingShader, false);
    ground.draw(depthMappingShader, false);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
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
                                            150.f);
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
    //pointLight
    cubeShader.setVec3("pointLight.pos", pointLight.position);
    cubeShader.setVec3("pointLight.lightDir", pointLight.direction);
    cubeShader.setVec3("pointLight.color", pointLight.color);
    cubeShader.setFloat("pointLight.constant", pointLight.constant);
    cubeShader.setFloat("pointLight.linear", pointLight.linear);
    cubeShader.setFloat("pointLight.quadratic", pointLight.quadratic);
    //directionalLight
    cubeShader.setVec3( "directionalLight.pos", directionalLight.position);
    cubeShader.setVec3("directionalLight.lightDir", directionalLight.direction);
    cubeShader.setVec3("directionalLight.color", directionalLight.color);
    //spotLight
    cubeShader.setVec3("spotLight.pos", spotLight.position);
    cubeShader.setVec3("spotLight.lightDir", spotLight.direction);
    cubeShader.setVec3("spotLight.color", spotLight.color);
    cubeShader.setFloat("spotLight.constant", spotLight.constant);
    cubeShader.setFloat("spotLight.linear", spotLight.linear);
    cubeShader.setFloat("spotLight.quadratic", spotLight.quadratic);
    cubeShader.setFloat("spotLight.innerCosPhy", spotLight.innerCosPhy);
    cubeShader.setFloat("spotLight.outerCosPhy", spotLight.outerCosPhy);

    cubeShader.setInt("emissionMap", 2);
    cubeShader.setInt("shadowMap", 3);
    cubeShader.setFloat("timeValue", timeValueForColor);
    cubeShader.setBool("isShadowOn", isShadowOn);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    puzzle.draw(cubeShader, true);
    glFrontFace(GL_CW);
    ground.draw(cubeShader, true);
    glFrontFace(GL_CCW);

    // draw a light box to indicate light position
    cubeShader.setBool("isLightBox", true);
    pointLightCube.draw(cubeShader, true);
    directionalLightCube.draw(cubeShader, true);
    spotLightCube.draw(cubeShader, true);
    cubeShader.setBool("isLightBox", false);
    // end of light box


    // draw obj model
    glBindVertexArray(objVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brick.diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, brick.specular);
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setFloat("material.shininess", brick.shininess);
    cubeShader.setVec3("material.ambient", brick.ambient);
    // set model matrix for obj
    glm::mat4 objmodel(1.0f);
    float objangle = -90.0f;
    objmodel = glm::translate(objmodel, glm::vec3(-6.0f, 2.0f, -8.0f));
    objmodel = glm::scale(objmodel, (glm::vec3(0.15), glm::vec3(0.15), glm::vec3(0.15)));
    objmodel = glm::rotate(objmodel, glm::radians(objangle), glm::vec3(1.0f, 0.0f, 0.0f));
    objmodel = glm::rotate(objmodel, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    cubeShader.setMat4("model", objmodel);
    glDrawElements(GL_TRIANGLES, objVertices, GL_UNSIGNED_INT, 0);




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

    checkError();
    // End frame
    glfwSwapBuffers(window);
    // Detect inputs
    glfwPollEvents();

  }

  // deallocate resources
  glDeleteVertexArrays(1, &axisVao);
  glDeleteBuffers(1, &axisVbo);

  // camera
  delete camera;
  // models
  delete lightBoxModel;
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
//  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
//    currentModel = model1;
//    currentWall = wall1;
//    cornerObjects = {model2, wall2, model3, wall3, model4, wall4, model5, wall5};
//    updateModelPosition();
//  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
//    currentModel = model2;
//    currentWall = wall2;
//    cornerObjects = {model1, wall1, model3, wall3, model4, wall4, model5, wall5};
//    updateModelPosition();
//  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
//    currentModel = model3;
//    currentWall = wall3;
//    cornerObjects = {model1, wall1, model2, wall2, model4, wall4, model5, wall5};
//    updateModelPosition();
//  } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
//    currentModel = model4;
//    currentWall = wall4;
//    cornerObjects = {model1, wall1, model2, wall2, model3, wall3, model5, wall5};
//    updateModelPosition();
//  } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
//    currentModel = model5;
//    currentWall = wall5;
//    cornerObjects = {model1, wall1, model2, wall2, model3, wall3, model4, wall4};
//    updateModelPosition();
//  }

//  // u: scale up model
//  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
//    currentModel->scaleUp(deltaTime);
//  }
//
//  // j: scale down model
//  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
//    currentModel->scaleDown(deltaTime);
//  }
//
//  // =: scale up model and wall
//  if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
//    currentWall->scaleUp(deltaTime);
//    currentModel->scaleUp(deltaTime);
//  }
//
//  // -: scale down model and wall
//  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
//    currentWall->scaleDown(deltaTime);
//    currentModel->scaleDown(deltaTime);
//  }
//
//  // shift + w: move up model
//  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
//      || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
//      && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//    currentModel->move(ModelMovement::UP, deltaTime);
//  }
//
//  // shift + s: move down model
//  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
//      || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
//      && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//    currentModel->move(ModelMovement::DOWN, deltaTime);
//  }

//  // shift + a: move left model
//  if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
//      || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
//      && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//  }
//
//
//  // a
//  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
//        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//      // shift + a: move left model
//      currentModel->move(ModelMovement::LEFT, deltaTime);
//    } else {
//      // a: rotate left about y-axis
//      currentModel->rotate(glm::radians(-90.0f),
//                           glm::vec3(0.0f, 1.0f, 0.0f),
//                           deltaTime);
//    }
//  }
//
//  // d
//  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
//        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//      // shift + d: move right model
//      currentModel->move(ModelMovement::RIGHT, deltaTime);
//    } else {
//      // d: rotate right about y-axis
//      currentModel->rotate(glm::radians(90.0f),
//                           glm::vec3(0.0f, 1.0f, 0.0f),
//                           deltaTime);
//    }
//  }
//
//  // w
//  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
//        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//      // shift + w: move left model
//      currentModel->move(ModelMovement::UP, deltaTime);
//    } else {
//      // w: rotate left about x-axis
//      currentModel->rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f), deltaTime);
//    }
//  }
//
//  // s
//  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
//        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//      // shift + s: move left model
//      currentModel->move(ModelMovement::DOWN, deltaTime);
//    } else {
//      // s: rotate left about x-axis
//      currentModel->rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f), deltaTime);
//    }
//  }
//
//  // q
//  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
//        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//      // shift + q: move forward model
//      currentModel->move(ModelMovement::FORWARD, deltaTime);
//    } else {
//      // q: rotate left about z-axis
//      currentModel->rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f), deltaTime);
//    }
//  }
//
//  // e
//  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
//        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//      // shift + e: move left model
//      currentModel->move(ModelMovement::BACKWARD, deltaTime);
//    } else {
//      // e: rotate left about z-axis
//      currentModel->rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f), deltaTime);
//    }
//  }

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

}

//static void updateModelPosition() {
//  currentModel->resetPosition();
//  currentWall->resetPosition();
//  currentModel->resetOrientation();
//  currentWall->resetOrientation();
//  for (int i = 0; i < 8; i++) {
////    cornerObjects[i]->setBasePosition(cornerPositions[i / 2]);
//  }
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
    isGlowingOn = !isGlowingOn;
  }

  // x: toggle texture
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    isTextureOn = !isTextureOn;
  }

  // b: toggle shadow
  if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    isShadowOn = !isShadowOn;
  }
}

GLuint setupModelEBO(std::string path, int& vertexCount)
{
    std::vector<int> vertexIndices;
    //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

    //read the vertices from the cube.obj file
    //We won't be needing the normals or UVs for this program
    loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    //EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}

#pragma endregion // Window callback functions