#include "shape.hpp"

#include <array>
#include <iostream>
#include <numbers>

MeshData make_cylinder(bool capped, const std::size_t subdivs,
                       const Mat44f& preTransform, const Vec3f& ambient,
                       const Vec3f& diffuse, const Vec3f& specular,
                       float shininess, const Vec3f& emissive) {
  GLuint numVertices = 3 * (2 + (capped ? 2 : 0)) * GLuint(subdivs);
  std::vector<Vec3f> pos(numVertices);
  std::vector<Vec3f> norm(numVertices);

  float prevY = std::sin(0.f);
  float prevZ = std::cos(0.f);
  std::size_t index = 0;
  for (std::size_t i = 0; i < subdivs; i++) {
    float const angle =
        (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

    float y = std::sin(angle);
    float z = std::cos(angle);

    // Normals point out from unit circle
    norm[index] = Vec3f{0.f, prevY, prevZ};
    pos[index++] = Vec3f{0.f, prevY, prevZ};
    norm[index] = Vec3f{0.f, prevY, prevZ};
    pos[index++] = Vec3f{1.f, prevY, prevZ};
    norm[index] = Vec3f{0.f, y, z};
    pos[index++] = Vec3f{0.f, y, z};

    norm[index] = Vec3f{0.f, y, z};
    pos[index++] = Vec3f{0.f, y, z};
    norm[index] = Vec3f{0.f, prevY, prevZ};
    pos[index++] = Vec3f{1.f, prevY, prevZ};
    norm[index] = Vec3f{0.f, y, z};
    pos[index++] = Vec3f{1.f, y, z};

    if (capped) {
      norm[index] = Vec3f{-1.f, 0.f, 0.f};
      pos[index++] = Vec3f{0.f, 0.f, 0.f};
      norm[index] = Vec3f{-1.f, 0.f, 0.f};
      pos[index++] = Vec3f{0.f, prevY, prevZ};
      norm[index] = Vec3f{-1.f, 0.f, 0.f};
      pos[index++] = Vec3f{0.f, y, z};

      norm[index] = Vec3f{1.f, 0.f, 0.f};
      pos[index++] = Vec3f{1.f, 0.f, 0.f};
      norm[index] = Vec3f{1.f, 0.f, 0.f};
      pos[index++] = Vec3f{1.f, y, z};
      norm[index] = Vec3f{1.f, 0.f, 0.f};
      pos[index++] = Vec3f{1.f, prevY, prevZ};
    }
    prevY = y;
    prevZ = z;
  }

  for (auto& p : pos) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = preTransform * p4;
    t /= t.w;
    p = Vec3f{t.x, t.y, t.z};
  }

  Mat33f const N = mat44_to_mat33(transpose(invert(preTransform)));
  for (auto& n : norm) {
    n = N * n;
  }

  std::vector<Vec3f> ambientV(numVertices, ambient);
  std::vector<Vec3f> diffuseV(numVertices, diffuse);
  std::vector<Vec3f> specularV(numVertices, specular);
  std::vector<float> shininessV(numVertices, shininess);
  std::vector<Vec3f> emissiveV(numVertices, emissive);

  return MeshData{
      std::move(pos),        std::move(norm),     {},
      std::move(ambientV),   std::move(diffuseV), std::move(specularV),
      std::move(shininessV), std::move(emissiveV)};
}

MeshData make_cone(bool capped, const std::size_t subdivs,
                   const Mat44f& preTransform, const Vec3f& ambient,
                   const Vec3f& diffuse, const Vec3f& specular, float shininess,
                   const Vec3f& emissive) {
  GLuint numVertices = 3 * (1 + (capped ? 1 : 0)) * GLuint(subdivs);
  std::vector<Vec3f> pos(numVertices);
  std::vector<Vec3f> norm(numVertices);

  float prevY = std::sin(0.f);
  float prevZ = std::cos(0.f);
  std::size_t index = 0;
  for (std::size_t i = 0; i < subdivs; i++) {
    float const angle =
        (i + 1) / float(subdivs) * 2.f * std::numbers::pi_v<float>;

    float y = std::sin(angle);
    float z = std::cos(angle);

    // Normals are slanted upwards towards X axis by 45 degrees. Normalize
    // vector sum of (1, 0, 0) and (0, y, z)

    // Cone top chosen to have normal equal to face normal
    norm[index] =
        normalize(Vec3f{1.f, 0.f, 0.f} +
                  normalize(Vec3f{0.f, (prevY + y) / 2, (prevZ + z) / 2}));
    pos[index++] = Vec3f{1.f, 0.f, 0.f};
    norm[index] = normalize(Vec3f{1.f, y, z});
    pos[index++] = Vec3f{0.f, y, z};
    norm[index] = normalize(Vec3f{1.f, prevY, prevZ});
    pos[index++] = Vec3f{0.f, prevY, prevZ};

    if (capped) {
      norm[index] = Vec3f{-1.f, 0.f, 0.f};
      pos[index++] = Vec3f{0.f, 0.f, 0.f};
      norm[index] = Vec3f{-1.f, 0.f, 0.f};
      pos[index++] = Vec3f{0.f, prevY, prevZ};
      norm[index] = Vec3f{-1.f, 0.f, 0.f};
      pos[index++] = Vec3f{0.f, y, z};
    }
    prevY = y;
    prevZ = z;
  }

  for (auto& p : pos) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = preTransform * p4;
    t /= t.w;
    p = Vec3f{t.x, t.y, t.z};
  }

  Mat33f const N = mat44_to_mat33(transpose(invert(preTransform)));
  for (auto& n : norm) {
    n = N * n;
  }

  std::vector<Vec3f> ambientV(numVertices, ambient);
  std::vector<Vec3f> diffuseV(numVertices, diffuse);
  std::vector<Vec3f> specularV(numVertices, specular);
  std::vector<float> shininessV(numVertices, shininess);
  std::vector<Vec3f> emissiveV(numVertices, emissive);

  return MeshData{
      std::move(pos),        std::move(norm),     {},
      std::move(ambientV),   std::move(diffuseV), std::move(specularV),
      std::move(shininessV), std::move(emissiveV)};
}

