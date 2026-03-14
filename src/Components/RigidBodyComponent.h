#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include <glm/glm.hpp>

struct RigidBodyComponent {
  glm::vec2 velocity;

  // We initialize the arguments with default values in case no values are
  // passed. If we pass our values, the default ones will not be counted, only
  // the ones passed.
  RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)) {
    this->velocity = velocity;
  }
};

#endif