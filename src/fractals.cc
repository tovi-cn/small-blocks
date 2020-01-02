#include "fractals.h"

void SimpleFractal(Block *block, int depth) {
  if (depth <= 0) {
    return;
  }
  block->set_child(depth % 2 == 0 ? 0 : 7, new Block(100));
  block->set_child(depth % 2 == 0 ? 7 : 0, new Block());
  SimpleFractal(block->child(depth % 2 == 0 ? 7 : 0), depth - 1);
}
