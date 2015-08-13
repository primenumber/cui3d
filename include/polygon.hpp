#ifndef _HEADER_CUI3D_POLYGON_HPP_
#define _HEADER_CUI3D_POLYGON_HPP_
#include "cui3d.hpp"
#include <cstddef>
#include <array>
#include <vector>

namespace cui3d {

class Vec3D {
 public:
  Vec3D() {};
  Vec3D(double x, double y, double z) : vec{x, y, z} {}
  double &operator[](std::size_t index) { return vec[index]; }
  const double &operator[](std::size_t index) const { return vec[index]; }
 private:
  std::array<double, 3> vec;
};

Vec3D operator+(const Vec3D &, const Vec3D &);
Vec3D operator-(const Vec3D &, const Vec3D &);
Vec3D operator-(const Vec3D &);
Vec3D operator*(const Vec3D &, const Vec3D &);

struct Triangle {
  Vec3D &operator[](std::size_t index) { return verticies[index]; }
  const Vec3D &operator[](std::size_t index) const { return verticies[index]; }
  std::array<Vec3D, 3> verticies;
  Triangle(Vec3D a, Vec3D b, Vec3D c)
    : verticies{a, b, c} {}
};

struct Polygon {
  template <typename T>
  using table = std::vector<std::vector<T>>;
  std::vector<Triangle> triangles;
  table<char> texture;
};

class Camera {
 public:
  Camera() : camera_pos(0, 0, -1.0), camera_direction(0, 0, 1.0) {};
  CuiImage render(CuiImage &, const Polygon &);
  CuiImage render(CuiImage &, const std::vector<Polygon> &);
  Vec3D camera_pos;
  Vec3D camera_direction;
 private:
};

} // namespace cui3d
#endif
