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
#include "Board.h"

#include <irrklang/irrKlang.h> // for audio
#if __APPLE__
#include <unistd.h>
#define Sleep(X) usleep(X * 1000)
#else
#include <Windows.h>
#endif
using namespace irrklang;

ISoundEngine *SoundEngineBackground = createIrrKlangDevice();
ISoundEngine *SoundEngineKey = createIrrKlangDevice();
ISoundEngine *SoundEngineFace = createIrrKlangDevice();

#pragma region Declare static functions

static void clearError();

static void checkError();

static void processInput(GLFWwindow *window);

static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

static unsigned int loadTexture(const char *path);

static unsigned int loadCubeMap(std::vector<std::string> faces);

static void processInput();

static void renderScene(Shader &shader);

std::vector<glm::vec3> bitToLetter(std::vector<int> &letter);

static void setIncrementZ(int i, float startPositionZ);

//Sets up a model using an Element Buffer Object to refer to vertex data
GLuint setupModelEBO(std::string path, int &vertexCount);

#pragma endregion // declare functions

#pragma region Declare global variables

Camera *camera;

Model *lightBoxModel;

Puzzle *currentPuzzle;

Board *pBoard;

bool isGlowingOn = false;
bool isTextureOn = true;
bool isShadowOn = true;
float zIncrement[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

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

bool winThisFrame = false;
glm::vec3 textColor(1.0f);
std::string timer;
std::vector<Puzzle*> puzzles;
#pragma endregion // declare global variables

// added for text rendering
#include <map>
#include <string>
#include <glm/gtc/type_ptr.hpp>
unsigned int VAO, VBO;
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;      // Size of glyph
  glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

int main(int argc, char *argv[]) {

  irrklang::ISoundEngine *engine = irrklang::createIrrKlangDevice();

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

  SoundEngineBackground->play2D("res/audio/Minecraft_bgm.mp3", true);
  //SoundEngineBackground->setSoundVolume(1);

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
  glm::vec3 pointLightPosition(0.0f, 15.0f, -150.0f);
  glm::vec3 spotLightPosition(0.0f, 30.0f, 40.0f);
  glm::vec3 directionalLightPosition(0.0f, 49.0f, 0.0f);

  // build models
  // --------------------------------------------------------
  lightBoxModel = new Model(pointLightPosition, relativeLightBoxPosition);

  // set model position

  // configure light
  // ---------------------------------------
  PointLight pointLight = PointLight(pointLightPosition,
                                     glm::vec3(glm::radians(0.0f), 0, 0),
                                     glm::vec3(1.0f, 1.0f, 0.88f) * 45.0f
  );

  DirectionalLight directionalLight = DirectionalLight(
      directionalLightPosition,
      glm::vec3(glm::radians(90.0f), 0, 0),
      glm::vec3(0.55f, 0.45f, 0.53f) * 0.8f
  );

  SpotLight spotLight = SpotLight(
      spotLightPosition,
      //angle: Forward(0,0,0), Down(-90,0,0)
      glm::vec3(glm::radians(-40.0f), glm::radians(0.0f), 0),
      glm::vec3(0.6f, 0.55f, 0.40f) * 50.0f
  );

  std::vector<glm::vec3> relativeCharPosition = {
      //Head
      glm::vec3(-35.0f, 10.0f, -120.0f),
      glm::vec3(-25.0f, 22.0f, -80.0f),
      glm::vec3(-10.0f, 23.0f, -60.0f),
      glm::vec3(10.0f, 25.0f, -70.0f),
      glm::vec3(25.0f, 15.0f, -110.0f),
      glm::vec3(35.0f, 5.0f, -130.0f),
      glm::vec3(-22.0f, 18.0f, -90.0f),
      glm::vec3(15.0f, 20.0f, -120.0f),
      glm::vec3(0.0f, 30.0f, -50.0f),
  };

  glm::vec3 baseCubePosition(0.0f, 0.0f, 0.0f);
  std::vector<glm::vec3> charT = bitToLetter(gauT);
  std::vector<glm::vec3> charE = bitToLetter(gauE);
  std::vector<glm::vec3> charA = bitToLetter(gauA);
  std::vector<glm::vec3> charM = bitToLetter(gauM);
  std::vector<glm::vec3> char1 = bitToLetter(gau1);
  std::vector<glm::vec3> char2 = bitToLetter(gau2);
  std::vector<Model *> flyingModels;
  flyingModels.reserve(6);
  flyingModels.push_back(new Model(relativeCharPosition[0], charT));
  flyingModels.push_back(new Model(relativeCharPosition[1], charE));
  flyingModels.push_back(new Model(relativeCharPosition[2], charA));
  flyingModels.push_back(new Model(relativeCharPosition[3], charM));
  flyingModels.push_back(new Model(relativeCharPosition[4], char1));
  flyingModels.push_back(new Model(relativeCharPosition[5], char2));

  // build and compile shader
  // -----------------------------------------------------
  Shader cubeShader("res/shader/CubeVertex.shader",
                    "res/shader/CubeFragment.shader");
  Shader lineShader("res/shader/LineVertex.shader",
                    "res/shader/LineFragment.shader");
  Shader depthMappingShader("res/shader/DepthMappingVertex.shader",
                            "res/shader/DepthMappingFragment.shader");
  Shader skyBoxShader("res/shader/SkyBoxVertex.shader",
                      "res/shader/SkyBoxFragment.shader");


  // text rendering shader
  Shader textShader("res/shader/textVertex.shader", "res/shader/textFragment.shader");
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(scrWidth), 0.0f, static_cast<float>(scrHeight));
  textShader.use();
  glUniformMatrix4fv(glGetUniformLocation(textShader.id, "projection"),
                     1,
                     GL_FALSE,
                     glm::value_ptr(projection));

  // find path to font
  // website for free fonts:  https://www.websiteplanet.com/blog/best-free-fonts/
  std::string font_name = "res/font/BitterProSemiBoldItalic.ttf";
  if (font_name.empty()) {
    std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
    return -1;
  }

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    return -1;
  }

  // load font as face
  FT_Face face;
  if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    return -1;
  } else {
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++) {
      // Load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        continue;
      }
      // generate texture
      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RED,
          face->glyph->bitmap.width,
          face->glyph->bitmap.rows,
          0,
          GL_RED,
          GL_UNSIGNED_BYTE,
          face->glyph->bitmap.buffer
      );
      // set texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // now store character for later use
      Character character = {
          texture,
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          static_cast<unsigned int>(face->glyph->advance.x)
      };
      Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);


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
  std::vector<std::string> outerUniverse
      {
          "res/texture/OuterUniverseCubeMap/right.png",
          "res/texture/OuterUniverseCubeMap/left.png",
          "res/texture/OuterUniverseCubeMap/top.png",
          "res/texture/OuterUniverseCubeMap/bottom.png",
          "res/texture/OuterUniverseCubeMap/front.png",
          "res/texture/OuterUniverseCubeMap/back.png"
      };

  unsigned int OuterUniverseCubeMap = loadCubeMap(outerUniverse);

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

  Material white(loadTexture("res/texture/white.png"),
                 loadTexture("res/texture/white.png"),
                 glm::vec3(1.0f) * 2.0f,
                 128.0f);

  Material Universe(loadTexture("res/texture/Universe.jpg"),
                    loadTexture("res/texture/Universe.jpg"),
                    glm::vec3(1.0f, 1.0f, 1.0f),
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


  // import 3d model
  int monkeyVertexCount;
  GLuint monkeyVAO = setupModelEBO("res/object/monkey.obj", monkeyVertexCount);
  int tCatVertexCount;
  GLuint tCatVAO = setupModelEBO("res/object/tCat.obj", tCatVertexCount);
  int tCowVertexCount;
  GLuint tCowVAO = setupModelEBO("res/object/tCow.obj", tCowVertexCount);

  std::vector<unsigned int> importedModel;
  importedModel.reserve(3);
  importedModel.push_back(monkeyVAO);
  importedModel.push_back(tCatVAO);
  importedModel.push_back(tCowVAO);
  std::vector<int> importedVertexCount;
  importedVertexCount.reserve(3);
  importedVertexCount.push_back(monkeyVertexCount);
  importedVertexCount.push_back(tCatVertexCount);
  importedVertexCount.push_back(tCowVertexCount);

  // create puzzles

  puzzles.reserve(6);
  puzzles.push_back(new Puzzle(numberMaterials,0));
  puzzles.push_back(new Puzzle(numberMaterials,1));
  puzzles.push_back(new Puzzle(numberMaterials,2));
  puzzles.push_back(new Puzzle(numberMaterials,3));
  puzzles.push_back(new Puzzle(numberMaterials,4));
  puzzles.push_back(new Puzzle(numberMaterials,5));
  Board board(puzzles);
  currentPuzzle = board.getPuzzles()[0];
  pBoard = &board;
  board.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
  Cube boardCore(Universe);
  boardCore.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
  boardCore.setScale(glm::vec3(2.86f));
  boardCore.setQuaternion(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

  Cube worldBox(tile,unitWorldVertices);
  worldBox.setScale(glm::vec3(100.0f,160.0f,50.0f));
  worldBox.setPosition(glm::vec3(0.0f, 17.0f, -82.0f));
  worldBox.setQuaternion(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));
  Cube worldBoxBack(white, unitGroundVertices);
  worldBoxBack.setPosition(glm::vec3(0, 17.0f, -155.0f));
  worldBoxBack.setQuaternion(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f));
  worldBoxBack.setScale(glm::vec3(100.0f,0.0f,50.0f));

  Cube UniverseBox(white, unitCubeVertices);
  UniverseBox.setScale(glm::vec3(320.0f));

  Cube pointLightCube(lightBoxMaterial);
  pointLightCube.setPosition(pointLight.position);
  Cube directionalLightCube(lightBoxMaterial);
  directionalLightCube.setPosition(directionalLight.position);
  Cube spotLightCube(lightBoxMaterial);
  spotLightCube.setPosition(spotLight.position);

  unsigned int emissionMap;
  emissionMap = loadTexture("res/texture/Emission.png");

  //Cull_Face
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // added for text rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // configure VAO/VBO for texture quads
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


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
    float farPlane = 500.0f;
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
    depthMappingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    // draw objects
    board.draw(depthMappingShader, false);
    worldBox.draw(cubeShader, true);
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
                                            500.f);
    glm::mat4 view = camera->getViewMatrix();

    // draw model
    // ---------------------------------
    // load shader uniforms
    cubeShader.use();
    cubeShader.setVec3("cameraPos", camera->Position);
    cubeShader.setMat4("view", view);
    cubeShader.setMat4("projection", projection);
    cubeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    cubeShader.setVec3("ambientColor", glm::vec3(0.3f + 0.3f * timeValueForColor));
    //pointLight
    cubeShader.setVec3("pointLight.pos", pointLight.position);
    cubeShader.setVec3("pointLight.lightDir", pointLight.direction);
    cubeShader.setVec3("pointLight.color", pointLight.color);
    cubeShader.setFloat("pointLight.constant", pointLight.constant);
    cubeShader.setFloat("pointLight.linear", pointLight.linear);
    cubeShader.setFloat("pointLight.quadratic", pointLight.quadratic);
    //directionalLight
    cubeShader.setVec3("directionalLight.pos", directionalLight.position);
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
    cubeShader.setFloat("timeValuePeriod", timeValueForColor);
    cubeShader.setFloat("timeValueIncrement", (float) glfwGetTime());
    cubeShader.setBool("isShadowOn", isShadowOn);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //draw board
    board.draw(cubeShader, true);

    //draw board core
    glCullFace(GL_FRONT);
    cubeShader.setBool("isFlowing", true);
    cubeShader.setBool("isLightBox", true);
    boardCore.draw(cubeShader, true);
    cubeShader.setBool("isLightBox", false);
    cubeShader.setBool("isFlowing", false);
    glCullFace(GL_BACK);

    //draw ground
    cubeShader.setBool("isFlowing", true);
    glCullFace(GL_FRONT);
    worldBox.draw(cubeShader, true);
    glCullFace(GL_BACK);
    cubeShader.setBool("isLightBox", true);
    worldBoxBack.draw(cubeShader, true);
    cubeShader.setBool("isLightBox", false);
    cubeShader.setBool("isFlowing", false);

    // draw a light box to indicate light position
    cubeShader.setBool("isLightBox", true);
    //point light box
    spotLightCube.draw(cubeShader, true);
    //directional light box
    directionalLightCube.draw(cubeShader, true);
    //spotLightCube light box
    spotLightCube.draw(cubeShader, true);
    cubeShader.setBool("isLightBox", false);
    // end of light box

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metal.diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, metal.specular);
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setFloat("material.shininess", metal.shininess);
    cubeShader.setVec3("material.ambient", metal.ambient);
