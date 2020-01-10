#include "physics.h"

Body::Body()
   : position_(0.0f), velocity_(0.0f), acceleration_(0.0f),
     fixed_(false) {}

Body::~Body() {
}

BoxBody::BoxBody(glm::vec3 size) : size_(size) {}

BoxBody::~BoxBody() {
}
