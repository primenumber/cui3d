#include "polygon.hpp"
#include <cmath>
#include <bitset>
#include <complex>
#include <boost/optional.hpp>

namespace cui3d {

Polygon make_cuboid(const Vec3D &begin, const Vec3D &end) {
  Polygon res;
  std::array<Vec3D, 8> verticies;
  for (int i = 0; i < 8; ++i) {
    std::bitset<3> be(i);
    Vec3D p;
    for (int j = 0; j < 3; ++j) {
      p[j] = be[j] ? begin[j] : end[j];
    }
    verticies[i] = p;
  }
  for (int i = 0; i < 3; ++i) {
    int a = 1<<i, b = 1<<((i+1)%3);
    for (int j = 0; j < 2; ++j) {
      int c = (0x7^(a|b))*j;
      res.triangles.emplace_back(verticies[c], verticies[a+c], verticies[b+c]);
      res.triangles.emplace_back(verticies[a+b+c], verticies[a+c], verticies[b+c]);
    }
  }
  return res;
}

Polygon applyTransform(const Polygon &p, const Transform3D &trans) {
  Polygon res;
  res.texture = p.texture;
  for (const auto &tri : p.triangles)
    res.triangles.emplace_back(applyTransform(tri, trans));
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

std::vector<Vec3D> cross(const Polygon &p, const Line &l) {
  std::vector<Vec3D> v;
  for (const auto &tri : p.triangles) {
    if (auto op = cross(tri, l)) {
      v.emplace_back(*op);
    }
  }
  return v;
}

CuiImage Camera::render(CuiImage &img, const std::vector<Polygon> &vp) {
  std::vector<std::vector<double>> depth(img.height,
      std::vector<double>(img.width, 1e+8));
  auto basis = orthonormal_basis(camera_direction);
  for (int i = 0; i < img.height; ++i) {
    for (int j = 0; j < img.width; ++j) {
      double x = (double)j / img.width - 0.5;
      double y = (double)i / img.height - 0.5;
      double scale = abs(camera_direction);
      Line l(camera_pos,
          camera_pos + camera_direction + x * basis[0] + y * basis[1]);
      for (int k = 0; k < vp.size(); ++k) {
        auto vc = cross(vp[k], l);
        for (auto p : vc) {
          double dep = abs(p - camera_pos);
          if (dep < depth[i][j]) {
            img.data[i][j] = vp[k].texture(p);
            img.visible[i][j] = true;
            depth[i][j] = dep;
          }
        }
      }
    }
  }
  return img;
}

} // namespace cui3d