//
//
    for (size_t i = 0; i < flyingModels.size(); i++) {
      setIncrementZ((int) i, relativeCharPosition[i].z);
      glm::vec3 nextPosition = glm::vec3(relativeCharPosition[i].x,
                                         relativeCharPosition[i].y + 2 * cos(4 * currentFrame),
                                         relativeCharPosition[i].z + zIncrement[i]);
      flyingModels[i]->setBasePosition(nextPosition);
      flyingModels[i]->rotate(3.0f,
                              glm::vec3(0.0f, 0.0f, 1.0f),
                              (float) glm::pow(-1, i) * deltaTime);
      flyingModels[i]->rotate(1.5f,
                              glm::vec3(1.0f, 0.0f, 0.0f),
                              (float) glm::pow(-1, i) * deltaTime);
      for (size_t j = 0; j < flyingModels[i]->size(); j++) {
        glm::mat4 cubeModelMatrix = flyingModels[i]->getModelMatrix(j);
        cubeShader.setMat4("model", cubeModelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }


//     draw obj model
    clearError();
    for (size_t i= 0; i < importedModel.size(); i++) {
      glBindVertexArray(importedModel[i]);
      // set model matrix for obj
      glm::mat4 objModelMatrix(1.0f);
      setIncrementZ((int)i+6, relativeCharPosition[i+6].z);
      objModelMatrix = glm::translate(objModelMatrix,glm::vec3(relativeCharPosition[i+6].x,
                                                               relativeCharPosition[i+6].y + 2 * cos(4 * currentFrame),
                                                               relativeCharPosition[i+6].z + zIncrement[6]));
      if(i==0){
        objModelMatrix = glm::scale(objModelMatrix, (glm::vec3(2.0), glm::vec3(2.0), glm::vec3(2.0)));
      }else{
        objModelMatrix = glm::scale(objModelMatrix, (glm::vec3(0.008), glm::vec3(0.008), glm::vec3(0.008)));
      }
      objModelMatrix =
          glm::rotate(objModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
      objModelMatrix =
          glm::rotate(objModelMatrix, 3 * currentFrame, glm::vec3(0.0f, 0.0f, 1.0f));
      objModelMatrix =
          glm::rotate(objModelMatrix, 1.5f * currentFrame, glm::vec3(1.0f, 0.0f, 0.0f));

      cubeShader.setMat4("model", objModelMatrix);
      glDrawElements(GL_TRIANGLES, importedVertexCount[i], GL_UNSIGNED_INT, nullptr);
    }

    //Universe box
    skyBoxShader.use();
    // if incoming pixel depth is less or equals to value in depth buffer; pass the test;
    // note, the vertex shader of skyBoxShader has already passed w=1, therefore, it will always pass;
    glDepthFunc(GL_LEQUAL);
    // make depth value cannot be written to depth buffer;
    glDepthMask(GL_FALSE);
    skyBoxShader.setMat4("view", view);
    skyBoxShader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, OuterUniverseCubeMap);
    skyBoxShader.setInt("skybox", 0);
    glCullFace(GL_FRONT);
    UniverseBox.draw(skyBoxShader, true, false, true);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);


    currentPuzzle->puzzleSec += deltaTime;
    if (currentPuzzle->puzzleSec > 60) {
        currentPuzzle->puzzleSec -= 60;
        currentPuzzle->puzzleMin++;
    }
        
    if (currentPuzzle->puzzleSec>=10) {
        if (currentPuzzle->puzzleMin>=10) {
            timer = "Time used:" + std::to_string(currentPuzzle->puzzleMin) + ":" + std::to_string((int)currentPuzzle->puzzleSec);
        }
        else {
            timer = "Time used:0" + std::to_string(currentPuzzle->puzzleMin) + ":" + std::to_string((int)currentPuzzle->puzzleSec);
        }
        
    }
    else {
        if (currentPuzzle->puzzleMin >= 10) {
            timer = "Time used:" + std::to_string(currentPuzzle->puzzleMin) + ":0" + std::to_string((int)currentPuzzle->puzzleSec);
        }
        else {
            timer = "Time used:0" + std::to_string(currentPuzzle->puzzleMin) + ":0" + std::to_string((int)currentPuzzle->puzzleSec);
        }
    }
    
    
    
    //ggez
    // render text

    RenderText(textShader, "Presented by Comp 371 Team12 fORdAwIN @2021 SUMMER", 630.0f, 10.0f, 0.3f, textColor);
    RenderText(textShader, timer, 30.0f, 730.0f, 0.5f, textColor);
    RenderText(textShader, "Step Used: "+std::to_string(currentPuzzle->puzzleStep), 850.0f, 730.0f, 0.5f, textColor);
    if (winThisFrame) {
        winThisFrame = false;
        Sleep(5000);
        currentPuzzle->puzzleSec = -5.f;
        currentPuzzle->puzzleMin = 0;
        currentPuzzle->setWinBool();
        textColor = glm::vec3(1.0f);
        currentPuzzle->puzzleStep = 0;
    }
    if (currentPuzzle->getWinBool()) {
        winThisFrame = true;
        RenderText(textShader, "VICTORY ACHIEVED", 290.0f, 375.0f, 1.0f, glm::vec3(0.99f, 0.99f, 0.435f));
    }
    
//    checkError();
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

static unsigned int loadCubeMap(std::vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      GLenum format = GL_RGB;
      if (nrChannels == 1)
        format = GL_RED;
      else if (nrChannels == 3)
        format = GL_RGB;
      else if (nrChannels == 4)
        format = GL_RGBA;
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                   0, (int) format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
      stbi_image_free(data);
    }
  }
  //Anisotropic texture filtering
  //get the maximum Anisotropic filtering level your PC supports.
  GLfloat fLargest;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
  // turn on "AF"
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
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

    //Anisotropic texture filtering
    //get the maximum Anisotropic filtering level your PC supports.
