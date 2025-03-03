#ifndef SHAPE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29
#define SHAPE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29

#include <cstdlib>
#include <vector>

#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/vec3.hpp"
#include "mesh.hpp"

MeshData make_cylinder(bool capped, const std::size_t subdivs,
                       const Mat44f &preTransform, const Vec3f &ambient,
                       const Vec3f &diffuse, const Vec3f &specular,
                       float shininess, const Vec3f &emissive);

MeshData make_cone(bool capped, const std::size_t subdivs,
                   const Mat44f &preTransform, const Vec3f &ambient,
                   const Vec3f &diffuse, const Vec3f &specular, float shininess,
                   const Vec3f &emissive);

MeshData make_sphere(std::size_t subdivLoops, const Mat44f &preTransform,
                     const Vec3f &ambient, const Vec3f &diffuse,
                     const Vec3f &specular, float shininess,
                     const Vec3f &emissive);

#endif
