#include "polygon.hpp"
#include <cmath>
#include <bitset>
#include <complex>

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

using P = std::complex<double>;
using VP = std::vector<P>;
using VVP = std::vector<VP>;

bool is_in_polygon(double x, double y, const VP &t) {
  std::bitset<3> positive;
  for (int i = 0; i < 3; ++i) {
    P p(x, y);
    p -= t[i];
    P b = t[(i+1)%3] - t[i];
    double cross = std::imag(std::conj(b)*p);
    positive[i] = cross > 0;
    if (std::abs(cross) < 1e-8) return false;
  }
  return positive.all() || positive.none();
}

bool is_in_polygon(double x, double y, const VVP &p) {
  for (const auto &tri : p) {
    if (is_in_polygon(x, y, tri)) return true;
  }
  return false;
}

VVP convert2d(const Polygon &p, const Vec3D &camera_pos) {
  VVP res;
  for (auto tri : p.triangles) {
    bool visible = true;
    VP res_t;
    for (auto &v : tri.verticies) {
      v = v - camera_pos;
      if (v[2] <= 0.0) {
        visible = false;
        break;
      }
      v[0] /= v[2] / 2.0;
      v[1] /= v[2] / 2.0;
      res_t.emplace_back(v[0] / v[2] * 2.0, v[1] / v[2] * 2.0);
    }
    if (visible) res.push_back(res_t);
  }
  return res;
}

CuiImage Camera::render(CuiImage &img, const Polygon &p) {
  auto poly2d = convert2d(p, camera_pos);
  for (int i = 0; i < img.height; ++i) {
    for (int j = 0; j < img.width; ++j) {
      double x = (double)j / img.width - 0.5;
      double y = (double)i / img.height - 0.5;
      if (is_in_polygon(x, y, poly2d)) {
        img.data[i][j] = p.texture[i%p.texture.size()][j%p.texture[0].size()];
        img.visible[i][j] = true;
      }
    }
  }
  return img;
}

CuiImage Camera::render(CuiImage &img, const std::vector<Polygon> &vp) {
  std::vector<VVP> poly2dv;
  for (auto &p : vp) poly2dv.emplace_back(convert2d(p, camera_pos));
  for (int i = 0; i < img.height; ++i) {
    for (int j = 0; j < img.width; ++j) {
      double x = (double)j / img.width - 0.5;
      double y = (double)i / img.height - 0.5;
      for (int k = 0; k < vp.size(); ++k) {
        if (is_in_polygon(x, y, poly2dv[k])) {
          img.data[i][j] = vp[k].texture[i%vp[k].texture.size()][j%vp[k].texture[0].size()];
          img.visible[i][j] = true;
          break;
        }
      }
    }
  }
  return img;
}

} // namespace cui3d