//    GLfloat fLargest;
//    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    // turn on "AF"
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

// designed for word modeling
std::vector<glm::vec3> bitToLetter(std::vector<int> &letter) {
  std::vector<glm::vec3> ans;
  int width = 5, height = 7;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if (letter[i] & (1 << (width - j - 1))) {
        ans.emplace_back(j, height - i - 1, 0);
      }
    }
  }
  return ans;
}

//while zIncrement of an object is large enough to offset it's z position, reset to 0;
static void setIncrementZ(int i, float startPositionZ) {
  zIncrement[i] += 10.0f * deltaTime;
  if (zIncrement[i] > -startPositionZ+30) {
    zIncrement[i] = 0;
  }
}

static void processInput(GLFWwindow *window) {
  // Escape
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

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

//  // a
//  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
//    currentPuzzle->move(Movement::LEFT);
//  }
//  // d
//  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
//    currentPuzzle->move(Movement::RIGHT);
//  }
//
//  // w
//  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
//    currentPuzzle->move(Movement::UP);
//  }
//
//  // s
//  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
//    currentPuzzle->move(Movement::DOWN);
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

  // wasd: puzzle movement
  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
      if (currentPuzzle->move(Movement::UP)) {
          SoundEngineKey->play2D("res/audio/solid.wav", true);
          Sleep(20);
          SoundEngineKey->stopAllSounds();
          currentPuzzle->puzzleStep++;
    }
    
  }
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      if (currentPuzzle->move(Movement::LEFT)){
          SoundEngineKey->play2D("res/audio/solid.wav", true);
      Sleep(20);
      SoundEngineKey->stopAllSounds();
      currentPuzzle->puzzleStep++;     
    }
      
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      if (currentPuzzle->move(Movement::DOWN)) {
      SoundEngineKey->play2D("res/audio/solid.wav", true);
      Sleep(20);
      SoundEngineKey->stopAllSounds();
      currentPuzzle->puzzleStep++;
  }
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS) {
      if (currentPuzzle->move(Movement::RIGHT)) {
          SoundEngineKey->play2D("res/audio/solid.wav", true);
          Sleep(20);
          SoundEngineKey->stopAllSounds();
          currentPuzzle->puzzleStep++;
          
      }
  }

  // 1-6: switch puzzles
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    currentPuzzle = pBoard->getPuzzles()[0];
    pBoard->setQuaternion(glm::quat(glm::vec3(0.0f)));
    SoundEngineKey->play2D("res/audio/powerup.wav", true);
    Sleep(30);
    SoundEngineKey->stopAllSounds();

  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    currentPuzzle = pBoard->getPuzzles()[1];
    pBoard->setQuaternion(glm::quat(glm::vec3(0.0f, glm::radians(-90.0f), 0.0f)));
    SoundEngineKey->play2D("res/audio/powerup.wav", true);
    Sleep(30);
    SoundEngineKey->stopAllSounds();

  }
  if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
    currentPuzzle = pBoard->getPuzzles()[2];
    pBoard->setQuaternion(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, 0.0f)));
    SoundEngineKey->play2D("res/audio/powerup.wav", true);
    Sleep(30);
    SoundEngineKey->stopAllSounds();

  }
  if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
    currentPuzzle = pBoard->getPuzzles()[3];
    pBoard->setQuaternion(glm::quat(glm::vec3(0.0f, glm::radians(180.0f), 0.0f)));
    SoundEngineKey->play2D("res/audio/powerup.wav", true);
    Sleep(30);
    SoundEngineKey->stopAllSounds();

  }
  if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
    currentPuzzle = pBoard->getPuzzles()[4];
    pBoard->setQuaternion(glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f)));
    SoundEngineKey->play2D("res/audio/powerup.wav", true);
    Sleep(30);
    SoundEngineKey->stopAllSounds();

  }
  if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
    currentPuzzle = pBoard->getPuzzles()[5];
    pBoard->setQuaternion(glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f)));
    SoundEngineKey->play2D("res/audio/powerup.wav", true);
    Sleep(30);
    SoundEngineKey->stopAllSounds();

  }
  if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
      currentPuzzle->resetAll();
  }
  if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
      for each (Puzzle* x in puzzles)
      {
          x->resetAll();
      }
  }
}

