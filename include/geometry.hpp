#ifndef _HEADER_CUI3D_GEOMETRY_HPP_
#define _HEADER_CUI3D_GEOMETRY_HPP_
#include <array>
#include <vector>
#include <boost/optional.hpp>

namespace cui3d {

constexpr double pi = 3.1415926535897932384646;

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

} // namespace cui3d

#endif