MeshData make_sphere(std::size_t subdivLoops, const Mat44f& preTransform,
                     const Vec3f& ambient, const Vec3f& diffuse,
                     const Vec3f& specular, float shininess,
                     const Vec3f& emissive) {
  // Make by subdividing a shape, and projecting onto sphere

  // Start with triangular icosohedron
  float phi = (1.f + sqrt(5.f)) * 0.5f;
  float a = 1.f / sqrtf(1.f + 1.f / powf(phi, 2.f));
  float b = a / phi;

  // add vertices
  std::vector<Vec3f> points = {{0, b, -a},  {b, a, 0},  {-b, a, 0},
                               {0, b, a},   {0, -b, a}, {-a, 0, b},
                               {0, -b, -a}, {a, 0, -b}, {a, 0, b},
                               {-a, 0, -b}, {b, -a, 0}, {-b, -a, 0}};

  std::vector<std::array<std::size_t, 3>> faces = {
      {2, 1, 0}, {1, 2, 3},   {5, 4, 3},   {4, 8, 3},  {7, 6, 0},
      {6, 9, 0}, {11, 10, 4}, {10, 11, 6}, {9, 5, 2},  {5, 9, 11},
      {8, 7, 1}, {7, 8, 10},  {2, 5, 3},   {8, 1, 3},  {9, 2, 0},
      {1, 7, 0}, {11, 9, 6},  {7, 10, 6},  {5, 11, 4}, {10, 8, 4}};

  // Perform sub division
  for (std::size_t i = 0; i < subdivLoops; i++) {
    std::size_t point_i = points.size();
    std::size_t face_i = 0;
    std::vector<std::array<std::size_t, 3>> newFaces(faces.size() * 4);
    points.resize(points.size() + faces.size() * 3);
    for (const auto& f : faces) {
      Vec3f v0 = points[f[0]];
      Vec3f v1 = points[f[1]];
      Vec3f v2 = points[f[2]];

      // Add midpoints to points
      points[point_i++] = normalize((v0 + v1) / 2);
      points[point_i++] = normalize((v1 + v2) / 2);
      points[point_i++] = normalize((v2 + v0) / 2);

      // Create new faces (ensuring forward facing)
      newFaces[face_i++] = {f[0], point_i - 3, point_i - 1};
      newFaces[face_i++] = {f[1], point_i - 2, point_i - 3};
      newFaces[face_i++] = {f[2], point_i - 1, point_i - 2};
      newFaces[face_i++] = {point_i - 3, point_i - 2, point_i - 1};
    }
    faces = newFaces;
  }

  // Convert to triangle soup mesh structure
  std::size_t numVertices = 3 * faces.size();
  std::vector<Vec3f> pos(numVertices);
  std::size_t index = 0;
  for (const auto& f : faces) {
    pos[index++] = points[f[0]];
    pos[index++] = points[f[1]];
    pos[index++] = points[f[2]];
  }

  // Normals are equal to positions!!
  std::vector<Vec3f> norm(pos);

  // Apply the transformation to all vertices
  for (auto& p : pos) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = preTransform * p4;
    t /= t.w;
    p = Vec3f{t.x, t.y, t.z};
  }

  Mat33f const N = mat44_to_mat33(transpose(invert(preTransform)));
  for (auto& n : norm) {
    n = N * n;
  }
  // Return with constant colouring
  std::vector<Vec3f> ambientV(numVertices, ambient);
  std::vector<Vec3f> diffuseV(numVertices, diffuse);
  std::vector<Vec3f> specularV(numVertices, specular);
  std::vector<float> shininessV(numVertices, shininess);
  std::vector<Vec3f> emissiveV(numVertices, emissive);

  return MeshData{
      std::move(pos),        std::move(norm),     {},
      std::move(ambientV),   std::move(diffuseV), std::move(specularV),
      std::move(shininessV), std::move(emissiveV)};
}