GLuint setupModelEBO(std::string path, int &vertexCount) {
  std::vector<unsigned int> vertexIndices;
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
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(glm::vec3),
               &vertices.front(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
  glEnableVertexAttribArray(0);

  //Normals VBO setup
  GLuint normals_VBO;
  glGenBuffers(1, &normals_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               normals.size() * sizeof(glm::vec3),
               &normals.front(),
               GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
  glEnableVertexAttribArray(1);

  //UVs VBO setup
  GLuint uvs_VBO;
  glGenBuffers(1, &uvs_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
  glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *) 0);
  glEnableVertexAttribArray(2);

  //EBO setup
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               vertexIndices.size() * sizeof(int),
               &vertexIndices.front(),
               GL_STATIC_DRAW);

  glBindVertexArray(0);
  // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
  vertexCount = vertexIndices.size();
  return VAO;
}

#pragma endregion // Window callback functions

// render line of text
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color) {
  // activate corresponding render state
  shader.use();
  glUniform3f(glGetUniformLocation(shader.id, "textColor"), color.x, color.y, color.z);
  shader.setInt("text", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    float xpos = x + ch.Bearing.x * scale;
    float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;
    // update VBO for each character
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},
        {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f},
        {xpos + w, ypos, 1.0f, 1.0f},
        {xpos + w, ypos + h, 1.0f, 0.0f}
    };
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(vertices),
                    vertices); // be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.Advance >> 6)
        * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}