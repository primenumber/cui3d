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

double norm(const Vec3D &v) {
  return dot(v, v);
}

double abs(const Vec3D &v) {
  return std::sqrt(norm(v));
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
  Vec3D orthogonal = (tri[1] - tri[0]) * (tri[2] - tri[0]);
  for (int i = 0; i < 3; ++i) {
    auto a = tri[(i+1)%3] - tri[i], q = p - tri[i];
    double c1 = dot(a*q, orthogonal);
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

Transform3D rotateX(const double theta) {
  Transform3D rot;
  rot[1][1] = rot[2][2] = std::cos(theta);
  rot[1][2] = -std::sin(theta);
  rot[2][1] = std::sin(theta);
  return rot;
}
Transform3D rotateY(const double theta) {
  Transform3D rot;
  rot[0][0] = rot[2][2] = std::cos(theta);
  rot[0][2] = std::sin(theta);
  rot[2][0] = -std::sin(theta);
  return rot;
}
Transform3D rotateZ(const double theta) {
  Transform3D rot;
  rot[0][0] = rot[1][1] = std::cos(theta);
  rot[0][1] = -std::sin(theta);
  rot[1][0] = std::sin(theta);
  return rot;
}
Transform3D scaleX(const double scale) {
  Transform3D scl;
  scl[0][0] = scale;
  return scl;
}
Transform3D scaleY(const double scale) {
  Transform3D scl;
  scl[1][1] = scale;
  return scl;
}
Transform3D scaleZ(const double scale) {
  Transform3D scl;
  scl[2][2] = scale;
  return scl;
}
Transform3D scale_all(const double scale) {
  Transform3D scl;
  for (std::size_t i = 0; i < 3; ++i)
    scl[i][i] = scale;
  return scl;
}
Transform3D scaleXYZ(const double scaleX, const double scaleY,
    const double scaleZ) {
  Transform3D scl;
  scl[0][0] = scaleX;
  scl[1][1] = scaleY;
  scl[2][2] = scaleZ;
  return scl;
}
Transform3D translateX(const double dist) {
  Transform3D trans;
  trans[0][3] = dist;
  return trans;
}
Transform3D translateY(const double dist) {
  Transform3D trans;
  trans[1][3] = dist;
  return trans;
}
Transform3D translateZ(const double dist) {
  Transform3D trans;
  trans[2][3] = dist;
  return trans;
}
Transform3D translateXYZ(const double distX, const double distY,
    const double distZ) {
  Transform3D trans;
  trans[0][3] = distX;
  trans[1][3] = distY;
  trans[2][3] = distZ;
  return trans;
}

Vec3D applyTransform(const Vec3D &vec, const Transform3D &trans) {
  Vec3D res(0, 0, 0);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 4; ++j)
      res[i] += trans[i][j] * vec[j];
  return res;
}

Triangle applyTransform(const Triangle &tri, const Transform3D &trans) {
  Triangle res = tri;
  for (int i = 0; i < 3; ++i)
    res[i] = applyTransform(tri[i], trans);
  return res;
}

} // namespace cui3d
