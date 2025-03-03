#include "camera.hpp"

#include <cmath>
#include <iostream>
#include <numbers>

#include "../vmlib/mat33.hpp"
#include "../vmlib/vec3.hpp"

Camera::Camera(Vec3f pos)
    : pos(pos),
      pitch(0.f),
      yaw(0.f),
      mouseActive(false),
      aForward(false),
      aBackward(false),
      aLeft(false),
      aRight(false),
      aUp(false),
      aDown(false),
      aSpeedUp(false),
      aSlowDown(false),
      lastMouseX(0.f),
      lastMouseY(0.f) {
  world2camera = kIdentity44f;
}

void Camera::updateState(float dt) {
  float dist = kMovementPerSecond * dt;

  if (aSpeedUp) {
    dist = dist * kSpeedFactor;
  }
  if (aSlowDown) {
    dist = dist * kSlowFactor;
  }
  if (aForward) {
    moveDirection(dist * Vec3f{0.f, 0.f, 1.f});
  }
  if (aBackward) {
    moveDirection(dist * Vec3f{0.f, 0.f, -1.f});
  }
  if (aLeft) {
    moveDirection(dist * Vec3f{1.f, 0.f, 0.f});
  }
  if (aRight) {
    moveDirection(dist * Vec3f{-1.f, 0.f, 0.f});
  }
  if (aUp) {
    moveDirection(dist * Vec3f{0.f, -1.f, 0.f});
  }
  if (aDown) {
    moveDirection(dist * Vec3f{0.f, 1.f, 0.f});
  }
}

void Camera::track(const Vec3f& position) {
  Vec3f offset = {0.f, 0.f, 1.f};
  pos = position + offset;
}

void Camera::pointAt(const Vec3f& position) {
  Vec3f direction = normalize(position - pos);
  pitch = -asin(direction.y);
  yaw = atan(direction.z / direction.x) - std::numbers::pi_v<float> / 2.f;
}

void Camera::resetState(GLFWwindow* aWindow) {
  mouseActive = false;
  aForward = false;
  aBackward = false;
  aLeft = false;
  aRight = false;
  aUp = false;
  aDown = false;
  aSpeedUp = false;
  aSlowDown = false;
  glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

const Mat44f Camera::getProjection(float aspect) const {
  return make_perspective_projection(std::numbers::pi_v<float> / 3.f, aspect,
                                     0.1f, 100.0f) *
         make_rotation_x(pitch) * make_rotation_y(yaw) * make_translation(-pos);
}

void Camera::updateKeyActions(int aKey, int aAction) {
  switch (aKey) {
    case GLFW_KEY_W:
      if (aAction == GLFW_PRESS) {
        aForward = true;
      } else if (aAction == GLFW_RELEASE) {
        aForward = false;
      }
      break;
    case GLFW_KEY_S:
      if (aAction == GLFW_PRESS) {
        aBackward = true;
      } else if (aAction == GLFW_RELEASE) {
        aBackward = false;
      }
      break;
    case GLFW_KEY_A:
      if (aAction == GLFW_PRESS) {
        aLeft = true;
      } else if (aAction == GLFW_RELEASE) {
        aLeft = false;
      }
      break;
    case GLFW_KEY_D:
      if (aAction == GLFW_PRESS) {
        aRight = true;
      } else if (aAction == GLFW_RELEASE) {
        aRight = false;
      }
      break;
    case GLFW_KEY_Q:
      if (aAction == GLFW_PRESS) {
        aDown = true;
      } else if (aAction == GLFW_RELEASE) {
        aDown = false;
      }
      break;
    case GLFW_KEY_E:
      if (aAction == GLFW_PRESS) {
        aUp = true;
      } else if (aAction == GLFW_RELEASE) {
        aUp = false;
      }
      break;
    case GLFW_KEY_LEFT_CONTROL:
    case GLFW_KEY_RIGHT_CONTROL:
      if (aAction == GLFW_PRESS) {
        aSlowDown = true;
      } else if (aAction == GLFW_RELEASE) {
        aSlowDown = false;
      }
      break;
    case GLFW_KEY_LEFT_SHIFT:
    case GLFW_KEY_RIGHT_SHIFT:
      if (aAction == GLFW_PRESS) {
        aSpeedUp = true;
      } else if (aAction == GLFW_RELEASE) {
        aSpeedUp = false;
      }
      break;
  }
}

void Camera::updateMouseMovement(double aX, double aY) {
  if (mouseActive) {
    auto const dx = float(aX - lastMouseX);
    auto const dy = float(aY - lastMouseY);

    float addPitch = dy * kMouseSensitivity;
    float addYaw = dx * kMouseSensitivity;

    moveAngle(addPitch, addYaw);

    // Clamp pitch values to prevent inverted view
    if (pitch > std::numbers::pi_v<float> / 2.f - 0.1f) {
      pitch = std::numbers::pi_v<float> / 2.f - 0.1f;
    } else if (pitch < -std::numbers::pi_v<float> / 2.f + 0.1f) {
      pitch = -std::numbers::pi_v<float> / 2.f + 0.1f;
    }
  }
  lastMouseX = float(aX);
  lastMouseY = float(aY);
}

void Camera::updateMouseClicked(GLFWwindow* aWindow, int aButton, int aAction) {
  if (aButton == GLFW_MOUSE_BUTTON_RIGHT && aAction == GLFW_PRESS) {
    mouseActive = !mouseActive;

    if (mouseActive)
      glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    else
      glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void Camera::moveDirection(const Vec3f& dir) {
  Vec4f addDirection = make_rotation_x(-pitch) * make_rotation_y(-yaw) *
                       Vec4f{dir.x, dir.y, dir.z, 1.f};
  pos -= Vec3f{addDirection.x, addDirection.y, addDirection.z};
}

void Camera::moveAngle(float addPitch, float addYaw) {
  // Update roatations
  pitch += addPitch;
  yaw += addYaw;

  // Ensure yaw values stay reasonable
  if (yaw >= 2.f * std::numbers::pi_v<float>) {
    yaw -= 2.f * std::numbers::pi_v<float>;
  } else if (yaw <= 2.f * std::numbers::pi_v<float>) {
    yaw += 2.f * std::numbers::pi_v<float>;
  }
}