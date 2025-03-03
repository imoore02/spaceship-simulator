#ifndef STATE_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define STATE_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include "../support/program.hpp"
#include "camera.hpp"
#include "spaceship.hpp"

class Button;

struct State {
  ShaderProgram *prog;

  Camera *firstPersonCamera;
  Camera *trackingCamera;
  Camera *groundedCamera;
  Camera *leftScreenCamera;
  Camera *rightScreenCamera;
  bool splitScreen;

  Spaceship *spaceship;

  // UI
  std::vector<Button *> buttons;
};

#endif  // STATE_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9