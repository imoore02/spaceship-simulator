#include "mesh.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

MeshData concatenate(MeshData aM, MeshData const& aN) {
  aM.positions.insert(aM.positions.end(), aN.positions.begin(),
                      aN.positions.end());
  aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());

  aM.texcoords.insert(aM.texcoords.end(), aN.texcoords.begin(),
                      aN.texcoords.end());
  aM.ambient.insert(aM.ambient.end(), aN.ambient.begin(), aN.ambient.end());
  aM.diffuse.insert(aM.diffuse.end(), aN.diffuse.begin(), aN.diffuse.end());
  aM.specular.insert(aM.specular.end(), aN.specular.begin(), aN.specular.end());
  aM.shininess.insert(aM.shininess.end(), aN.shininess.begin(),
                      aN.shininess.end());
  aM.emissive.insert(aM.emissive.end(), aN.emissive.begin(), aN.emissive.end());
  return aM;
}

GLuint create_vao(MeshData const& aMeshData) {
  GLuint positionVBO = 0;
  GLuint normalVBO = 0;
  GLuint textureVBO = 0;
  GLuint ambientVBO = 0;
  GLuint diffuseVBO = 0;
  GLuint specularVBO = 0;
  GLuint shininessVBO = 0;
  GLuint emissiveVBO = 0;
  if (aMeshData.positions.size() > 0) {
    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f),
                 aMeshData.positions.data(), GL_STATIC_DRAW);
  }
  if (aMeshData.normals.size() > 0) {
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f),
                 aMeshData.normals.data(), GL_STATIC_DRAW);
  }

  if (aMeshData.texcoords.size() > 0) {
    glGenBuffers(1, &textureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f),
                 aMeshData.texcoords.data(), GL_STATIC_DRAW);
  }
  if (aMeshData.ambient.size() > 0) {
    glGenBuffers(1, &ambientVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ambientVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.ambient.size() * sizeof(Vec3f),
                 aMeshData.ambient.data(), GL_STATIC_DRAW);
  }
  if (aMeshData.diffuse.size() > 0) {
    glGenBuffers(1, &diffuseVBO);
    glBindBuffer(GL_ARRAY_BUFFER, diffuseVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.diffuse.size() * sizeof(Vec3f),
                 aMeshData.diffuse.data(), GL_STATIC_DRAW);
  }
  if (aMeshData.specular.size() > 0) {
    glGenBuffers(1, &specularVBO);
    glBindBuffer(GL_ARRAY_BUFFER, specularVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.specular.size() * sizeof(Vec3f),
                 aMeshData.specular.data(), GL_STATIC_DRAW);
  }
  if (aMeshData.shininess.size() > 0) {
    glGenBuffers(1, &shininessVBO);
    glBindBuffer(GL_ARRAY_BUFFER, shininessVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.shininess.size() * sizeof(float),
                 aMeshData.shininess.data(), GL_STATIC_DRAW);
  }
  if (aMeshData.emissive.size() > 0) {
    glGenBuffers(1, &emissiveVBO);
    glBindBuffer(GL_ARRAY_BUFFER, emissiveVBO);
    glBufferData(GL_ARRAY_BUFFER, aMeshData.emissive.size() * sizeof(Vec3f),
                 aMeshData.emissive.data(), GL_STATIC_DRAW);
  }

  // VAO
  GLuint vao = 0;
  int index = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  if (aMeshData.positions.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.normals.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.texcoords.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.ambient.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, ambientVBO);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.diffuse.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, diffuseVBO);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.specular.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, specularVBO);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.shininess.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, shininessVBO);
    glVertexAttribPointer(index, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }
  if (aMeshData.emissive.size() > 0) {
    glBindBuffer(GL_ARRAY_BUFFER, emissiveVBO);
    glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    index++;
  }

  // Reset State
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &positionVBO);
  glDeleteBuffers(1, &normalVBO);
  glDeleteBuffers(1, &textureVBO);
  glDeleteBuffers(1, &ambientVBO);
  glDeleteBuffers(1, &diffuseVBO);
  glDeleteBuffers(1, &specularVBO);
  glDeleteBuffers(1, &shininessVBO);
  glDeleteBuffers(1, &emissiveVBO);

  return vao;
}

MeshData load_wavefront_obj(char const* aPath, bool useTexture) {
  // rapidobj to load file
  auto result = rapidobj::ParseFile(aPath);
  if (result.error)
    throw Error("Unable to load OBJ file '%s': %s", aPath,
                result.error.code.message().c_str());

  // Must triangulate for OpenGL rendering
  rapidobj::Triangulate(result);
  // Convert the OBJ data into a MeshData structure
  MeshData ret;
  for (auto const& shape : result.shapes) {
    for (std::size_t i = 0; i < shape.mesh.indices.size(); i++) {
      auto const& idx = shape.mesh.indices[i];
      ret.positions.emplace_back(
          Vec3f{result.attributes.positions[idx.position_index * 3 + 0],
                result.attributes.positions[idx.position_index * 3 + 1],
                result.attributes.positions[idx.position_index * 3 + 2]});

      ret.normals.emplace_back(
          Vec3f{result.attributes.normals[idx.normal_index * 3 + 0],
                result.attributes.normals[idx.normal_index * 3 + 1],
                result.attributes.normals[idx.normal_index * 3 + 2]});
      if (useTexture) {
        ret.texcoords.emplace_back(
            Vec2f{result.attributes.texcoords[idx.texcoord_index * 2 + 0],
                  result.attributes.texcoords[idx.texcoord_index * 2 + 1]});
      } else {
        auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
        // Use ambient color
        ret.ambient.emplace_back(
            Vec3f{mat.ambient[0], mat.ambient[1], mat.ambient[2]});
        ret.diffuse.emplace_back(
            Vec3f{mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]});
        ret.specular.emplace_back(
            Vec3f{mat.specular[0], mat.specular[1], mat.specular[2]});
        ret.shininess.emplace_back(mat.shininess);
        ret.emissive.emplace_back(
            Vec3f{mat.emission[0], mat.emission[1], mat.emission[2]});
      }
    }
  }
  return ret;
}
