#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>

struct Vertex {
  // Position
  float x;
  float y;
  float z;

  // Normal
  float nx;
  float ny;
  float nz;

  // Color
  float r;
  float g;
  float b;
};

extern const std::vector<Vertex> kCubeVertices;
extern const std::vector<unsigned int> kCubeIndices;

#endif  // GEOMETRY_H_
