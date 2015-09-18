#ifndef _HEADER_CUI3D_GEOMETRY_HPP_
#define _HEADER_CUI3D_GEOMETRY_HPP_
#include <array>
#include <vector>
#include <boost/optional.hpp>
#include "matrix.hpp"

namespace cui3d {

constexpr double pi = 3.1415926535897932384646;

class Vec3D {
 public:
  Vec3D() {};
  Vec3D(double x, double y, double z) : vec{x, y, z} {}
  double &operator[](std::size_t index) { return vec[index]; }
  const double &operator[](std::size_t index) const { return vec[index]; }
 private:
  alignas(32) std::array<double, 4> vec;
};

Vec3D operator+(const Vec3D &, const Vec3D &);
Vec3D operator-(const Vec3D &, const Vec3D &);
Vec3D operator-(const Vec3D &);
// cross
Vec3D operator*(const Vec3D &, const Vec3D &);
// scalar
Vec3D operator*(const double, const Vec3D &);
double dot(const Vec3D &, const Vec3D &);
double abs(const Vec3D &);
Vec3D normalize(const Vec3D &);
std::array<Vec3D, 3> orthonormal_basis(const Vec3D &vec);

struct Line {
  Vec3D a, b;
  Line(const Vec3D &a, const Vec3D &b) : a(a), b(b) {}
};

struct Triangle {
  Vec3D &operator[](std::size_t index) { return verticies[index]; }
  const Vec3D &operator[](std::size_t index) const { return verticies[index]; }
  std::array<Vec3D, 3> verticies;
  Triangle(Vec3D a, Vec3D b, Vec3D c)
    : verticies{a, b, c} {}
};

struct Plane {
  Vec3D normal;
  Vec3D offset;
  Plane(const Vec3D &normal, const Vec3D &offset)
    : normal(normal), offset(offset) {}
  Plane(const Triangle &tri)
    : normal(normalize((tri[1]-tri[0])*(tri[2]-tri[0]))),
      offset(tri[0]) {}
};

Vec3D cross(const Plane &, const Line &);
boost::optional<Vec3D> cross(const Triangle &, const Line &);

using Transform3D = Matrix<4, 4>;
Transform3D rotateX(const double theta);
Transform3D rotateY(const double theta);
Transform3D rotateZ(const double theta);
Transform3D scaleX(const double scale);
Transform3D scaleY(const double scale);
Transform3D scaleZ(const double scale);
Transform3D scale_all(const double scale);
Transform3D scaleXYZ(const double scaleX, const double scaleY,
    const double scaleZ);
Transform3D translateX(const double dist);
Transform3D translateY(const double dist);
Transform3D translateZ(const double dist);
Transform3D translateXYZ(const double distX, const double distY,
    const double distZ);

Vec3D applyTransform(const Vec3D&, const Transform3D &);
Triangle applyTransform(const Triangle &, const Transform3D &);

} // namespace cui3d

#endif
