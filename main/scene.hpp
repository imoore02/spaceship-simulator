#ifndef SCENE_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31
#define SCENE_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31

#include <glad/glad.h>

#include <array>

#include "../support/program.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"
#include "mesh.hpp"
#include "texture.hpp"

// Lighting
class Lighting {
 public:
  Lighting() {
    globalLightDirection = normalize(Vec3f{0.f, 1.f, -1.f});
    globalLightAmbient = {0.05f, 0.05f, 0.05f};
    globalLightDiffuse = {0.9f, 0.9f, 0.6f};

    // Other initialisation
    viewpoint = {};
    pointLightPos = {};
    pointLightAmbient = {};
    pointLightDiffuse = {};
  }

  void updateLighting(const Vec3f iViewpoint,
                      const std::array<Vec3f, 3> iPointLightPos,
                      const std::array<Vec3f, 3> iPointLightAmbient,
                      const std::array<Vec3f, 3> iPointLightDiffuse) {
    viewpoint = iViewpoint;
    pointLightPos = iPointLightPos;
    pointLightAmbient = iPointLightAmbient;
    pointLightDiffuse = iPointLightDiffuse;
  }

  void setLighting() const {
    glUniform3fv(3, 1, &globalLightDirection.x);
    glUniform3fv(4, 1, &globalLightAmbient.x);
    glUniform3fv(5, 1, &globalLightDiffuse.x);
    glUniform3fv(6, 1, &viewpoint.x);
    glUniform3fv(7, 3, &pointLightPos[0].x);
    glUniform3fv(10, 3, &pointLightAmbient[0].x);
    glUniform3fv(13, 3, &pointLightDiffuse[0].x);
  }

 private:
  Vec3f globalLightDirection;
  Vec3f globalLightAmbient;
  Vec3f globalLightDiffuse;
  Vec3f viewpoint;
  std::array<Vec3f, 3> pointLightPos;
  std::array<Vec3f, 3> pointLightAmbient;
  std::array<Vec3f, 3> pointLightDiffuse;
};

class Scene {
 public:
  Scene() {
    MeshData groundMesh = load_wavefront_obj("assets/cw2/langerso.obj", true);
    groundVao = create_vao(groundMesh);
    groundVertices = GLuint(groundMesh.positions.size());
    groundTexture = load_texture_2d("assets/cw2/L3211E-4k.jpg");

    // Launchpad
    MeshData lpadMesh = load_wavefront_obj("assets/cw2/landingpad.obj", false);
    lpadVao = create_vao(lpadMesh);
    lpadVertices = GLuint(lpadMesh.positions.size());

    lpadModel2World1 =
        make_translation({5.f, 0.f, -5.f}) * make_rotation_y(1.f);
    lpadModel2World2 =
        make_translation({-5.f, 0.f, 3.5f}) * make_rotation_y(-0.5f);

    lpadNormalMatrix1 = mat44_to_mat33(transpose(invert(lpadModel2World1)));
    lpadNormalMatrix2 = mat44_to_mat33(transpose(invert(lpadModel2World2)));
  }

  void drawGround(const Mat44f& cameraProjection) const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glUniformMatrix4fv(0, 1, GL_TRUE, cameraProjection.v);
    glUniformMatrix4fv(1, 1, GL_TRUE, kIdentity44f.v);
    glUniformMatrix3fv(2, 1, GL_TRUE, kIdentity33f.v);

    glBindVertexArray(groundVao);
    glDrawArrays(GL_TRIANGLES, 0, groundVertices);
  }

  void drawLaunchpads(const Mat44f& cameraProjection) const {
    // Must set lighting first

    // Launchpad 1
    glUniformMatrix4fv(0, 1, GL_TRUE, cameraProjection.v);
    glUniformMatrix4fv(1, 1, GL_TRUE, lpadModel2World1.v);
    glUniformMatrix3fv(2, 1, GL_TRUE, lpadNormalMatrix1.v);
    glBindVertexArray(lpadVao);
    glDrawArrays(GL_TRIANGLES, 0, lpadVertices);

    // Launchpad 2
    glUniformMatrix4fv(0, 1, GL_TRUE, cameraProjection.v);
    glUniformMatrix4fv(1, 1, GL_TRUE, lpadModel2World2.v);
    glUniformMatrix3fv(2, 1, GL_TRUE, lpadNormalMatrix2.v);
    glBindVertexArray(lpadVao);
    glDrawArrays(GL_TRIANGLES, 0, lpadVertices);
  }

 private:
  // Ground
  GLuint groundVao;
  GLsizei groundVertices;
  GLuint groundTexture;

  // Launchpad
  GLuint lpadVao;
  GLsizei lpadVertices;

  Mat44f lpadModel2World1;
  Mat44f lpadModel2World2;

  Mat33f lpadNormalMatrix1;
  Mat33f lpadNormalMatrix2;
};

#endif  // SCENE_HPP_D0746DED_C9C6_40CD_B6E0_C6FEF665DD31
