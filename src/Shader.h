//
// Created by Yuguo Zheng on 2021-07-14.
//


#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>   // Include GLEW - OpenGL Extension Wrangler
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
    unsigned int id;

    // constructor: read source from file
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    // use shader
    void use() const;

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
};

#endif //SHADER_HPP
