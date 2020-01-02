#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>

struct Vertex {
  float x;
  float y;
  float z;
  float r;
  float g;
  float b;
};

static const std::vector<Vertex> kCubeVertices = {
  // Front
  {0, 1, 1, 1, 0, 0},
  {1, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 0, 0},
  {1, 0, 1, 1, 0, 0},

  // Back
  {1, 1, 0, 1, 0, 0},
  {0, 1, 0, 1, 0, 0},
  {1, 0, 0, 1, 0, 0},
  {0, 0, 0, 1, 0, 0},

  // Left
  {0, 1, 0, 0, 1, 0},
  {0, 1, 1, 0, 1, 0},
  {0, 0, 0, 0, 1, 0},
  {0, 0, 1, 0, 1, 0},

  // Right
  {1, 1, 1, 0, 1, 0},
  {1, 1, 0, 0, 1, 0},
  {1, 0, 1, 0, 1, 0},
  {1, 0, 0, 0, 1, 0},

  // Top
  {0, 1, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 1},
  {0, 1, 1, 0, 0, 1},
  {1, 1, 1, 0, 0, 1},

  // Bottom
  {0, 0, 1, 0, 0, 1},
  {1, 0, 1, 0, 0, 1},
  {0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 1},
};

static const std::vector<unsigned int> kCubeIndices = {
  // Front
  2, 1, 0, 2, 3, 1,

  // Back
  6, 5, 4, 6, 7, 5,

  // Left
  10, 9, 8, 10, 11, 9,

  // Right
  14, 13, 12, 14, 15, 13,

  // Top
  18, 17, 16, 18, 19, 17,

  // Bottom
  22, 21, 20, 22, 23, 21,
};

#endif  // GEOMETRY_H_
