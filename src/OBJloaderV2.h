#include <glm/glm.hpp>
#include <cstring>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//For loading .obj files using a polygon list format

bool loadOBJ2(
    const char *path,
    std::vector<unsigned int> &vertexIndices,
    std::vector<glm::vec3> &temp_vertices,
    std::vector<glm::vec3> &out_normals,
    std::vector<glm::vec2> &out_uvs) {

  std::vector<int> uvIndices, normalIndices;
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  FILE *file;
  file = fopen(path, "r");
  if (!file) {
    printf("Impossible to open the file ! Are you in the right path ?\n");
    getchar();
    return false;
  }

  while (1) {

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
      break; // EOF = End Of File. Quit the loop.

    // else : parse lineHeader

    if (strcmp(lineHeader, "v") == 0) {
      glm::vec3 vertex;
      res = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

      temp_vertices.emplace_back(vertex);
    } else if (strcmp(lineHeader, "vt") == 0) {
      glm::vec2 uv;
      res = fscanf(file, "%f %f\n", &uv.x, &uv.y);
      if (res != 2) {
        printf("Missing uv information!\n");
      }
      uv.y =
          -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
      temp_uvs.emplace_back(uv);
    } else if (strcmp(lineHeader, "vn") == 0) {
      glm::vec3 normal;
      res = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
      if (res != 3) {
        printf("Missing normal information!\n");
      }
      temp_normals.emplace_back(normal);
    } else if (strcmp(lineHeader, "f") == 0) {
      char *getRes;
      int vertexIndex[3], uvIndex[3], normalIndex[3];
      bool uv = true;
      bool norm = true;
      char line[128];
      getRes = fgets(line, 128, file);
      if (getRes == 0) {
        printf("incomplete face\n");
      }

      //vertex, uv, norm
      int matches = sscanf(line,
                           "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                           &vertexIndex[0],
                           &uvIndex[0],
                           &normalIndex[0],
                           &vertexIndex[1],
                           &uvIndex[1],
                           &normalIndex[1],
                           &vertexIndex[2],
                           &uvIndex[2],
                           &normalIndex[2]);
      if (matches != 9) {
        //vertex, norm
        matches = sscanf(line,
                         "%d//%d %d//%d %d//%d\n",
                         &vertexIndex[0],
                         &normalIndex[0],
                         &vertexIndex[1],
                         &normalIndex[1],
                         &vertexIndex[2],
                         &normalIndex[2]);
        if (matches != 6) {
          //vertex, uv
          matches = sscanf(line,
                           "%d/%d %d/%d %d/%d\n",
                           &vertexIndex[0],
                           &uvIndex[0],
                           &vertexIndex[1],
                           &uvIndex[1],
                           &vertexIndex[2],
                           &uvIndex[2]);
          if (matches != 6) {
            //vertex
            matches = sscanf(line, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
            if (matches != 3) {
              printf(
                  "File can't be read by our simple parser. 'f' format expected: d/d/d d/d/d d/d/d || d/d d/d d/d || d//d d//d d//d\n");
              printf("Character at %ld", ftell(file));
              return false;
            }
            uv, norm = false;
          } else {
            norm = false;
          }
        } else {
          uv = false;
        }
      }
      vertexIndices.push_back(abs(vertexIndex[0]) - 1);
      vertexIndices.push_back(abs(vertexIndex[1]) - 1);
      vertexIndices.push_back(abs(vertexIndex[2]) - 1);
      if (norm) {
        normalIndices.push_back(abs(normalIndex[0]) - 1);
        normalIndices.push_back(abs(normalIndex[1]) - 1);
        normalIndices.push_back(abs(normalIndex[2]) - 1);
      }
      if (uv) {
        uvIndices.push_back(abs(uvIndex[0]) - 1);
        uvIndices.push_back(abs(uvIndex[1]) - 1);
        uvIndices.push_back(abs(uvIndex[2]) - 1);
      }
    } else {
      char clear[1000];
      char *getsRes = fgets(clear, 1000, file);
    }
  }
  if (!normalIndices.empty())
    out_normals.resize(temp_normals.size());
  if (!uvIndices.empty())
    out_uvs.resize(temp_uvs.size());
  for (unsigned int i = 0; i < vertexIndices.size(); i++) {
    unsigned vi = vertexIndices[i];
    if (!normalIndices.empty()) {
      int ni = normalIndices[i];
      out_normals[vi] = temp_normals[ni];
    }
    if (!uvIndices.empty() && i < uvIndices.size()) {
      int ui = uvIndices[i];
      out_uvs[vi] = temp_uvs[ui];
    }
  }

  return true;
}
