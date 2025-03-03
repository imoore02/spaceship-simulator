#ifndef MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad/glad.h>

#include <vector>

#include "../vmlib/vec2.hpp"
#include "../vmlib/vec3.hpp"

struct MeshData {
  std::vector<Vec3f> positions;
  std::vector<Vec3f> normals;
  // Only used for texture objects
  std::vector<Vec2f> texcoords;

  // Used for lighting and color
  std::vector<Vec3f> ambient;

  // Blinn-Phong parameters
  std::vector<Vec3f> diffuse;
  std::vector<Vec3f> specular;
  std::vector<float> shininess;
  std::vector<Vec3f> emissive;
};

MeshData concatenate(MeshData, MeshData const&);

GLuint create_vao(MeshData const&);

MeshData load_wavefront_obj(char const* aPath, bool useTexture);

#endif  // MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
