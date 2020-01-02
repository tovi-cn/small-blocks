#include "utilities.h"

#include <cstdlib>
#include <ctime>

void SeedRandom() {
  srand(time(NULL));
}

float RandomFloat() {
  return static_cast<float>(rand()) / RAND_MAX;
}
