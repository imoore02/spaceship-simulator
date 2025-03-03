#ifndef SPACESHIP_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31
#define SPACESHIP_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31

#include <array>
#include <iostream>

#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shape.hpp"

class Spaceship {
 public:
  Spaceship();
  void resetState();
  void updateMatrices();

  const Vec3f& getPosition() const { return position; }

  void launch() { animationRunning = !animationRunning; }
  void animate(float dt);
  void updateKeyActions(int aKey, int aAction);

  const std::array<Vec3f, 3> getLightPos() const {
    // Light positions in world space
    std::array<Vec3f, 3> array = {};
    for (int i = 0; i < 3; i++) {
      Vec4f pos = model2world * Vec4f{lightOffsets[i].x, lightOffsets[i].y,
                                      lightOffsets[i].z, 1.f};
      array[i] = Vec3f{pos.x, pos.y, pos.z};
    }
    return array;
  }

  const std::array<Vec3f, 3> getLightAmbient() const { return lightAmbient; }
  const std::array<Vec3f, 3> getLightDiffuse() const { return lightDiffuse; }

  void draw(const Mat44f& cameraProjection) const {
    glUniformMatrix4fv(0, 1, GL_TRUE, cameraProjection.v);
    glUniformMatrix4fv(1, 1, GL_TRUE, model2world.v);
    glUniformMatrix3fv(2, 1, GL_TRUE, normalMatrix.v);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
  }

 private:
  GLuint vao;
  GLsizei numVertices;

  // Animation
  float time;
  Vec3f initialPosition;
  Vec3f position;
  float rotationZ;

  Mat44f model2world;
  Mat33f normalMatrix;

  bool animationRunning;

  // Lights
  std::array<Vec3f, 3> lightOffsets;
  std::array<Vec3f, 3> lightAmbient;
  std::array<Vec3f, 3> lightDiffuse;
};

#endif  // SPACESHIP_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31
