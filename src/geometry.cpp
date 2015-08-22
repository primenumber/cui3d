#include "geometry.hpp"
#include <cmath>
#include <bitset>

namespace cui3d {

Vec3D operator+(const Vec3D &lhs, const Vec3D &rhs) {
  Vec3D res;
  for (int i = 0; i < 3; ++i) res[i] = lhs[i] + rhs[i];
  return res;
}

Vec3D operator-(const Vec3D &lhs, const Vec3D &rhs) {
  Vec3D res;
  for (int i = 0; i < 3; ++i) res[i] = lhs[i] - rhs[i];
  return res;
}

Vec3D operator*(const Vec3D &lhs, const Vec3D &rhs) {
  Vec3D res;
  for (int i = 0; i < 3; ++i) {
    int j=(i+1)%3, k=(i+2)%3;
    res[i] = lhs[j]*rhs[k] - lhs[k]*rhs[j];
  }
  return res;
}

Vec3D operator*(const double lhs, const Vec3D &rhs) {
  Vec3D res;
  for (int i = 0; i < 3; ++i) res[i] = lhs * rhs[i];
  return res;
}

double dot(const Vec3D &lhs, const Vec3D &rhs) {
  double res = 0;
  for (int i = 0; i < 3; ++i) res += lhs[i] * rhs[i];
  return res;
}

double abs(const Vec3D &v) {
  return std::sqrt(dot(v, v));
}

Vec3D normalize(const Vec3D &v) {
  double norm = abs(v);
  return (1.0 / norm) * v;
}

Vec3D cross(const Plane &lhs, const Line &rhs) {
  double t = dot(lhs.offset - rhs.a, lhs.normal)
    / dot(rhs.b - rhs.a, lhs.normal);
  return t * (rhs.b - rhs.a) + rhs.a;
}

std::array<Vec3D, 3> orthonormal_basis(const Vec3D &vec) {
  using std::cos;
  using std::sin;
  double theta = std::atan2(vec[2], vec[0]);
  Vec3D bz = normalize(vec);
  Vec3D bx(sin(theta), 0, -cos(theta));
  Vec3D by = bz * bx;
  return {bx, by, bz};
}

bool is_in_triangle_impl(const Triangle &tri, const Vec3D &p) {
  std::bitset<3> bs;
  Vec3D normal = normalize((tri[1] - tri[0]) * (tri[2] - tri[0]));
  for (int i = 0; i < 3; ++i) {
    auto a = tri[(i+1)%3] - tri[i], q = p - tri[i];
    double c1 = dot(a*q, normal);
    if (std::abs(c1) < 1e-8) return false;
    bs[i] = c1 > 0;
  }
  return bs.all() || bs.none();
}

boost::optional<Vec3D> cross(const Triangle &lhs, const Line &rhs) {
  Vec3D p = cross(Plane(lhs), rhs);
  if (is_in_triangle_impl(lhs, p)) {
    return boost::optional<Vec3D>(p);
  } else {
    return boost::none;
  }
}

} // namespace cui3d
