#ifndef CAMERA_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define CAMERA_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <GLFW/glfw3.h>

#include <iostream>

#include "../vmlib/mat44.hpp"
#include "../vmlib/vec3.hpp"

constexpr float kMovementPerSecond = 1.f;   // units per second
constexpr float kMouseSensitivity = 0.01f;  // radians per pixel
constexpr float kSpeedFactor = 5.f;
constexpr float kSlowFactor = 0.2f;

class Camera {
 public:
  Camera(Vec3f pos);
  const Vec3f getCamWorldPosition() const { return pos; }

  // Camera position methods
  void updateState(float dt);
  void track(const Vec3f& position);
  void pointAt(const Vec3f& position);

  void resetState(GLFWwindow* aWindow);
  const Mat44f getProjection(float aspect) const;

  // Input polling
  void updateKeyActions(int aKey, int aAction);
  void updateMouseMovement(double aX, double aY);
  void updateMouseClicked(GLFWwindow* aWindow, int aButton, int aAction);

 private:
  Vec3f pos;
  float pitch, yaw;
  Mat44f world2camera;

  bool mouseActive;
  bool aForward, aBackward, aLeft, aRight, aUp, aDown;
  bool aSpeedUp, aSlowDown;

  float lastMouseX, lastMouseY;

  void moveDirection(const Vec3f& dir);

  void moveAngle(float addPitch, float addYaw);
};

#endif  // CAMERA_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9