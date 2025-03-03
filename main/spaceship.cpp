#include "spaceship.hpp"

#include <GLFW/glfw3.h>

#include <array>
#include <numbers>

#include "../vmlib/mat33.hpp"

Spaceship::Spaceship() {
  // Global Colours
  Vec3f red = {1.f, 0.f, 0.f};
  Vec3f green = {0.f, 1.f, 0.f};
  Vec3f blue = {0.f, 0.f, 1.f};
  Vec3f black = {0.f, 0.f, 0.f};
  Vec3f white = {0.1f, 0.1f, 0.1f};

  // Some parts relative to central body transformation
  Mat44f centralBody = make_rotation_z(0.5f * std::numbers::pi_v<float>) *
                       make_scaling(2.f, 5.f, 5.f);
  MeshData body1 =
      make_cylinder(true, 32, centralBody, white, white, white, 100, black);

  // Smaller scaling + offset from central body
  MeshData body2 =
      make_cylinder(false, 32,
                    make_translation({0.f, 2.f, 0.f}) * centralBody *
                        make_scaling(0.5f, 0.8f, 0.81f),
                    white, white, white, 100, black);

  // Smaller width + offset from central body
  MeshData cone1 = make_cone(true, 32,
                             make_translation({0.f, 3.f, 0.f}) * centralBody *
                                 make_scaling(0.5f, 1.f, 1.f),
                             white, white, white, 100, black);

  // Larger radius, smaller width + offset from central body
  MeshData cone2 = make_cone(true, 32,
                             make_translation({0.f, 1.f, 0.f}) * centralBody *
                                 make_scaling(0.5f, 1.6f, 1.6f),
                             white, white, white, 100, black);

  // Smaller width and flipped compared to central body
  MeshData cone3 =
      make_cone(false, 32,
                centralBody * make_rotation_z(std::numbers::pi_v<float>) *
                    make_scaling(0.5f, 1.f, 1.f),
                white, white, white, 100, black);

  Mat44f legTransform = make_translation({2.f, 0.f, 0.f}) *
                        make_rotation_z(-0.5f * std::numbers::pi_v<float>) *
                        make_scaling(2.f, 0.1f, .1f);

  // Relative to leg transform
  MeshData leg1 =
      make_cylinder(true, 32, legTransform, white, white, white, 100, black);

  MeshData leg2 = make_cylinder(
      true, 32,
      make_rotation_y(2.f * std::numbers::pi_v<float> / 3.f) * legTransform,
      white, white, white, 100, black);

  MeshData leg3 = make_cylinder(
      true, 32,
      make_rotation_y(4.f * std::numbers::pi_v<float> / 3.f) * legTransform,
      white, white, white, 100, black);

  MeshData stem =
      make_cylinder(true, 32,
                    make_translation({0.f, 4.f, 0.f}) *
                        make_rotation_z(0.5f * std::numbers::pi_v<float>) *
                        make_scaling(2.f, 0.05f, 0.05f),
                    white, white, white, 100, black);

  MeshData ball1 = make_sphere(
      2, make_translation({0.f, 5.f, 0.f}) * make_scaling(0.5f, 0.5f, 0.5f),
      white, white, white, 100, black);

  MeshData ball2 = make_sphere(
      2, make_translation({0.f, 8.f, 0.f}) * make_scaling(0.5f, 2.f, 0.5f),
      white, white, white, 100, black);

  // Concatenate all parts of the spaceship
  MeshData mesh = concatenate(std::move(body1), body2);
  mesh = concatenate(std::move(mesh), cone1);
  mesh = concatenate(std::move(mesh), cone2);
  mesh = concatenate(std::move(mesh), cone3);
  mesh = concatenate(std::move(mesh), leg1);
  mesh = concatenate(std::move(mesh), leg2);
  mesh = concatenate(std::move(mesh), leg3);
  mesh = concatenate(std::move(mesh), ball1);
  mesh = concatenate(std::move(mesh), ball2);
  mesh = concatenate(std::move(mesh), stem);

  Mat44f scaling = make_scaling(0.04f, 0.04f, 0.04f);
  // Apply scaling to all vertices
  // Normals are not effected
  for (auto& p : mesh.positions) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = scaling * p4;
    t /= t.w;
    p = Vec3f{t.x, t.y, t.z};
  }

  vao = create_vao(mesh);
  numVertices = GLsizei(mesh.positions.size());

  // Light
  lightOffsets = {Vec3f{0.21f, -0.02f, 0.f}, Vec3f{-0.21f, -0.02f, 0.f},
                  Vec3f{0.f, 0.45f, 0.f}};
  lightAmbient = {red / 1000, green / 1000, blue / 1000};
  lightDiffuse = {Vec3f{1.f, 0.2f, 0.2f}, Vec3f{0.2f, 1.f, 0.2f},
                  Vec3f{0.2f, 0.2f, 1.f}};
  // Animation
  initialPosition = Vec3f{-5.0f, 0.1f, 3.5};

  resetState();
}

void Spaceship::resetState() {
  time = 0.f;
  position = initialPosition;
  rotationZ = 0;
  animationRunning = false;

  updateMatrices();
}

void Spaceship::updateMatrices() {
  model2world = make_translation(position) * make_rotation_z(rotationZ);
  normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
}

void Spaceship::animate(float dt) {
  if (animationRunning) {
    time += dt;
    float kX = 0.005f;
    float kY = 0.08f;
    float x = kX * time * time * time;
    float y = kY * time * time;
    position = initialPosition + Vec3f{x, y, 0.f};

    // Rotation
    float dx = 3 * kX * time * time;
    float dy = 2 * kY * time;
    rotationZ = std::atan2(dy, dx) - 0.5f * std::numbers::pi_v<float>;

    updateMatrices();
  }
}

void Spaceship::updateKeyActions(int aKey, int aAction) {
  // F-Key start and stop animation
  if (GLFW_KEY_F == aKey && GLFW_PRESS == aAction) {
    animationRunning = !animationRunning;
  }

  // R-Key spaceship animation reset
  if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction) {
    resetState();
  }
}
