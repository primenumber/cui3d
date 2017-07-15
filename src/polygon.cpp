#include "polygon.hpp"
#include <cmath>
#include <bitset>
#include <complex>
#include <future>
#include <thread>
#include <iostream>
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

bool is_on_segment(const Vec3D &p, const Line &l) {
  return abs(l.a-l.b) + 1e-8 > abs(p-l.a) + abs(p-l.b);
}

struct Line2D {
  P a, b;
};

boost::optional<Line2D> render_impl(
    const Triangle &tri, const Plane &plane, const std::array<Vec3D, 3> &basis,
    const Vec3D &camera_pos, const Vec3D &camera_direction) {
  VP cross_point;
  for (int i = 0; i < 3; ++i) {
    Line l(tri[i], tri[(i+1)%3]);
    Vec3D c = cross(plane, l);
    if (is_on_segment(c, l)) {
      double x = dot(c - camera_pos, basis[0]);
      double z = dot(c - camera_pos, camera_direction);
      if (z > 0) {
        cross_point.emplace_back(x/z, z);
      }
    }
  }
  std::sort(std::begin(cross_point), std::end(cross_point),
      [](P l, P r){ return l.real() < r.real(); });
  if (cross_point.size() == 2) {
    return (Line2D){cross_point[0], cross_point[1]};
  }
  return boost::none;
}

double linear_interpolation(Line2D line, double x) {
  double scale = (x - line.a.real()) / (line.b.real() - line.a.real());
  return scale * (line.b.imag() - line.a.imag()) + line.a.imag();
}

CuiImage Camera::render_line(CuiImage &img, const std::vector<Polygon> &vp, const int i) const {
  std::vector<double> depth(img.width, 1e+8);
  double y = (double)i / img.height - 0.5;
  auto basis = orthonormal_basis(camera_direction);
  double scale = abs(camera_direction);
  Plane plane(Triangle(camera_pos,
        camera_pos + camera_direction + -0.5 * basis[0] + y * basis[1],
        camera_pos + camera_direction + 0.5 * basis[0] + y * basis[1]));
  for (const Polygon &poly : vp) {
    for (const Triangle &tri : poly.triangles) {
      if (auto opt_segment = render_impl(tri, plane, basis, camera_pos, camera_direction)) {
        auto segment = *opt_segment;
        for (int j = std::max(0.0, (segment.a.real() + 0.5) * img.width);
            j < std::min((double)img.width, (segment.b.real() + 0.5) * img.width); ++j) {
          double x = (double)j / img.width - 0.5;
          double dep = linear_interpolation(segment, x);
          if (dep < depth[j]) {
            Vec3D p = camera_pos + dep * camera_direction + dep * x * basis[0] + dep * y * basis[1];
            depth[j] = dep;
            img.data[i][j] = poly.texture(p);
            img.visible[i][j] = true;
          }
        }
      }
    }
  }
  return img;
}

CuiImage Camera::render(CuiImage &img, const std::vector<Polygon> &vp) const {
  std::vector<std::vector<double>> depth(img.height,
      std::vector<double>(img.width, 1e+8));
  size_t th = std::thread::hardware_concurrency();
  if (th == 1) {
    for (int i = 0; i < img.height; ++i) {
      render_line(img, vp, i);
    }
  } else {
    std::vector<std::future<void>> vf;
    for (size_t t = 0; t < th; ++t) {
      vf.push_back(std::async(std::launch::async, [=](CuiImage &img, const std::vector<Polygon> &vp, const size_t t) {
            for (int i = t; i < img.height; i += th) {
              render_line(img, vp, i);
            }
          }, std::ref(img), std::cref(vp), t));
    }
    for (auto && f : vf) f.get();
  }
  return img;
}

} // namespace cui3d
